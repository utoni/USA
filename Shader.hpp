#pragma once

#include "glad.h"

#include "Optimization.hpp"

#include <string>
#include <unordered_map>

class Shader
{
public:
    explicit Shader() {}
    ~Shader();

    void CompileAndLink(const std::string& vs_path, const std::string& fs_path);
    void SetUniform(int location, GLint valuei) const {
        return glUniform1i(location, valuei);
    }
    void SetUniform(int location, GLfloat valuef) const {
        return glUniform1f(location, valuef);
    }
    void SetUniform(int location, const GLfloat* value4fv) const {
        return glUniformMatrix4fv(location, 1, GL_FALSE, value4fv);
    }
    int GetUniformLocation(const std::string& name) {
        return UniformLocations[name];
    }
    void Use() const { return glUseProgram(ShaderID); }

private:
    void InitUniforms();

    static std::string LoadFromFile(const std::string& path);
    static unsigned int Compile(unsigned int type, const std::string& source);

    unsigned int VertexShaderID = -1;
    unsigned int FragmentShaderID = -1;
    unsigned int ShaderID = -1;
    std::unordered_map<const std::string, int,
                       Optimization::TransparentHash,
                       Optimization::TransparentEqual> UniformLocations;
};
