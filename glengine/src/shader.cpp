#include <glengine/shader.hpp>
#include <glengine/utils.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace GLEngine {
    Shader::Shader(const char* vertexPath, const char* fragmentPath) {
        std::string vertexCode = readFile(vertexPath);
        std::string fragmentCode = readFile(fragmentPath);

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertex, fragment;
        
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        
        id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);
        glLinkProgram(id);
        checkCompileErrors(id, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath) {
        std::string vertexCode = readFile(vertexPath);
        std::string geometryCode = readFile(geometryPath);
        std::string fragmentCode = readFile(fragmentPath);

        const char* vShaderCode = vertexCode.c_str();
        const char* gShaderCode = geometryCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        unsigned int vertex, geometry, fragment;
        
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
        
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        
        id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, geometry);
        glAttachShader(id, fragment);
        glLinkProgram(id);
        checkCompileErrors(id, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(geometry);
        glDeleteShader(fragment);
    }

    Shader::~Shader() {
        glDeleteProgram(id);
    }

    void Shader::use() const {
        glUseProgram(id);
    }

    void Shader::setBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
    }
    
    void Shader::setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }
    
    void Shader::setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }

    void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }

    void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void Shader::checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
    }
}