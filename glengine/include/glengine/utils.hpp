#ifndef GLENGINE_UTILS_HPP
#define GLENGINE_UTILS_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace GLEngine {
    std::string readFile(const char* filePath);
    
    void loadObjFile(const char* filePath, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
    void computeNormal(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, std::vector<glm::vec3>& normal);
    
    void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    void processInput(GLFWwindow* window);
}

#endif