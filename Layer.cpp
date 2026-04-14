#include "Layer.hpp"
#include "Transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

Layer::Layer(const std::vector<unsigned int>& textureIDs,
             const Shader& shader,
             float scrollSpeed)
    : TextureIDs(textureIDs), LayerShader(shader),
      ScrollSpeed(scrollSpeed)
{
    Locations.UvOffset = LayerShader.GetUniformLocation("uvOffset");
    Locations.UvScale = LayerShader.GetUniformLocation("uvScale");
    Locations.MVP = LayerShader.GetUniformLocation("mvp");
}

void Layer::Update(float delta)
{
    Offset += ScrollSpeed * delta;
}

void Layer::Render(int width, int height,
                   const Quad& quad) const
{
    int texCount = static_cast<int>(TextureIDs.size());
    auto texOffset = fmod(static_cast<double>(Offset),
                          static_cast<double>(texCount));
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
        LayerShader.SetUniform(Locations.UvOffset, 0.0f, 0.0f);
        LayerShader.SetUniform(Locations.UvScale, 1.0f, 1.0f);
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

    return static_cast<float>(fmod(static_cast<double>(Offset),
                                   static_cast<double>(texCount)));
}
