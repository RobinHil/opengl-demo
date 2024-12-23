#ifndef GLENGINE_MESH_HPP
#define GLENGINE_MESH_HPP

#include <vector>
#include <string>

namespace GLEngine {
    struct Mesh {
        unsigned int VAO, VBO, EBO;
        size_t indexCount;
    };

    std::vector<std::string> getObjFiles(const std::string& directory);
    Mesh loadMesh(const std::string& objPath);
    void cleanupMesh(Mesh& mesh);
}

#endif // GLENGINE_MESH_HPP