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

    void loadObjFile(const char* filePath, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, bool& hasTexCoords) {
        std::vector<glm::vec3> temp_positions;
        std::vector<glm::vec2> temp_texcoords;
        std::vector<glm::vec3> temp_normals;
        std::vector<int> vertex_map;
        hasTexCoords = false;

        std::ifstream file(filePath);
        std::string line;
        
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "v") {
                glm::vec3 pos;
                iss >> pos.x >> pos.y >> pos.z;
                temp_positions.push_back(pos);
            }
            else if (type == "vt") {
                glm::vec2 tex;
                iss >> tex.x >> tex.y;
                temp_texcoords.push_back(tex);
                hasTexCoords = true;
            }
            else if (type == "vn") {
                glm::vec3 normal;
                iss >> normal.x >> normal.y >> normal.z;
                temp_normals.push_back(normal);
            }
            else if (type == "f") {
                std::string vertex;
                std::vector<std::string> face_vertices;
                while (iss >> vertex) {
                    face_vertices.push_back(vertex);
                }

                for (int i = 0; i < 3; i++) {
                    std::istringstream vertex_stream(face_vertices[i]);
                    std::string index_str;
                    std::vector<int> face_indices;

                    while (std::getline(vertex_stream, index_str, '/')) {
                        if (index_str.empty()) {
                            face_indices.push_back(0);
                        } else {
                            face_indices.push_back(std::stoi(index_str));
                        }
                    }

                    Vertex vert;
                    vert.position = temp_positions[face_indices[0] - 1];
                    
                    if (face_indices.size() > 1 && face_indices[1] != 0) {
                        vert.texCoords = temp_texcoords[face_indices[1] - 1];
                    } else {
                        vert.texCoords = glm::vec2(0.0f);
                    }
                    
                    if (face_indices.size() > 2 && face_indices[2] != 0) {
                        vert.normal = temp_normals[face_indices[2] - 1];
                    } else {
                        vert.normal = glm::vec3(0.0f);
                    }

                    vertices.push_back(vert);
                    indices.push_back(indices.size());
                }
            }
        }

        if (temp_normals.empty()) {
            computeNormals(vertices, indices);
        }
    }

    void computeNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
        for (auto& vertex : vertices) {
            vertex.normal = glm::vec3(0.0f);
        }

        for (size_t i = 0; i < indices.size(); i += 3) {
            unsigned int i1 = indices[i];
            unsigned int i2 = indices[i + 1];
            unsigned int i3 = indices[i + 2];

            glm::vec3 v1 = vertices[i2].position - vertices[i1].position;
            glm::vec3 v2 = vertices[i3].position - vertices[i1].position;
            glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

            vertices[i1].normal += normal;
            vertices[i2].normal += normal;
            vertices[i3].normal += normal;
        }

        for (auto& vertex : vertices) {
            vertex.normal = glm::normalize(vertex.normal);
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