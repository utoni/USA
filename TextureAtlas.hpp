#pragma once

#include "TextureManager.hpp"
#include "Optimization.hpp"

#include <stdexcept>
#include <string>
#include <unordered_map>

class TextureAtlas
{
public:
    struct Region {
        float u0 = 0.0f;
        float v0 = 0.0f;
        float u1 = 0.0f;
        float v1 = 0.0f;
    };

public:
    explicit TextureAtlas(int texWidth, int texHeight)
        : TextureAtlasWidth(texWidth), TextureAtlasHeight(texHeight)
    {}

    void SetTexture(const TextureManager& texMgr,
                    const std::string& name);
    void Add(const std::string& name,
             int x, int y, int width, int height);
    [[nodiscard]]
    const Region& Get(const std::string& name) const {
        const auto iter = Regions.find(name);
        if (iter == Regions.cend())
            throw std::runtime_error("Texture does not exist in Texture Atlas!");
        return iter->second;
    }
    [[nodiscard]]
    unsigned int GetAtlasTexture() const { return TextureAtlasID; }


private:
    int TextureAtlasWidth;
    int TextureAtlasHeight;
    unsigned int TextureAtlasID = 0;
    std::unordered_map<const std::string, Region,
                       Optimization::TransparentHash,
                       Optimization::TransparentEqual> Regions;
};
