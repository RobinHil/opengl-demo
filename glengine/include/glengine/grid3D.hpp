#ifndef GRID3D_HPP
#define GRID3D_HPP

#include <glengine/shader.hpp>
#include <vector>
#include <glm/glm.hpp>

namespace GLEngine {
    class Grid3D {
    public:
        Grid3D(float size = 10.0f, float spacing = 1.0f);
        ~Grid3D();
        
        void draw(const glm::mat4& view, const glm::mat4& projection);
        void cleanup();
        
    private:
        unsigned int VAO, VBO;
        std::vector<float> vertices;
        size_t gridVertexCount;
        size_t axesVertexCount;
        
        void setupGrid(float size, float spacing);
        void setupAxes(float size);
    };
}

#endif