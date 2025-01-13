#include <glengine/grid3D.hpp>
#include <glad/glad.h>

namespace GLEngine {
    Grid3D::Grid3D(float size, float spacing) {
        setupGrid(size, spacing);
        setupAxes(size);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    Grid3D::~Grid3D() {
        cleanup();
    }

    void Grid3D::setupGrid(float size, float spacing) {
        float gridColor = 0.1f;
        
        for (float i = -size; i <= size; i += spacing) {
            vertices.insert(vertices.end(), {
                -size, 0.0f, i, gridColor, gridColor, gridColor,
                size, 0.0f, i, gridColor, gridColor, gridColor
            });

            vertices.insert(vertices.end(), {
                i, 0.0f, -size, gridColor, gridColor, gridColor,
                i, 0.0f, size, gridColor, gridColor, gridColor
            });
        }
        
        gridVertexCount = vertices.size() / 6;
    }

    void Grid3D::setupAxes(float size) {
        // X-axis (Red)
        vertices.insert(vertices.end(), {
            0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            size, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f
        });

        // Y-axis (Green)
        vertices.insert(vertices.end(), {
            0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, size, 0.0f, 0.0f, 1.0f, 0.0f
        });

        // Z-axis (Blue)
        vertices.insert(vertices.end(), {
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, size, 0.0f, 0.0f, 1.0f
        });

        axesVertexCount = (vertices.size() / 6) - gridVertexCount;
    }

    void Grid3D::draw(const glm::mat4& view, const glm::mat4& projection) {
        glBindVertexArray(VAO);
        
        glDrawArrays(GL_LINES, 0, gridVertexCount);
        
        glLineWidth(3.0f);
        glDrawArrays(GL_LINES, gridVertexCount, axesVertexCount);
        glLineWidth(1.0f);
        
        glBindVertexArray(0);
    }

    void Grid3D::cleanup() {
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