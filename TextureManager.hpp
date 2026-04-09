#pragma once

#include "Optimization.hpp"
#include "Texture.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_map>

class TextureManager
{
public:
    explicit TextureManager()
    {}

    void LoadTextures(const std::filesystem::path& textureRoot);
    [[nodiscard]]
    unsigned int Get(const std::string& name) const {
        const auto iter = Textures.find(name);
        if (iter == Textures.cend())
            throw std::runtime_error("Texture does not exist!");
        return iter->second.Get();
    }

private:
    std::unordered_map<const std::string, const Texture,
                       Optimization::TransparentHash,
                       Optimization::TransparentEqual> Textures;
};
