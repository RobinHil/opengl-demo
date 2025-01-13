#include <glengine/cube.hpp>
#include <glad/glad.h>
#include <vector>

namespace GLEngine {
    Cube::Cube(float size) {
        setupCube(size);
    }

    Cube::~Cube() {
        cleanup();
    }

    void Cube::setupCube(float size) {
        float halfSize = size / 2.0f;
        
        std::vector<float> vertices = {
            // Face avant
            -halfSize, -halfSize,  halfSize,  0.0f,  0.0f,  1.0f,
             halfSize, -halfSize,  halfSize,  0.0f,  0.0f,  1.0f,
             halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,
            -halfSize,  halfSize,  halfSize,  0.0f,  0.0f,  1.0f,
            // Face arrière
            -halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,
             halfSize, -halfSize, -halfSize,  0.0f,  0.0f, -1.0f,
             halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,
            -halfSize,  halfSize, -halfSize,  0.0f,  0.0f, -1.0f,
            // Face droite
             halfSize, -halfSize,  halfSize,  1.0f,  0.0f,  0.0f,
             halfSize, -halfSize, -halfSize,  1.0f,  0.0f,  0.0f,
             halfSize,  halfSize, -halfSize,  1.0f,  0.0f,  0.0f,
             halfSize,  halfSize,  halfSize,  1.0f,  0.0f,  0.0f,
            // Face gauche
            -halfSize, -halfSize,  halfSize, -1.0f,  0.0f,  0.0f,
            -halfSize, -halfSize, -halfSize, -1.0f,  0.0f,  0.0f,
            -halfSize,  halfSize, -halfSize, -1.0f,  0.0f,  0.0f,
            -halfSize,  halfSize,  halfSize, -1.0f,  0.0f,  0.0f,
            // Face supérieure
            -halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,
             halfSize,  halfSize,  halfSize,  0.0f,  1.0f,  0.0f,
             halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f,
            -halfSize,  halfSize, -halfSize,  0.0f,  1.0f,  0.0f,
            // Face inférieure
            -halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,
             halfSize, -halfSize,  halfSize,  0.0f, -1.0f,  0.0f,
             halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f,
            -halfSize, -halfSize, -halfSize,  0.0f, -1.0f,  0.0f
        };

        std::vector<unsigned int> indices = {
            // Face avant
            0, 1, 2,
            2, 3, 0,
            // Face arrière
            4, 5, 6,
            6, 7, 4,
            // Face droite
            8, 9, 10,
            10, 11, 8,
            // Face gauche
            12, 13, 14,
            14, 15, 12,
            // Face supérieure
            16, 17, 18,
            18, 19, 16,
            // Face inférieure
            20, 21, 22,
            22, 23, 20
        };

        unsigned int VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Cube::draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Cube::cleanup() {
        if (VAO) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
    }
}