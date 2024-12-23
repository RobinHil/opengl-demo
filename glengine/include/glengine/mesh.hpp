#ifndef GLENGINE_MESH_HPP
#define GLENGINE_MESH_HPP

#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace GLEngine {
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
        
        void setupBuffers(const std::vector<glm::vec3>& vertices, 
                         const std::vector<glm::vec3>& normals,
                         const std::vector<unsigned int>& indices);
    };
}

#endif // GLENGINE_MESH_HPP