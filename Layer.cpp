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
    PreviousOffset = Offset;
    Offset += ScrollSpeed * delta;
}

void Layer::Render(int width, int height,
                   const Quad& quad) const
{
    int texCount = static_cast<int>(TextureIDs.size());
    auto texOffset = fmod(static_cast<double>(Offset),
                          static_cast<double>(texCount));
    if (texOffset < 0.0) texOffset += static_cast<double>(texCount);
    int texIndex = (int)texOffset;
    auto localOffset = texOffset - texIndex;

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width),
                                      0.0f, static_cast<float>(height));

    for (int i = 0; i < 2; ++i) {
        int idx = (texIndex + i) % texCount;
        float x = floor((-localOffset + i) * width);

        const auto model = Transform::Create2D(x, 0.0f,
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

float Layer::GetScrollOffset() const
{
    const int texCount = static_cast<int>(TextureIDs.size());
    if (texCount <= 0)
        return 0.0f;

    float offset = std::fmod(Offset, static_cast<float>(texCount));
    if (offset < 0.0f) offset += static_cast<float>(texCount);
    return offset;
}

float Layer::GetPreviousScrollOffset() const
{
    const int texCount = static_cast<int>(TextureIDs.size());
    if (texCount <= 0)
        return 0.0f;

    float offset = std::fmod(PreviousOffset, static_cast<float>(texCount));
    if (offset < 0.0f) offset += static_cast<float>(texCount);
    return offset;
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
