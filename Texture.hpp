#pragma once

#include <filesystem>

class Texture
{
public:
    explicit Texture() {}
    ~Texture() {}
    Texture(Texture&& other) noexcept {
        TextureID = other.TextureID;
        other.TextureID = 0;
    }
    Texture& operator=(Texture&& other) noexcept  = delete;
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    void Open(const std::filesystem::path& path);
    [[nodiscard]]
    unsigned int Get() const { return TextureID; }

    [[nodiscard]]
    static Texture Create(const std::filesystem::path& path);

private:
    unsigned int TextureID = 0;
};
