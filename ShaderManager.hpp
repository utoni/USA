#pragma once

#include "Optimization.hpp"
#include "Shader.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_map>

class ShaderManager
{
public:
    explicit ShaderManager()
    {}

    void LoadShaders(const std::filesystem::path& shaderRoot);
    [[nodiscard]]
    const Shader& Get(const std::string& name) const {
        const auto iter = Shaders.find(name);
        if (iter == Shaders.cend())
            throw std::runtime_error("Shader does not exist!");
        return iter->second;
    }

private:
    std::unordered_map<const std::string, const Shader,
                       Optimization::TransparentHash,
                       Optimization::TransparentEqual> Shaders;
};
