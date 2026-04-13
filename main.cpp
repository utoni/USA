// g++ -Wall -Os -DSTB_IMAGE_IMPLEMENTATION=1 *.c *.cpp -o usa -lglfw

#include "glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

#include "Framebuffer.hpp"
#include "Layer.hpp"
#include "Quad.hpp"
#include "ShaderManager.hpp"
#include "SpriteBatch.hpp"
#include "TextureAtlas.hpp"
#include "TextureManager.hpp"

const unsigned int WIDTH = 1920;
const unsigned int HEIGHT = 1080;

const unsigned int INTERNAL_WIDTH = 480;
const unsigned int INTERNAL_HEIGHT = 270;

void GLAPIENTRY GLDebugMessage(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* message, const void* userParam)
{
    std::cerr << "OpenGL Debug: " << message << "\n";
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Pixel Art Engine", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoaderLoadGL();

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLDebugMessage, nullptr);

    glViewport(0, 0, WIDTH, HEIGHT);

    {
        TextureManager texMgr;
        texMgr.LoadTextures("./assets");

        ShaderManager shaderMgr;
        shaderMgr.LoadShaders("./shaders");

        TextureAtlas sprites(192, 64);
        sprites.SetTexture(texMgr, "sprites");
        sprites.Add("moon", 0, 0, 64, 64);
        sprites.Add("star_00", 64, 0, 64, 64);
        sprites.Add("star_01", 128, 0, 64, 64);

        const auto& spriteShader = shaderMgr.Get("sprite_batch");
        const auto& layerShader = shaderMgr.Get("layer");
        const auto& fbShader = shaderMgr.Get("framebuffer");

        SpriteBatch batch(sprites, spriteShader);
        batch.Init();

        Framebuffer fb(INTERNAL_WIDTH, INTERNAL_HEIGHT, fbShader);
        fb.Init();
        fb.SetMoonSourcePosition(0.53f, 0.85f); // Sprite center in normalized screen-space.
        fb.SetMoonDirection(-0.25f, -1.0f);     // Off-screen moonlight direction.

        Quad quad;
        quad.Create();

        std::vector<Layer> layers = {
            Layer{ { texMgr.Get("background") }, layerShader, 0.05f },
            Layer{ { texMgr.Get("midground") }, layerShader, 0.10f },
            Layer{ { texMgr.Get("foreground3") }, layerShader, 0.15f },
            Layer{ { texMgr.Get("foreground2") }, layerShader, 0.20f },
            Layer{ { texMgr.Get("foreground") }, layerShader, 0.35f },
        };

        auto lastTime = glfwGetTime();
        bool toggleGodraysWasDown = false;
        bool toggleModeWasDown = false;
        bool toggleDebugWasDown = false;

        while (!glfwWindowShouldClose(window)) {
            auto currentTime = glfwGetTime();
            auto delta = currentTime - lastTime;
            lastTime = currentTime;

            fb.Use();

            batch.Begin();
            batch.Draw("moon", 0.5f, 0.8f, 0.06f, 0.1f);
            batch.Draw("star_00", 0.85f, 0.6f, 0.03f, 0.05f);
            batch.Draw("star_00", 0.6f, 0.75f, 0.03f, 0.05f);
            batch.Draw("star_00", 0.4f, 0.5f, 0.03f, 0.05f);
            batch.Draw("star_00", 0.3f, 0.6f, 0.03f, 0.05f);
            batch.Draw("star_00", 0.1f, 0.8f, 0.03f, 0.05f);
            batch.Draw("star_01", 0.8f, 0.7f, 0.03f, 0.05f);
            batch.Draw("star_01", 0.7f, 0.6f, 0.03f, 0.05f);
            batch.Draw("star_01", 0.35f, 0.50f, 0.03f, 0.05f);
            batch.Draw("star_01", 0.25f, 0.65f, 0.03f, 0.05f);
            batch.Draw("star_01", 0.15f, 0.75f, 0.03f, 0.05f);
            batch.Flush(INTERNAL_WIDTH, INTERNAL_HEIGHT);

            for (auto i = 0lu; i < layers.size(); ++i) {
                layers[i].Render(INTERNAL_WIDTH, INTERNAL_HEIGHT, quad);
            }

            for (auto& layer : layers) {
                layer.Update(static_cast<float>(delta));
            }

            const bool toggleGodraysDown = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;
            if (toggleGodraysDown && !toggleGodraysWasDown)
                fb.ToggleGodrays();
            toggleGodraysWasDown = toggleGodraysDown;

            const bool toggleModeDown = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;
            if (toggleModeDown && !toggleModeWasDown)
                fb.ToggleGodraysSourceMode();
            toggleModeWasDown = toggleModeDown;

            const bool toggleDebugDown = glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS;
            if (toggleDebugDown && !toggleDebugWasDown)
                fb.ToggleGodraysMaskDebug();
            toggleDebugWasDown = toggleDebugDown;

            int winW, winH;
            glfwGetFramebufferSize(window, &winW, &winH);
            fb.RenderToScreen(quad, winW, winH, static_cast<float>(currentTime));

            glfwSwapBuffers(window);
            glfwPollEvents();
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_RELEASE)
                break;
        }
    }

    glfwTerminate();
    return 0;
}
