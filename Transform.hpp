#pragma once

#include "glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Transform
{
    static glm::mat4 Create2D(float x, float y,
                              float w, float h)
    {
        glm::mat4 model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(x, y, 0)
        );

        model = glm::scale(
            model,
            glm::vec3(w, h, 1)
        );

        return model;
    }
};
