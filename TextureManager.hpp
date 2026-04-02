#pragma once

#include "Texture.hpp"
#include "Optimization.hpp"

#include <filesystem>
#include <unordered_map>
#include <stdexcept>
#include <string>

class TextureManager
{
public:
    explicit TextureManager()
    {}
    ~TextureManager() {}

    void LoadTextures(const std::filesystem::path& textureRoot);
    [[nodiscard]]
    unsigned int Get(const std::string& name) const {
        const auto iter = Textures.find(name);
        if (iter == Textures.cend())
            throw std::runtime_error("Texture does not exist!");
        return iter->second.Get();
    }

private:
    std::unordered_map<const std::string, Texture,
                       Optimization::TransparentHash,
                       Optimization::TransparentEqual> Textures;
};
