#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include <vector>

#include "project/config.hpp"
#include <glengine/shader.hpp>
#include <glengine/orbitalCamera.hpp>
#include <glengine/utils.hpp>
#include <glengine/mesh.hpp>
#include <glengine/grid3D.hpp>
#include <glengine/cube.hpp>

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const float NEAR_PLANE = 1.0f;
const float FAR_PLANE = 1000.0f;

bool firstMouse = true;
float lastX;
float lastY;

enum class MousePressedButton { NONE, LEFT, RIGHT, MIDDLE };
enum class LightingMode {
    NONE,
    PHONG,
    BLINN_PHONG,
    GAUSSIAN
};

MousePressedButton mouseButtonState = MousePressedButton::NONE;

GLEngine::OrbitalCamera orbitalCamera(glm::vec3(0.3f, 0.4f, 3.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

void onMouseButton(GLFWwindow* window, int button, int action, int mods);
void onMouseMove(GLFWwindow* window, double xpos, double ypos);
void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL - Demonstrator", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, GLEngine::framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetScrollCallback(window, onMouseScroll);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    float dpi_scale = 1.0f;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor != nullptr) {
        float xscale, yscale;
        glfwGetMonitorContentScale(monitor, &xscale, &yscale);
        dpi_scale = xscale;
    }
    io.FontGlobalScale = dpi_scale;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // Load and compile shaders
    std::string basicVertPath = std::string(_resources_directory).append("shader/basic/basic.vert");
    std::string basicFragPath = std::string(_resources_directory).append("shader/basic/basic.frag");
    GLEngine::Shader basicShader(basicVertPath.c_str(), basicFragPath.c_str());

    std::string phongVertPath = std::string(_resources_directory).append("shader/phong/phong.vert");
    std::string phongFragPath = std::string(_resources_directory).append("shader/phong/phong.frag");
    GLEngine::Shader phongShader(phongVertPath.c_str(), phongFragPath.c_str());

    std::string blinnPhongVertPath = std::string(_resources_directory).append("shader/blinn-phong/blinn-phong.vert");
    std::string blinnPhongFragPath = std::string(_resources_directory).append("shader/blinn-phong/blinn-phong.frag");
    GLEngine::Shader blinnPhongShader(blinnPhongVertPath.c_str(), blinnPhongFragPath.c_str());

    std::string gaussianVertPath = std::string(_resources_directory).append("shader/gaussian/gaussian.vert");
    std::string gaussianFragPath = std::string(_resources_directory).append("shader/gaussian/gaussian.frag");
    GLEngine::Shader gaussianShader(gaussianVertPath.c_str(), gaussianFragPath.c_str());

    std::string gridVertPath = std::string(_resources_directory).append("shader/grid/grid.vert");
    std::string gridFragPath = std::string(_resources_directory).append("shader/grid/grid.frag");
    GLEngine::Shader gridShader(gridVertPath.c_str(), gridFragPath.c_str());

    std::string normalVertPath = std::string(_resources_directory).append("shader/normal/normal.vert");
    std::string normalGeomPath = std::string(_resources_directory).append("shader/normal/normal.geom");
    std::string normalFragPath = std::string(_resources_directory).append("shader/normal/normal.frag");
    GLEngine::Shader normalShader(normalVertPath.c_str(), normalGeomPath.c_str(), normalFragPath.c_str());

    std::string lightVertPath = std::string(_resources_directory).append("shader/light/light.vert");
    std::string lightFragPath = std::string(_resources_directory).append("shader/light/light.frag");
    GLEngine::Shader lightShader(lightVertPath.c_str(), lightFragPath.c_str());

    std::string objectsDir = std::string(_resources_directory).append("object/");
    std::vector<std::string> objFiles = GLEngine::Mesh::getObjFiles(objectsDir);
    
    static int currentItem = 0;
    std::string currentObjPath = objectsDir + objFiles[currentItem];
    GLEngine::Mesh currentMesh;
    currentMesh.loadFromFile(currentObjPath);

    GLEngine::Grid3D grid(1.0f, 0.2f);
    GLEngine::Cube lightCube(0.1f);
    GLEngine::Shader objectShader = basicShader;

    static float lightPos[3] = {3.0f, 1.0f, 3.0f};
    static float objectColor[3] = {0.8f, 0.8f, 0.8f};
    static float lightColor[3] = {1.0f, 1.0f, 1.0f};
    static float shininess = 256.0f;
    static float ambientStrength = 0.1f;
    static float specularStrength = 0.5f;
    static float backgroundColor[3] = {0.2f, 0.3f, 0.3f};
    static bool showWireframe = false;
    static bool showGrid = true;
    static bool showNormals = false;
    static float normalLength = 0.1f;
    static LightingMode currentLightingMode = LightingMode::PHONG;

    while (!glfwWindowShouldClose(window)) {
        GLEngine::processInput(window);

        std::vector<std::string> objFiles = GLEngine::Mesh::getObjFiles(objectsDir);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = orbitalCamera.getViewMatrix();
        glm::mat4 projection = glm::perspective(orbitalCamera.getFov(), 
            (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);

        // Draw grid if enabled
        if (showGrid) {
            gridShader.use();
            gridShader.setMat4("view", view);
            gridShader.setMat4("projection", projection);
            gridShader.setMat4("model", glm::mat4(1.0f));
            grid.draw(view, projection);
        }
        
        switch (currentLightingMode) {
            case LightingMode::NONE:
                objectShader = basicShader;
                break;
            case LightingMode::PHONG:
                objectShader = phongShader;
                break;
            case LightingMode::BLINN_PHONG:
                objectShader = blinnPhongShader;
                break;
            case LightingMode::GAUSSIAN:
                objectShader = gaussianShader;
                break;
        }

        objectShader.use();
        objectShader.setMat4("model", model);
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);

        switch (currentLightingMode) {
            case LightingMode::NONE:
                basicShader.setVec3("objectColor", glm::vec3(objectColor[0], objectColor[1], objectColor[2]));
                break;

            case LightingMode::PHONG:
                phongShader.setVec3("lightPos", glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
                phongShader.setVec3("viewPos", orbitalCamera.getPosition());
                phongShader.setVec3("lightColor", glm::vec3(lightColor[0], lightColor[1], lightColor[2]));
                phongShader.setVec3("objectColor", glm::vec3(objectColor[0], objectColor[1], objectColor[2]));
                phongShader.setFloat("shininess", shininess);
                phongShader.setFloat("ambientStrength", ambientStrength);
                phongShader.setFloat("specularStrength", specularStrength);
                break;

            case LightingMode::BLINN_PHONG:
                blinnPhongShader.setVec3("lightPos", glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
                blinnPhongShader.setVec3("viewPos", orbitalCamera.getPosition());
                blinnPhongShader.setVec3("lightColor", glm::vec3(lightColor[0], lightColor[1], lightColor[2]));
                blinnPhongShader.setVec3("objectColor", glm::vec3(objectColor[0], objectColor[1], objectColor[2]));
                blinnPhongShader.setFloat("shininess", shininess);
                blinnPhongShader.setFloat("ambientStrength", ambientStrength);
                blinnPhongShader.setFloat("specularStrength", specularStrength);
                break;

            case LightingMode::GAUSSIAN:
                gaussianShader.setVec3("lightPos", glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
                gaussianShader.setVec3("viewPos", orbitalCamera.getPosition());
                gaussianShader.setVec3("lightColor", glm::vec3(lightColor[0], lightColor[1], lightColor[2]));
                gaussianShader.setVec3("objectColor", glm::vec3(objectColor[0], objectColor[1], objectColor[2]));
                gaussianShader.setFloat("shininess", shininess);
                gaussianShader.setFloat("ambientStrength", ambientStrength);
                gaussianShader.setFloat("specularStrength", specularStrength);
                break;
        }

        glPolygonMode(GL_FRONT_AND_BACK, showWireframe ? GL_LINE : GL_FILL);
        currentMesh.draw();

        if (currentLightingMode != LightingMode::NONE) {
            lightShader.use();
            glm::mat4 lightModel = glm::mat4(1.0f);
            lightModel = glm::translate(lightModel, glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
            lightShader.setMat4("model", lightModel);
            lightShader.setMat4("view", view);
            lightShader.setMat4("projection", projection);
            lightShader.setVec3("lightColor", glm::vec3(lightColor[0], lightColor[1], lightColor[2]));
            lightCube.draw();
        }

        if (showNormals) {
            normalShader.use();
            normalShader.setMat4("model", model);
            normalShader.setMat4("view", view);
            normalShader.setMat4("projection", projection);
            normalShader.setFloat("normalLength", normalLength);
            currentMesh.draw();
        }

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        
        // ImGui
        ImGui::SetNextWindowPos(ImVec2(width * 0.75f, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(width * 0.25f, height), ImGuiCond_Always);
        
        ImGui::Begin("Rendering Parameters", nullptr, 
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus
        );

        glm::vec3 camPos = orbitalCamera.getPosition();
        ImGui::Text("Camera position: (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);

        ImGui::ColorEdit3("Background Color", backgroundColor);

        ImGui::Checkbox("Show Grid", &showGrid);
        
        if (ImGui::CollapsingHeader("Light")) {
            const char* lighting_modes[] = { "None", "Phong", "Blinn-Phong", "Gaussian" };
            int current_mode = static_cast<int>(currentLightingMode);
            if (ImGui::Combo("Lighting Mode", &current_mode, lighting_modes, IM_ARRAYSIZE(lighting_modes))) {
                currentLightingMode = static_cast<LightingMode>(current_mode);
            }

            if (
                currentLightingMode == LightingMode::PHONG ||
                currentLightingMode == LightingMode::BLINN_PHONG ||
                currentLightingMode == LightingMode::GAUSSIAN
            ) {
                ImGui::DragFloat3("Light Position", lightPos, 0.1f);
                ImGui::ColorEdit3("Light Color", lightColor);
                ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Specular Strength", &specularStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Shininess", &shininess, 1.0f, 256.0f);
            }
        }

        if (ImGui::CollapsingHeader("Object")) {
            if (ImGui::Combo("3D Model", &currentItem, 
                [](void* data, int idx, const char** out_text) {
                    std::vector<std::string>* files = (std::vector<std::string>*)data;
                    if (idx < 0 || (size_t)idx >= files->size()) return false;
                    *out_text = (*files)[idx].c_str();
                    return true;
                }, 
                &objFiles, objFiles.size())) 
            {
                std::string newObjPath = objectsDir + objFiles[currentItem];
                if (newObjPath != currentObjPath) {
                    currentMesh.loadFromFile(newObjPath);
                    currentObjPath = newObjPath;
                }
            }
            ImGui::Checkbox("Show Wireframe", &showWireframe);
            ImGui::ColorEdit3("Object Color", objectColor);
            ImGui::Checkbox("Show Normals", &showNormals);
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    currentMesh.cleanup();
    grid.cleanup();
    lightCube.cleanup();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void onMouseButton(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_RELEASE) {
        mouseButtonState = MousePressedButton::NONE;
    } else {
        switch (button) {
            case GLFW_MOUSE_BUTTON_LEFT: 
                mouseButtonState = MousePressedButton::LEFT;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT: 
                mouseButtonState = MousePressedButton::RIGHT;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE: 
                mouseButtonState = MousePressedButton::MIDDLE;
                break;
        }
    }
}

void onMouseMove(GLFWwindow* window, double xpos, double ypos) {
    if (!ImGui::GetIO().WantCaptureMouse) {
        if (mouseButtonState == MousePressedButton::NONE) {
            lastX = (float)xpos;
            lastY = (float)ypos;
        } else {
            if (firstMouse) {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = (float)xpos - lastX;
            float yoffset = lastY - (float)ypos;

            lastX = (float)xpos;
            lastY = (float)ypos;

            switch (mouseButtonState) {
                case MousePressedButton::LEFT: 
                    orbitalCamera.orbit(xoffset, yoffset);
                    break;
                case MousePressedButton::RIGHT:
                    orbitalCamera.track(xoffset);
                    orbitalCamera.pedestal(yoffset);
                    break;
                case MousePressedButton::MIDDLE: 
                    orbitalCamera.dolly(yoffset);
                    break;
                case MousePressedButton::NONE:
                    break;
            }
        }
    }
}

void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    if (!ImGui::GetIO().WantCaptureMouse) {
        orbitalCamera.zoom((float)yoffset);
    }
}