#include "Shader.hpp"

#include <fstream>
#include <sstream>

Shader::~Shader()
{
    using IdType = decltype(VertexShaderID);

    if (VertexShaderID != static_cast<IdType>(-1))
        glDeleteShader(VertexShaderID);

    if (FragmentShaderID != static_cast<IdType>(-1))
        glDeleteShader(FragmentShaderID);

    if (ShaderID != static_cast<IdType>(-1))
        glDeleteProgram(ShaderID);
}

void Shader::CompileAndLink(const std::string& vs_path, const std::string& fs_path)
{
    const auto vs_source = LoadFromFile(vs_path);
    const auto fs_source = LoadFromFile(fs_path);

    unsigned int vs = Compile(GL_VERTEX_SHADER, vs_source);
    unsigned int fs = Compile(GL_FRAGMENT_SHADER, fs_source);
    unsigned int shader = glCreateProgram();
    glAttachShader(shader, vs);
    glAttachShader(shader, fs);
    glLinkProgram(shader);

    GLint success = GL_FALSE;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        char infoLog[512];
        glGetProgramInfoLog(shader, 512, nullptr, infoLog);
        std::stringstream ss;
        ss << "Shader Linker Error: " << infoLog << "\n";
        throw std::runtime_error(ss.str());
    }

    VertexShaderID = vs;
    FragmentShaderID = fs;
    ShaderID = shader;

    InitUniforms();
}

std::string Shader::LoadFromFile(const std::string& path)
{
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

unsigned int Shader::Compile(unsigned int type, const std::string& source)
{
    unsigned int shader = glCreateShader(type);
    const char* shader_source = source.c_str();
    glShaderSource(shader, 1, &shader_source, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::stringstream ss;
        ss << "Shader Compilation Error: " << infoLog << "\n";
        throw std::runtime_error(ss.str());
    }

    return shader;
}

void Shader::InitUniforms()
{
    GLint amountOfUniforms = 0;

    glGetProgramiv(ShaderID, GL_ACTIVE_UNIFORMS, &amountOfUniforms);
    if (amountOfUniforms <= 0)
        return;

    for (GLuint index = 0; index < static_cast<GLuint>(amountOfUniforms); ++index)
    {
        GLchar name[512];
        GLenum type;
        GLint size;
        glGetActiveUniform(ShaderID, index, sizeof(name), nullptr, &size, &type, name);
        name[sizeof(name) - 1] = '\0';

        GLint location = glGetUniformLocation(ShaderID, name);
        if (location >= 0)
            UniformLocations[name] = location;
    }
}
