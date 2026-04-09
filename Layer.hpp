#pragma once

#include "Quad.hpp"
#include "Shader.hpp"

#include <vector>

class Layer
{
public:
    explicit Layer(const std::vector<unsigned int>& textureIDs,
                   const Shader& shader,
                   float scrollSpeed);

    void Update(float delta);
    void Render(int width, int height,
                const Quad& quad) const;

private:
    const std::vector<unsigned int> TextureIDs;
    const Shader& LayerShader;
    struct {
        int UvOffset = -1;
        int UvScale = -1;
        int MVP = -1;
    } Locations;
    float ScrollSpeed;
    float Offset = 0.0f;
};
