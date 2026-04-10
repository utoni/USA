#include "ShaderManager.hpp"

void ShaderManager::LoadShaders(const std::filesystem::path& shaderRoot)
{
    struct ShaderFiles {
        bool VertexFound = false;
        bool FragmentFound = false;
    };
    std::unordered_map<const std::filesystem::path, ShaderFiles,
                       Optimization::TransparentHash,
                       Optimization::TransparentEqual> shaders;

    for (const auto& entry : std::filesystem::directory_iterator(shaderRoot))
    {
        const auto& path = entry.path();
        if (path.extension() != ".glsl")
            continue;

        ShaderFiles files;
        const auto& filename = path.filename().string();
        auto pathIndex = filename.rfind(".vertex");
        if (pathIndex == filename.npos)
            pathIndex = filename.rfind(".fragment");
        else
            files.VertexFound = true;
        if (pathIndex == filename.npos)
            throw std::runtime_error("Unexpected non-shader file found");
        else
            files.FragmentFound = true;

        const auto& shaderName = filename.substr(0, pathIndex);
        auto entryName = path.parent_path().string();
        entryName.append("/");
        entryName.append(shaderName);
        const auto& shaderIter = shaders.find(entryName);
        if (shaderIter == shaders.cend()) {
            shaders.emplace(entryName, files);
        } else {
            shaderIter->second.VertexFound |= files.VertexFound;
            shaderIter->second.FragmentFound |= files.FragmentFound;
        }
    }

    for (const auto& shader : shaders) {
        if (!shader.second.VertexFound)
            throw std::runtime_error("Missing a mandatory Vertex shader file");
        if (!shader.second.FragmentFound)
            throw std::runtime_error("Missing a mandatory Fragment shader file");

        auto vertexShader = shader.first;
        vertexShader += ".vertex.glsl";
        auto fragmentShader = shader.first;
        fragmentShader += ".fragment.glsl";
        Shaders.emplace(shader.first.stem(), Shader::Create(vertexShader, fragmentShader));
    }
}
