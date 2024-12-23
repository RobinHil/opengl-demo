#include <glengine/utils.hpp>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace GLEngine {
    std::string readFile(const char* filePath) {
        std::ifstream file;
        std::stringstream buffer;

        file.open(filePath);
        buffer << file.rdbuf();
        file.close();

        return buffer.str();
    }

    void loadObjFile(const char* filePath, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices) {
        std::ifstream objFile(filePath);
        std::string line;
        while (std::getline(objFile, line)) {
            std::istringstream lineStream(line);
            std::string lineType;
            lineStream >> lineType;
            if (lineType == "v") {
                glm::vec3 vertex;
                lineStream >> vertex.x >> vertex.y >> vertex.z;
                vertices.push_back(vertex);
            }
            else if (lineType == "f") {
                unsigned int index;
                for (int i = 0; i < 3; i++) {
                    lineStream >> index;
                    indices.push_back(index - 1);
                }
            }
        }
    }

    void computeNormal(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, std::vector<glm::vec3>& normal) {
        for (std::size_t k = 0; k < vertices.size(); k++) {
            normal.push_back(glm::vec3(0));
        }

        for (unsigned int k = 0; k < indices.size() - 3; k += 3) {
            unsigned int i1 = indices[k];
            unsigned int i2 = indices[k+1];
            unsigned int i3 = indices[k+2];

            glm::vec3 v1 = vertices[i1];
            glm::vec3 v2 = vertices[i2];
            glm::vec3 v3 = vertices[i3];

            glm::vec3 n = glm::cross(v3 - v1, v3 - v2);

            for (std::size_t di = 0; di < 3; di++) {
                normal[i1][di] += n[di];
                normal[i2][di] += n[di];
                normal[i3][di] += n[di];
            }
        }

        for (std::size_t k = 0; k < normal.size(); k++) {
            normal[k] = glm::normalize(normal[k]);
        }
    }

    void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    void processInput(GLFWwindow* window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }
}