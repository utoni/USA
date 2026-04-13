#include "Framebuffer.hpp"

#include "glad.h"
#include "Transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>
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
    Locations.GodraysEnabled = FboShader.GetUniformLocation("enableGodrays");
    Locations.ShowGodraysMaskDebug = FboShader.GetUniformLocation("showGodraysMask");
    Locations.GodraysSourceMode = FboShader.GetUniformLocation("godraysSourceMode");
    Locations.MoonSourcePosition = FboShader.GetUniformLocation("moonScreenPos");
    Locations.MoonDirection = FboShader.GetUniformLocation("moonDirection");
    Locations.GodraysLightCount = FboShader.GetUniformLocation("godraysLightCount");
    Locations.GodraysLightPositions = FboShader.GetUniformLocation("godraysLightPositions[0]");
    Locations.GodraysIntensity = FboShader.GetUniformLocation("godraysIntensity");
    Locations.GodraysExposure = FboShader.GetUniformLocation("godraysExposure");
    Locations.GodraysDecay = FboShader.GetUniformLocation("godraysDecay");
    Locations.GodraysDensity = FboShader.GetUniformLocation("godraysDensity");
    Locations.GodraysWeight = FboShader.GetUniformLocation("godraysWeight");
    Locations.GodraysSamples = FboShader.GetUniformLocation("godraysSamples");
    Locations.GodraysColor = FboShader.GetUniformLocation("godraysColor");
    Locations.GodraysNoiseAmount = FboShader.GetUniformLocation("godraysNoiseAmount");
    Locations.TimeSeconds = FboShader.GetUniformLocation("timeSeconds");

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

void Framebuffer::RenderToScreen(Quad& quad, int width, int height, float timeSeconds) const
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
    FboShader.SetUniform(Locations.GodraysEnabled, static_cast<int>(GodraysEnabled));
    FboShader.SetUniform(Locations.ShowGodraysMaskDebug, static_cast<int>(ShowGodraysMaskDebug));
    FboShader.SetUniform(Locations.GodraysSourceMode, static_cast<int>(SourceMode));
    FboShader.SetUniform(Locations.MoonSourcePosition,
                         MoonSourcePosition.x, MoonSourcePosition.y);
    FboShader.SetUniform(Locations.MoonDirection,
                         MoonDirection.x, MoonDirection.y);
    FboShader.SetUniform(Locations.GodraysLightCount, GodraysLightCount);
    if (GodraysLightCount > 0) {
        glUniform2fv(Locations.GodraysLightPositions, GodraysLightCount,
                     reinterpret_cast<const float*>(GodraysLightSources.data()));
    }
    FboShader.SetUniform(Locations.GodraysIntensity, Godrays.Intensity);
    FboShader.SetUniform(Locations.GodraysExposure, Godrays.Exposure);
    FboShader.SetUniform(Locations.GodraysDecay, Godrays.Decay);
    FboShader.SetUniform(Locations.GodraysDensity, Godrays.Density);
    FboShader.SetUniform(Locations.GodraysWeight, Godrays.Weight);
    FboShader.SetUniform(Locations.GodraysSamples, Godrays.Samples);
    FboShader.SetUniform(Locations.GodraysColor,
                         Godrays.Color.x, Godrays.Color.y, Godrays.Color.z);
    FboShader.SetUniform(Locations.GodraysNoiseAmount, Godrays.NoiseAmount);
    FboShader.SetUniform(Locations.TimeSeconds, timeSeconds);

    EndFrame();
}

void Framebuffer::ToggleGodrays()
{
    GodraysEnabled = !GodraysEnabled;
}

void Framebuffer::ToggleGodraysMaskDebug()
{
    ShowGodraysMaskDebug = !ShowGodraysMaskDebug;
}

void Framebuffer::ToggleGodraysSourceMode()
{
    if (SourceMode == GodraysSourceMode::MoonSprite)
        SourceMode = GodraysSourceMode::Directional;
    else
        SourceMode = GodraysSourceMode::MoonSprite;
}

void Framebuffer::SetMoonSourcePosition(float normalizedX, float normalizedY)
{
    MoonSourcePosition.x = normalizedX;
    MoonSourcePosition.y = normalizedY;
}

void Framebuffer::SetMoonDirection(float normalizedX, float normalizedY)
{
    constexpr float kDirectionEpsilon = 0.0001f;
    const auto length = std::sqrt(normalizedX * normalizedX + normalizedY * normalizedY);
    if (length <= kDirectionEpsilon)
        return;

    MoonDirection.x = normalizedX / length;
    MoonDirection.y = normalizedY / length;
}

void Framebuffer::ClearGodraysLightSources()
{
    GodraysLightCount = 0;
}

void Framebuffer::AddGodraysLightSource(float normalizedX, float normalizedY)
{
    if (GodraysLightCount >= MaxGodraysLightSources)
        return;

    GodraysLightSources[GodraysLightCount].x = std::clamp(normalizedX, 0.0f, 1.0f);
    GodraysLightSources[GodraysLightCount].y = std::clamp(normalizedY, 0.0f, 1.0f);
    ++GodraysLightCount;
}
