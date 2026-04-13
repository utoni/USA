#pragma once

#include "Quad.hpp"
#include "Shader.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Framebuffer
{
public:
    enum class GodraysSourceMode {
        MoonSprite = 0,
        Directional = 1
    };

    explicit Framebuffer(int width,
                         int height,
                         const Shader& shader)
        : Width(width), Height(height), FboShader(shader)
    {}
    Framebuffer(const Framebuffer &) = delete;
    Framebuffer(Framebuffer &&) = delete;
    Framebuffer& operator=(const Framebuffer &) = delete;
    Framebuffer& operator=(Framebuffer&&) = delete;
    ~Framebuffer();

    void Init();
    void Use() const;
    [[nodiscard]]
    unsigned int Get() const { return FboTextureID; }
    void RenderToScreen(Quad& quad, int width, int height, float timeSeconds) const;
    void ToggleGodrays();
    void ToggleGodraysMaskDebug();
    void ToggleGodraysSourceMode();
    void SetMoonSourcePosition(float normalizedX, float normalizedY);
    void SetMoonDirection(float normalizedX, float normalizedY);
    [[nodiscard]]
    bool IsGodraysEnabled() const { return GodraysEnabled; }
    [[nodiscard]]
    bool IsGodraysMaskDebugEnabled() const { return ShowGodraysMaskDebug; }
    [[nodiscard]]
    GodraysSourceMode GetGodraysSourceMode() const { return SourceMode; }
private:
    void BeginFrame(Quad& quad) const;
    void SetAspectRatio(int glfwWidth, int glfwHeight) const;
    void EndFrame() const;

    int Width;
    int Height;
    const Shader& FboShader;
    struct {
        int MVP = -1;
        int GodraysEnabled = -1;
        int ShowGodraysMaskDebug = -1;
        int GodraysSourceMode = -1;
        int MoonSourcePosition = -1;
        int MoonDirection = -1;
        int GodraysIntensity = -1;
        int GodraysExposure = -1;
        int GodraysDecay = -1;
        int GodraysDensity = -1;
        int GodraysWeight = -1;
        int GodraysSamples = -1;
        int GodraysColor = -1;
        int GodraysNoiseAmount = -1;
        int TimeSeconds = -1;
    } Locations;
    struct GodraysParameters {
        // Pixel-art friendly defaults (low sample count + subtle cool tint + tiny dithering).
        float Intensity = 0.55f;
        float Exposure = 0.24f;
        float Decay = 0.96f;
        float Density = 0.82f;
        float Weight = 0.7f;
        int Samples = 36;
        glm::vec3 Color = {0.62f, 0.72f, 0.95f};
        float NoiseAmount = 0.035f;
    } Godrays;
    bool GodraysEnabled = true;
    bool ShowGodraysMaskDebug = false;
    GodraysSourceMode SourceMode = GodraysSourceMode::MoonSprite;
    glm::vec2 MoonSourcePosition = {0.53f, 0.85f};
    glm::vec2 MoonDirection = {-0.35f, -1.0f};
    unsigned int FboID = 0;
    unsigned int FboTextureID = 0;
};
