#pragma once

#include "glad.h"

#include "Optimization.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_map>

class Shader
{
public:
    explicit Shader() {}
    Shader(const Shader &) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(const Shader &) = delete;
    Shader& operator=(Shader &&) = delete;
    ~Shader();

    void CompileAndLink(const std::filesystem::path& vertexPath,
                        const std::filesystem::path& fragmentPath);
    void SetUniform(int location, GLint valuei) const {
        return glUniform1i(location, valuei);
    }
    void SetUniform(int location, GLfloat valuef) const {
        return glUniform1f(location, valuef);
    }
    void SetUniform(int location, GLfloat value0f, GLfloat value1f) const {
        return glUniform2f(location, value0f, value1f);
    }
    void SetUniform(int location, const GLfloat* value4fv) const {
        return glUniformMatrix4fv(location, 1, GL_FALSE, value4fv);
    }
    [[nodiscard]]
    int GetUniformLocation(const std::string& name) const {
        const auto iter = UniformLocations.find(name);
        if (iter == UniformLocations.cend())
            throw std::runtime_error("Shader Uniform does not exist!");
        return iter->second;
    }
    void Use() const { return glUseProgram(ShaderID); }

    static Shader Create(const std::filesystem::path& vertexShader,
                         const std::filesystem::path& fragmentShader);

private:
    void InitUniforms();

    static std::string LoadFromFile(const std::string& path);
    static unsigned int Compile(unsigned int type, const std::string& source);

    unsigned int VertexShaderID = 0;
    unsigned int FragmentShaderID = 0;
    unsigned int ShaderID = 0;
    std::unordered_map<const std::string, int,
                       Optimization::TransparentHash,
                       Optimization::TransparentEqual> UniformLocations;
};
