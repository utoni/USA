#include "TextureManager.hpp"

void TextureManager::LoadTextures(const std::filesystem::path& textureRoot)
{
    for (const auto& entry : std::filesystem::directory_iterator(textureRoot))
    {
        const auto path = entry.path();
        if (path.extension() == ".png") {
            Textures.emplace(path.stem(), Texture::Create(path));
        }
    }
}
