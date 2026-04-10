#pragma once

#include "Shader.hpp"
#include "TextureAtlas.hpp"

#include <string>
#include <vector>

class SpriteBatch
{
public:
    struct SpriteVertex
    {
        float x;
        float y;
        float u;
        float v;
    };

    explicit SpriteBatch(const TextureAtlas& atlas,
                         const Shader& shader,
                         size_t maxSprites = 10000)
        : Atlas(atlas),
          SpriteShader(shader), MaxSprites(maxSprites)
    {
        Vertices.reserve(maxSprites * 6);
    }
    SpriteBatch(const SpriteBatch&) = delete;
    SpriteBatch(SpriteBatch&&) = delete;
    SpriteBatch& operator=(const SpriteBatch&) = delete;
    SpriteBatch& operator=(SpriteBatch&&) = delete;
    ~SpriteBatch();

    void Init();
    void Begin()
    {
        Vertices.clear();
    }
    void Draw(const std::string& name,
              float x, float y,
              float w, float h);
    void Flush(int width, int height);

private:
    void AddVertex(float x, float y, float u, float v)
    {
        Vertices.push_back({x,y,u,v});
    }

    const TextureAtlas& Atlas;
    const Shader& SpriteShader;
    struct {
        int MVP = -1;
    } Locations;
    size_t MaxSprites;
    std::vector<SpriteVertex> Vertices;
    unsigned int VAO = 0;
    unsigned int VBO = 0;
};
