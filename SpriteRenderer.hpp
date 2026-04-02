#pragma once

#include "Quad.hpp"
#include "Shader.hpp"
#include "TextureAtlas.hpp"

#include <glm/glm.hpp>

class SpriteRenderer
{
public:
    SpriteRenderer(const Shader& shader)
        : SpriteShader(shader),
          Locations(shader.GetDefaultLocations())
    {}
    ~SpriteRenderer() {}

    void Draw(const Quad& quad,
              const TextureAtlas& atlas,
              const TextureAtlas::Region& region,
              float x, float y, float width, float height,
              const glm::mat4& projection);

private:
    const Shader& SpriteShader;
    Shader::DefaultLocations Locations;
};
