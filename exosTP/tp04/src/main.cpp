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

#include "tp04/config.hpp"

void framebuffer_size_callback(GLFWwindow*, int, int);
void processInput(GLFWwindow *);
std::string readShaderFile(const char*);
void onMouseButton(GLFWwindow*, int, int, int);
void onMouseMove(GLFWwindow*, double, double);
void onMouseScroll(GLFWwindow*, double, double);
void loadObjFile(const char*, std::vector<glm::vec3>&, std::vector<unsigned int>&);
void computeNormal(const std::vector<glm::vec3>&, const std::vector<unsigned int>&, std::vector<glm::vec3>&);

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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL - TP4", NULL, NULL);
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

    // std::string objectPath = std::string(_resources_directory).append("object/bunny.obj");
    // std::string objectPath = std::string(_resources_directory).append("object/dragon2_small.obj");
    std::string objectPath = std::string(_resources_directory).append("object/dragon2_smooth.obj");

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    loadObjFile(objectPath.c_str(), vertices, indices);

    std::vector<glm::vec3> normals;
    computeNormal(vertices, indices, normals);

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

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
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

        glm::vec3 lightPos(3.0f, 1.0f, 3.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &orbitalCamera.getPosition()[0]);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.8f, 0.8f, 0.8f);
        glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), 255.0f);

        glBindVertexArray(VAO);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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

void loadObjFile(const char* filePath, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices)
{
    std::ifstream objFile(filePath);
    std::string line;
    while (std::getline(objFile, line))
    {
        std::istringstream lineStream(line);
        std::string lineType;
        lineStream >> lineType;
        if (lineType == "v")
        {
            glm::vec3 vertex;
            lineStream >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (lineType == "f")
        {
            unsigned int index;
            for (int i = 0; i < 3; i++)
            {
                lineStream >> index;
                indices.push_back(index - 1);
            }
        }
    }
}

void computeNormal(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, std::vector<glm::vec3>& normal)
{
    // init normal vector
    for (std::size_t k = 0; k < vertices.size(); k++)
    {
        normal.push_back(glm::vec3(0));
    }

    // Compute normal
    for (unsigned int k = 0; k < indices.size() - 3; k += 3)
    {
        unsigned int i1 = indices[k];
        unsigned int i2 = indices[k+1];
        unsigned int i3 = indices[k+2];

        // Vertices
        glm::vec3 v1 = vertices[i1];
        glm::vec3 v2 = vertices[i2];
        glm::vec3 v3 = vertices[i3];

        // Compute normal
        glm::vec3 n = glm::cross(v3 - v1, v3 - v2);

        for (std::size_t di = 0; di < 3; di++)
        {
            normal[i1][di] += n[di];
            normal[i2][di] += n[di];
            normal[i3][di] += n[di];
        }
    }

    // Normalize
    for (std::size_t k = 0; k < normal.size(); k++)
    {
        normal[k] = glm::normalize(normal[k]);
    }
}