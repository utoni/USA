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
    float GetScrollSpeed() const { return ScrollSpeed; }
    void SetScrollSpeed(float newSpeed) { ScrollSpeed = newSpeed; }

private:
    const std::vector<unsigned int> TextureIDs;
    const Shader& LayerShader;
    struct {
        int MVP = -1;
    } Locations;
    float ScrollSpeed;
    float Offset = 0.0f;
};
