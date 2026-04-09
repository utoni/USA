#include "SpriteBatch.hpp"

#include "glad.h"
#include "Transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

void SpriteBatch::Init()
{
    Locations.MVP = SpriteShader.GetUniformLocation("mvp");

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<long>(MaxSprites * 6 * sizeof(SpriteVertex)),
                 nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex),
                          reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SpriteBatch::Draw(const std::string& name,
                       float x, float y,
                       float w, float h)
{
    float x2 = x + w;
    float y2 = y + h;

    const auto& region = Atlas.Get(name);
    float u0 = region.u0;
    float v0 = region.v0;
    float u1 = region.u1;
    float v1 = region.v1;

    AddVertex(x, y2, u0, v1);
    AddVertex(x, y, u0, v0);
    AddVertex(x2, y, u1, v0);

    AddVertex(x, y2, u0, v1);
    AddVertex(x2, y, u1, v0);
    AddVertex(x2, y2, u1, v1);
}

void SpriteBatch::Flush(int width, int height)
{
    if (Vertices.empty())
        return;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    static_cast<long>(Vertices.size() * sizeof(SpriteVertex)),
                    Vertices.data());

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width),
                                      0.0f, static_cast<float>(height));
    const auto model = Transform::Create2D(0.0f, 0.0f,
                                                            static_cast<float>(width),
                                                            static_cast<float>(height));
    glm::mat4 mvp = projection * model;
    SpriteShader.Use();
    SpriteShader.SetUniform(Locations.MVP, glm::value_ptr(mvp));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Atlas.GetAtlasTexture());
    glDrawArrays(GL_TRIANGLES,0,static_cast<int>(Vertices.size()));
}
