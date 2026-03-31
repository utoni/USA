#include "Quad.hpp"

#include "glad.h"

Quad::~Quad()
{
    using IdType = decltype(VBO);

    if (VBO != static_cast<IdType>(-1))
        glDeleteBuffers(1, &VBO);

    if (VAO != static_cast<IdType>(-1))
        glDeleteVertexArrays(1, &VAO);
}

void Quad::Create()
{
    float quadVertices[] = {
        // positions            // texCoords
        0.0f, 1.0f,     0.0f, 1.0f,
        0.0f, 0.0f,     0.0f, 0.0f,
        1.0f, 0.0f,     1.0f, 0.0f,

        0.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 0.0f,   1.0f, 0.0f,
        1.0f, 1.0f,   1.0f, 1.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Quad::Draw() const
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
