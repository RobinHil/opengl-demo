#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

namespace GLEngine {
    class Shader {
    public:
        Shader(const char* vertexPath, const char* fragmentPath);
        Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);
        ~Shader();

        void use() const;
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setMat4(const std::string &name, const glm::mat4 &mat) const;

        unsigned int getId() const { return id; }

    private:
        unsigned int id;
        void checkCompileErrors(unsigned int shader, std::string type);
    };
}

#endif