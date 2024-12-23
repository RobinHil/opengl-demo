#include <glengine/mesh.hpp>
#include <glengine/utils.hpp>
#include <glad/glad.h>
#include <filesystem>

namespace GLEngine {
    Mesh::Mesh() : VAO(0), VBO(0), EBO(0), indexCount(0) {}
    
    Mesh::~Mesh() {
        cleanup();
    }
    
    std::vector<std::string> Mesh::getObjFiles(const std::string& directory) {
        std::vector<std::string> files;
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.path().extension() == ".obj") {
                files.push_back(entry.path().filename().string());
            }
        }
        return files;
    }
    
    void Mesh::loadFromFile(const std::string& objPath) {
        // Clean up existing resources if any
        cleanup();
        
        std::vector<glm::vec3> vertices;
        std::vector<unsigned int> indices;
        loadObjFile(objPath.c_str(), vertices, indices);

        std::vector<glm::vec3> normals;
        computeNormal(vertices, indices, normals);
        
        setupBuffers(vertices, normals, indices);
    }
    
    void Mesh::setupBuffers(const std::vector<glm::vec3>& vertices,
                           const std::vector<glm::vec3>& normals,
                           const std::vector<unsigned int>& indices) {
        indexCount = indices.size();
                           
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, (vertices.size() + normals.size()) * sizeof(glm::vec3),
                    nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]);
        glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                       normals.size() * sizeof(glm::vec3), &normals[0]);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                    &indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void*)(vertices.size() * sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
    void Mesh::draw() const {
        if (VAO != 0) {
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
    
    void Mesh::cleanup() {
        if (VAO) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        if (EBO) {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }
        indexCount = 0;
    }
}