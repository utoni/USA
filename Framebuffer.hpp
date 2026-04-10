#pragma once

#include "Quad.hpp"
#include "Shader.hpp"

class Framebuffer
{
public:
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
    void RenderToScreen(Quad& quad, int width, int height) const;
private:
    void BeginFrame(Quad& quad) const;
    void SetAspectRatio(int glfwWidth, int glfwHeight) const;
    void EndFrame() const;

    int Width;
    int Height;
    const Shader& FboShader;
    struct {
        int MVP = -1;
    } Locations;
    unsigned int FboID = 0;
    unsigned int FboTextureID = 0;
};
