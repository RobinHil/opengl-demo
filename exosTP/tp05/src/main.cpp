#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <glengine/orbitalCamera.hpp>
#include <imgui.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "tp05/config.hpp"

void framebuffer_size_callback(GLFWwindow*, int, int);
void processInput(GLFWwindow *);
std::string readShaderFile(const char*);
void onMouseButton(GLFWwindow*, int, int, int);
void onMouseMove(GLFWwindow*, double, double);
void onMouseScroll(GLFWwindow*, double, double);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 10.0f;

bool firstMouse = true;
float lastX;
float lastY;

enum class MousePressedButton { NONE, LEFT, RIGHT, MIDDLE };
MousePressedButton mouseButtonState = MousePressedButton::NONE;

GLEngine::OrbitalCamera orbitalCamera(glm::vec3(0.3f, 0.4f, 3.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    ImGui::CreateContext();

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL - TP5", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetScrollCallback(window, onMouseScroll);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    std::string vertexShaderPath = std::string(_resources_directory).append("shader/phong.vert");
    std::string vertexShaderSource = readShaderFile(vertexShaderPath.c_str());

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderCode = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    std::string fragmentShaderPath = std::string(_resources_directory).append("shader/phong.frag");
    std::string fragmentShaderSource = readShaderFile(fragmentShaderPath.c_str());
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderCode = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    std::vector<float> vertices = {
        // position            // colors         //edges of the cube
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    glBindVertexArray(0); 

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));

        glm::vec3 cameraPos = orbitalCamera.getPosition();
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));

        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));

        glm::vec3 objectColor(0.5f, 1.0f, 0.5f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, glm::value_ptr(objectColor));
        glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), 32.0f);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(28.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 view = glm::mat4(1.0f);
        view = orbitalCamera.getViewMatrix();
        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(orbitalCamera.getFov(), (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);
        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        float line_size = 0.05;
        GLint lineSizeLoc = glGetUniformLocation(shaderProgram, "line_size");
        glUniform1f(lineSizeLoc, line_size);
        glm::vec4 line_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
        GLint lineColorLoc = glGetUniformLocation(shaderProgram, "line_color");
        glUniform4f(lineColorLoc, line_color.r, line_color.g, line_color.b, line_color.a);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

std::string readShaderFile(const char* filePath)
{
    std::ifstream shaderFile;
    std::stringstream shaderStream;

    shaderFile.open(filePath);
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    return shaderStream.str();
}

void onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
  if (action == GLFW_RELEASE)
  {
      mouseButtonState = MousePressedButton::NONE;
  }
  else
  {
    switch (button)
    {
      case GLFW_MOUSE_BUTTON_LEFT: mouseButtonState = MousePressedButton::LEFT;
        break;
      case GLFW_MOUSE_BUTTON_RIGHT: mouseButtonState = MousePressedButton::RIGHT;
        break;
      case GLFW_MOUSE_BUTTON_MIDDLE: mouseButtonState = MousePressedButton::MIDDLE;
        break;
    }
  }
}

void onMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (mouseButtonState == MousePressedButton::NONE)
  {
    lastX = (float)xpos;
    lastY = (float)ypos;
  }
  else
  {
    if (firstMouse)
    {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;

    lastX = (float)xpos;
    lastY = (float)ypos;

    switch (mouseButtonState)
    {
      case MousePressedButton::LEFT: orbitalCamera.orbit(xoffset, yoffset);
        break;
      case MousePressedButton::RIGHT:
        orbitalCamera.track(xoffset);
        orbitalCamera.pedestal(yoffset);
        break;
      case MousePressedButton::MIDDLE: orbitalCamera.dolly(yoffset);
        break;
      case MousePressedButton::NONE:
        break;
    }
  }
}

void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  if (!ImGui::GetIO().WantCaptureMouse)
  {
    orbitalCamera.zoom((float)yoffset);
  }
}