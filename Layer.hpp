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
    [[nodiscard]]
    float GetScrollSpeed() const { return ScrollSpeed; }
    void SetScrollSpeed(float newSpeed) { ScrollSpeed = newSpeed; }
    [[nodiscard]]
    float GetScrollSpeedY() const { return ScrollSpeedY; }
    void SetScrollSpeedY(float newSpeed) { ScrollSpeedY = newSpeed; }
    [[nodiscard]]
    float GetScrollOffset() const;
    [[nodiscard]]
    float GetPreviousScrollOffset() const;
    [[nodiscard]]
    float GetScrollOffsetY() const;
    [[nodiscard]]
    float GetPreviousScrollOffsetY() const;
    bool HasTextureID(unsigned int textureID) const;

    static unsigned int GetLayerIndexByTextureID(unsigned int textureID, const std::vector<Layer>& layers);

private:
    const std::vector<unsigned int> TextureIDs;
    const Shader& LayerShader;
    struct {
        int MVP = -1;
    } Locations;
    float ScrollSpeed;
    float ScrollSpeedY = 0.0f;
    float Offset = 0.0f;
    float PreviousOffset = 0.0f;
    float OffsetY = 0.0f;
    float PreviousOffsetY = 0.0f;
};
