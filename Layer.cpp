#include "Layer.hpp"
#include "Transform.hpp"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

Layer::Layer(const std::vector<unsigned int>& textureIDs,
             const Shader& shader,
             float scrollSpeed)
    : TextureIDs(textureIDs), LayerShader(shader),
      ScrollSpeed(scrollSpeed)
{
    Locations.MVP = LayerShader.GetUniformLocation("mvp");
}

void Layer::Update(float delta)
{
    PreviousOffset  = Offset;
    PreviousOffsetY = OffsetY;
    Offset   += ScrollSpeed  * delta;
    OffsetY  += ScrollSpeedY * delta;
}

void Layer::Render(int width, int height,
                   const Quad& quad) const
{
    int texCount = static_cast<int>(TextureIDs.size());
    auto texOffset = fmod(static_cast<double>(Offset),
                          static_cast<double>(texCount));
    int texIndex = (int)texOffset;
    auto localOffsetX = texOffset - texIndex;

    auto localOffsetY = fmod(static_cast<double>(OffsetY), 1.0);
    if (localOffsetY < 0.0) localOffsetY += 1.0;

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width),
                                      0.0f, static_cast<float>(height));

    for (int iy = 0; iy < 2; ++iy) {
        for (int ix = 0; ix < 2; ++ix) {
            int idx = (texIndex + ix) % texCount;
            float x = floor((-localOffsetX + ix) * width);
            float y = floor((-localOffsetY + iy) * height);

            const auto model = Transform::Create2D(x, y,
                                                   static_cast<float>(width),
                                                   static_cast<float>(height));
            glm::mat4 mvp = projection * model;

            LayerShader.Use();
            LayerShader.SetUniform(Locations.MVP, glm::value_ptr(mvp));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, TextureIDs[idx]);
            quad.Draw();
        }
    }
}

float Layer::GetScrollOffset() const
{
    const int texCount = static_cast<int>(TextureIDs.size());
    if (texCount <= 0)
        return 0.0f;

    return std::fmod(Offset, static_cast<float>(texCount));
}

float Layer::GetPreviousScrollOffset() const
{
    const int texCount = static_cast<int>(TextureIDs.size());
    if (texCount <= 0)
        return 0.0f;

    return std::fmod(PreviousOffset, static_cast<float>(texCount));
}

float Layer::GetScrollOffsetY() const
{
    return std::fmod(OffsetY, 1.0f);
}

float Layer::GetPreviousScrollOffsetY() const
{
    return std::fmod(PreviousOffsetY, 1.0f);
}

bool Layer::HasTextureID(unsigned int textureID) const
{
    const auto iter = std::find_if(TextureIDs.begin(), TextureIDs.end(), [textureID](unsigned int other) {
        return textureID == other;
    });

    return iter != TextureIDs.cend();
}

unsigned int Layer::GetLayerIndexByTextureID(unsigned int textureID, const std::vector<Layer>& layers)
{
    for (size_t i = 0; i < layers.size(); ++i) {
        if (layers[i].HasTextureID(textureID))
            return i;
    }

    throw std::runtime_error("Layer Index not found with given Texture Name");
}
