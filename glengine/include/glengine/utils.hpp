#ifndef GLENGINE_UTILS_HPP
#define GLENGINE_UTILS_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "mesh.hpp"

namespace GLEngine {
    std::string readFile(const char* filePath);
    
    void loadObjFile(const char* filePath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, bool& hasTexCoords);
                     
    void computeNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    
    void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    void processInput(GLFWwindow* window);
}

#endif