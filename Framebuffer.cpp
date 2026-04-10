#include "Framebuffer.hpp"

#include "glad.h"
#include "Transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

Framebuffer::~Framebuffer()
{
    if (FboTextureID != 0)
        glDeleteTextures(1, &FboTextureID);

    if (FboID != 0)
        glDeleteFramebuffers(1, &FboID);
}

void Framebuffer::Init()
{
    Locations.MVP = FboShader.GetUniformLocation("mvp");

    glGenFramebuffers(1, &FboID);
    glBindFramebuffer(GL_FRAMEBUFFER, FboID);

    glGenTextures(1, &FboTextureID);
    glBindTexture(GL_TEXTURE_2D, FboTextureID);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, Width, Height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        FboTextureID,
        0
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("FBO Error (not complete)");
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Use() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, FboID);
    glViewport(0, 0, Width, Height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Framebuffer::BeginFrame(Quad& quad) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    quad.Draw();
}

void Framebuffer::SetAspectRatio(int glfwWidth, int glfwHeight) const
{
    float windowAspect = static_cast<float>(glfwWidth) / static_cast<float>(glfwHeight);
    float targetAspect = static_cast<float>(Width) / static_cast<float>(Height);

    int viewportX = 0;
    int viewportY = 0;
    int viewportW = glfwWidth;
    int viewportH = glfwHeight;

    if (windowAspect > targetAspect)
    {
        // Pillarbox
        viewportH = glfwHeight;
        viewportW = static_cast<int>(static_cast<float>(viewportH) * targetAspect);
        viewportX = (glfwWidth - viewportW) / 2;
    } else {
        // Letterbox
        viewportW = glfwWidth;
        viewportH = static_cast<int>(static_cast<float>(viewportW) / targetAspect);
        viewportY = (glfwHeight - viewportH) / 2;
    }

    glViewport(viewportX, viewportY, viewportW, viewportH);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Framebuffer::EndFrame() const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Get());
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Framebuffer::RenderToScreen(Quad& quad, int width, int height) const
{
    BeginFrame(quad);

    SetAspectRatio(width, height);

    glm::mat4 model = Transform::Create2D(0.0f, 0.0f,
                                          static_cast<float>(width),
                                          static_cast<float>(height));

    glm::mat4 projection =
        glm::ortho(0.0f, (float)width,
                   0.0f, (float)height);

    glm::mat4 mvp = projection * model;

    FboShader.Use();
    FboShader.SetUniform(Locations.MVP, glm::value_ptr(mvp));

    EndFrame();
}
