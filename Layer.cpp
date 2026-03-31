#include "Layer.hpp"
#include "Transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

void Layer::Update(float delta)
{
    Offset += ScrollSpeed * delta;
}

void Layer::Render(int width, int height,
                   const Quad& quad) const
{
    int texCount = TextureIDs.size();
    float texOffset = fmod(Offset, texCount);
    int texIndex = (int)texOffset;
    float localOffset = texOffset - texIndex;

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width),
                                      0.0f, static_cast<float>(height));

    for (int i = 0; i < 2; ++i) {
        int idx = (texIndex + i) % texCount;
        float x = floor((-localOffset + i) * width);

        const auto model = Transform::Create2D(x, 0.0f, width, height);
        glm::mat4 mvp = projection * model;

        LayerShader.Use();
        LayerShader.SetUniform(Locations.Texture, 0);
        LayerShader.SetUniform(Locations.Offset, 0.0f);
        LayerShader.SetUniform(Locations.MVP, glm::value_ptr(mvp));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureIDs[idx]);
        quad.Draw();
    }
}
