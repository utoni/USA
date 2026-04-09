#include "Shader.hpp"

#include <array>
#include <fstream>
#include <sstream>

Shader::~Shader()
{
    if (VertexShaderID != 0)
        glDeleteShader(VertexShaderID);

    if (FragmentShaderID != 0)
        glDeleteShader(FragmentShaderID);

    if (ShaderID != 0)
        glDeleteProgram(ShaderID);
}

Shader::Shader(Shader&& other) noexcept
{
    VertexShaderID = other.VertexShaderID;
    FragmentShaderID = other.FragmentShaderID;
    ShaderID = other.ShaderID;
    UniformLocations = std::move(other.UniformLocations);

    other.VertexShaderID = 0;
    other.FragmentShaderID = 0;
    other.ShaderID = 0;
    other.UniformLocations.clear();
}

void Shader::CompileAndLink(const std::filesystem::path& vertexPath,
                            const std::filesystem::path& fragmentPath)
{
    const auto vsSource = LoadFromFile(vertexPath);
    const auto fsSource = LoadFromFile(fragmentPath);

    unsigned int vs = Compile(GL_VERTEX_SHADER, vsSource);
    unsigned int fs = Compile(GL_FRAGMENT_SHADER, fsSource);
    unsigned int shader = glCreateProgram();
    glAttachShader(shader, vs);
    glAttachShader(shader, fs);
    glLinkProgram(shader);

    GLint success = GL_FALSE;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        std::array<char, 512> infoLog = {};
        glGetProgramInfoLog(shader, std::size(infoLog), nullptr, std::data(infoLog));
        std::stringstream ss;
        ss << "Shader Linker Error: " << std::data(infoLog);
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

    if (!file.is_open() || file.bad())
        throw std::runtime_error("Loading Shader Error: Could not open/read file(s)");
    ss << file.rdbuf();
    return ss.str();
}

unsigned int Shader::Compile(unsigned int type, const std::string& source)
{
    unsigned int shader = glCreateShader(type);
    const char* shaderSource = source.c_str();
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        std::array<char, 512> infoLog = {};
        glGetShaderInfoLog(shader, std::size(infoLog), nullptr, std::data(infoLog));
        std::stringstream ss;
        ss << "Shader Compilation Error: " << std::data(infoLog);
        throw std::runtime_error(ss.str());
    }

    return shader;
}

Shader Shader::Create(const std::filesystem::path& vertexShader,
                      const std::filesystem::path& fragmentShader)
{
    Shader shader;
    shader.CompileAndLink(vertexShader, fragmentShader);
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
        std::array<GLchar, 512> name = {};
        GLenum type;
        GLint size;
        glGetActiveUniform(ShaderID, index, std::size(name), nullptr, &size, &type, std::data(name));
        name[std::size(name) - 1] = '\0';

        GLint location = glGetUniformLocation(ShaderID, std::data(name));
        if (location >= 0)
            UniformLocations[std::data(name)] = location;
    }
}
