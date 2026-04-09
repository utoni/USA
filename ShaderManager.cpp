#include "ShaderManager.hpp"

void ShaderManager::LoadShaders(const std::filesystem::path& shaderRoot)
{
    std::unordered_map<std::filesystem::path, bool,
                       Optimization::TransparentHash,
                       Optimization::TransparentEqual> shaders;

    for (const auto& entry : std::filesystem::directory_iterator(shaderRoot))
    {
        const auto& path = entry.path();
        if (path.extension() != ".glsl")
            continue;

        const auto& filename = path.filename().string();
        auto pathIndex = filename.rfind(".vertex");
        if (pathIndex == filename.npos)
            pathIndex = filename.rfind(".fragment");
        if (pathIndex == filename.npos)
            throw std::runtime_error("Unexpected non-shader file found");

        const auto& shaderName = filename.substr(0, pathIndex);
        auto entryName = path.parent_path().string();
        entryName.append("/");
        entryName.append(shaderName);
        const auto& shaderIter = shaders.find(entryName);
        if (shaderIter == shaders.cend())
            shaders[entryName] = false;
        else
            shaders[entryName] = true;
    }

    for (const auto& shader : shaders) {
        if (!shader.second)
            throw std::runtime_error("Missing a Vertex OR Fragment shader file");

        auto vertexShader = shader.first;
        vertexShader += ".vertex.glsl";
        auto fragmentShader = shader.first;
        fragmentShader += ".fragment.glsl";
        Shaders.emplace(shader.first.stem(), Shader::Create(vertexShader, fragmentShader));
    }
}
