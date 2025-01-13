#ifndef GLENGINE_MESH_HPP
#define GLENGINE_MESH_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace GLEngine {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    class Mesh {
    public:
        Mesh();
        ~Mesh();
        
        void loadFromFile(const std::string& objPath);
        void draw() const;
        void cleanup();
        
        static std::vector<std::string> getObjFiles(const std::string& directory);
        
    private:
        unsigned int VAO, VBO, EBO;
        size_t indexCount;
        bool hasTexCoords;
        
        void setupBuffers(const std::vector<Vertex>& vertices,
                         const std::vector<unsigned int>& indices);
    };
}

#endif // GLENGINE_MESH_HPP