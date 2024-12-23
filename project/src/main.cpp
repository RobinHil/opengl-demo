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
    std::string objectPath = std::string(_resources_directory).append("object/dragon2_smooth.obj");
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    GLEngine::loadObjFile(objectPath.c_str(), vertices, indices);

    std::vector<glm::vec3> normals;
    GLEngine::computeNormal(vertices, indices, normals);

    // Setup vertex buffers and attributes
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (vertices.size() + normals.size()) * sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), normals.size() * sizeof(glm::vec3), &normals[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(vertices.size() * sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        GLEngine::processInput(window);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        glm::mat4 view = orbitalCamera.getViewMatrix();
        shader.setMat4("view", view);

        glm::mat4 projection = glm::perspective(orbitalCamera.getFov(), 
            (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);
        shader.setMat4("projection", projection);

        glm::vec3 lightPos(3.0f, 1.0f, 3.0f);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", orbitalCamera.getPosition());
        shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec3("objectColor", glm::vec3(0.8f, 0.8f, 0.8f));
        shader.setFloat("shininess", 255.0f);

        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::ShowDemoWindow();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

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

void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    if (!ImGui::GetIO().WantCaptureMouse) {
        orbitalCamera.zoom((float)yoffset);
    }
}