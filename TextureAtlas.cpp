#include "TextureAtlas.hpp"

void TextureAtlas::SetTexture(const TextureManager& texMgr,
                              const std::string& name)
{
    TextureAtlasID = texMgr.Get(name);
}

void TextureAtlas::Add(const std::string& name,
                       int x, int y, int width, int height)
{
    Region atlasRegion;

    atlasRegion.u0 = static_cast<float>(x) / static_cast<float>(TextureAtlasWidth);
    atlasRegion.v0 = static_cast<float>(y) / static_cast<float>(TextureAtlasHeight);
    atlasRegion.u1 = static_cast<float>(x + width) / static_cast<float>(TextureAtlasWidth);
    atlasRegion.v1 = static_cast<float>(y + height) / static_cast<float>(TextureAtlasHeight);

    Regions[name] = atlasRegion;
}
