#pragma once

#include "Quad.hpp"
#include "Shader.hpp"

#include <vector>

class Layer
{
public:
    explicit Layer(const std::vector<unsigned int>& textureIDs,
                   Shader& shader,
                   float scrollSpeed)
        : TextureIDs(textureIDs), LayerShader(shader), ScrollSpeed(scrollSpeed)
    {
        Locations.Offset = shader.GetUniformLocation("offset");
        Locations.Texture = shader.GetUniformLocation("tex");
        Locations.MVP = shader.GetUniformLocation("mvp");
    }
    ~Layer() {}

    void Update(float delta);
    void Render(int width, int height,
                const Quad& quad) const;

private:
    const std::vector<unsigned int> TextureIDs;
    Shader& LayerShader;
    struct {
        int Offset;
        int Texture;
        int MVP;
    } Locations;
    float ScrollSpeed;
    float Offset = 0.0f;
};
