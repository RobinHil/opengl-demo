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

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 10.0f;

bool firstMouse = true;
float lastX;
float lastY;

enum class MousePressedButton { NONE, LEFT, RIGHT, MIDDLE };
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
    std::string vertexPath = std::string(_resources_directory).append("shader/phong.vert");
    std::string fragmentPath = std::string(_resources_directory).append("shader/phong.frag");
    GLEngine::Shader shader(vertexPath.c_str(), fragmentPath.c_str());

    // Load 3D model
    std::string objectsDir = std::string(_resources_directory).append("object/");
    std::vector<std::string> objFiles = GLEngine::getObjFiles(objectsDir);
    
    static int currentItem = 0;
    std::string currentObjPath = objectsDir + objFiles[currentItem];
    
    GLEngine::Mesh currentMesh = GLEngine::loadMesh(currentObjPath);

    // Initialize grid
    std::string gridVertPath = std::string(_resources_directory).append("shader/grid.vert");
    std::string gridFragPath = std::string(_resources_directory).append("shader/grid.frag");
    GLEngine::Shader gridShader(gridVertPath.c_str(), gridFragPath.c_str());
    GLEngine::Grid3D grid(1.0f, 0.2f);

    // ImGui variables
    static float lightPos[3] = {3.0f, 1.0f, 3.0f};
    static float objectColor[3] = {0.8f, 0.8f, 0.8f};
    static float lightColor[3] = {1.0f, 1.0f, 1.0f};
    static float shininess = 255.0f;
    static float ambientStrength = 0.1f;
    static float specularStrength = 0.5f;
    static bool usePhongLighting = true;
    static float backgroundColor[3] = {0.2f, 0.3f, 0.3f};
    static bool showWireframe = false;
    static bool showGrid = true;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        GLEngine::processInput(window);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        glm::mat4 view = orbitalCamera.getViewMatrix();
        shader.setMat4("view", view);

        glm::mat4 projection = glm::perspective(orbitalCamera.getFov(), 
            (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);
        shader.setMat4("projection", projection);

        shader.setVec3("lightPos", glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
        shader.setVec3("viewPos", orbitalCamera.getPosition());
        shader.setVec3("lightColor", glm::vec3(lightColor[0], lightColor[1], lightColor[2]));
        shader.setVec3("objectColor", glm::vec3(objectColor[0], objectColor[1], objectColor[2]));
        shader.setFloat("shininess", shininess);

        if (showGrid) {
            gridShader.use();
            gridShader.setMat4("view", view);
            gridShader.setMat4("projection", projection);
            gridShader.setMat4("model", glm::mat4(1.0f));
            grid.draw(view, projection);
        }

        glBindVertexArray(currentMesh.VAO);
        
        if (usePhongLighting) {
            shader.use();
            shader.setVec3("lightPos", glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
            shader.setVec3("viewPos", orbitalCamera.getPosition());
            shader.setVec3("lightColor", glm::vec3(lightColor[0], lightColor[1], lightColor[2]));
            shader.setVec3("objectColor", glm::vec3(objectColor[0], objectColor[1], objectColor[2]));
            shader.setFloat("shininess", shininess);
            shader.setFloat("ambientStrength", ambientStrength);
            shader.setFloat("specularStrength", specularStrength);
        } else {
            shader.use();
            shader.setVec3("objectColor", glm::vec3(objectColor[0], objectColor[1], objectColor[2]));
            shader.setVec3("lightColor", glm::vec3(1.0f));
            shader.setFloat("ambientStrength", 1.0f);
            shader.setFloat("specularStrength", 0.0f);
        }

        // Dessiner le modèle
        glPolygonMode(GL_FRONT_AND_BACK, showWireframe ? GL_LINE : GL_FILL);
        glDrawElements(GL_TRIANGLES, currentMesh.indexCount, GL_UNSIGNED_INT, 0);

        // Get current window size
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        
        // Configure ImGui window with current size
        ImGui::SetNextWindowPos(ImVec2(width * 0.75f, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(width * 0.25f, height), ImGuiCond_Always);
        
        // Start the window with flags to make it fixed
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
            ImGui::Checkbox("Phong Lighting", &usePhongLighting);
            ImGui::DragFloat3("Light Position", lightPos, 0.1f);
            ImGui::ColorEdit3("Light Color", lightColor);
            ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 1.0f);
            ImGui::SliderFloat("Specular Strength", &specularStrength, 0.0f, 1.0f);
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
                    cleanupMesh(currentMesh);
                    currentMesh = GLEngine::loadMesh(newObjPath);
                    currentObjPath = newObjPath;
                }
            }
            ImGui::Checkbox("Show Wireframe", &showWireframe);
            ImGui::ColorEdit3("Object Color", objectColor);
            ImGui::SliderFloat("Shininess", &shininess, 1.0f, 256.0f);
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    GLEngine::cleanupMesh(currentMesh);
    grid.cleanup();

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