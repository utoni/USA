#pragma once

#include "Quad.hpp"
#include "Shader.hpp"

class Framebuffer
{
public:
    explicit Framebuffer(unsigned int width,
                         unsigned int height,
                         Shader& shader)
        : Width(width), Height(height), FboShader(shader)
    {
        Locations.Offset = shader.GetUniformLocation("offset");
        Locations.Texture = shader.GetUniformLocation("tex");
        Locations.MVP = shader.GetUniformLocation("mvp");
    }
    ~Framebuffer();

    void Init();
    void Use() const;
    [[nodiscard]]
    unsigned int Get() const { return FboTextureID; }
    void RenderToScreen(Quad& quad, int width, int height) const;
private:
    void BeginFrame(Quad& quad) const;
    void SetAspectRatio(int glfwWidth, int glfwHeight) const;
    void EndFrame() const;

    unsigned int Width;
    unsigned int Height;
    Shader& FboShader;
    struct {
        int Offset;
        int Texture;
        int MVP;
    } Locations;
    unsigned int FboID = -1;
    unsigned int FboTextureID = -1;
};
