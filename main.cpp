// g++ -Wall -Os -DSTB_IMAGE_IMPLEMENTATION=1 *.c *.cpp -o usa -lglfw

#include "glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "Framebuffer.hpp"
#include "Layer.hpp"
#include "ParticleEmitter.hpp"
#include "ParticleSystem.hpp"
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
        fb.ClearGodraysLightSources();
        fb.AddGodraysLightSource(0.53f, 0.85f);  // Moon
        fb.AddGodraysLightSource(0.865f, 0.625f);
        fb.AddGodraysLightSource(0.615f, 0.775f);
        fb.AddGodraysLightSource(0.415f, 0.525f);
        fb.AddGodraysLightSource(0.315f, 0.625f);
        fb.AddGodraysLightSource(0.115f, 0.825f);
        fb.AddGodraysLightSource(0.815f, 0.725f);
        fb.AddGodraysLightSource(0.715f, 0.625f);
        fb.AddGodraysLightSource(0.365f, 0.525f);
        fb.AddGodraysLightSource(0.265f, 0.675f);
        fb.AddGodraysLightSource(0.165f, 0.775f);

        Quad quad;
        quad.Create();

        std::vector<Layer> layers = {
            Layer{ { texMgr.Get("background") }, layerShader, 0.05f },
            Layer{ { texMgr.Get("next-midground") }, layerShader, 0.08f },
            Layer{ { texMgr.Get("midground") }, layerShader, 0.10f },
            Layer{ { texMgr.Get("next-next-foreground") }, layerShader, 0.15f },
            Layer{ { texMgr.Get("next-foreground") }, layerShader, 0.20f },
            Layer{ { texMgr.Get("foreground"), texMgr.Get("foreground2") }, layerShader, 0.35f },
        };

        ParticleSystem particles(shaderMgr.Get("particle"));
        ParticleEmitter emitters(particles);
        unsigned int emitter_count = 0;
        {
            const auto layerIndex = Layer::GetLayerIndexByTextureID(texMgr.Get("foreground"), layers);
            auto [emitter_cfg, emitter_anchor] = ParticleEmitter::MakeLeafEmitter(0.20f, 0.70f, emitter_count++,
                layerIndex, layerIndex, 11);
            emitters.AddEmitter(emitter_cfg, emitter_anchor);
        }
        {
            const auto layerIndex = Layer::GetLayerIndexByTextureID(texMgr.Get("foreground"), layers);
            auto [emitter_cfg, emitter_anchor] = ParticleEmitter::MakeLeafEmitter(0.75f, 0.70f, emitter_count++,
                layerIndex, layerIndex, 13);
            emitters.AddEmitter(emitter_cfg, emitter_anchor);
        }
        particles.Init();

        auto lastTime = glfwGetTime();
        bool toggleParticlesWasDown = false;
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

            for (decltype(layers)::size_type i = 0; i < layers.size(); ++i) {
                layers[i].Render(INTERNAL_WIDTH, INTERNAL_HEIGHT, quad);
                emitters.RenderAfterLayer(INTERNAL_WIDTH, INTERNAL_HEIGHT, i);
            }

            for (size_t i = 0; i < layers.size(); ++i) {
                layers[i].Update(static_cast<float>(delta));
            }

            emitters.Update(layers);
            particles.Update(static_cast<float>(delta));

            int winW, winH;
            glfwGetFramebufferSize(window, &winW, &winH);
            fb.RenderToScreen(quad, winW, winH, static_cast<float>(currentTime));

            glfwSwapBuffers(window);
            glfwPollEvents();

            const bool toggleParticlesDown = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;
            if (toggleParticlesDown && !toggleParticlesWasDown)
                particles.ToggleEnabled();
            toggleParticlesWasDown = toggleParticlesDown;

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

            constexpr float scrollAdjustRate = 0.6f;

            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                for (auto& layer : layers)
                    layer.SetScrollSpeed(layer.GetScrollSpeed() + scrollAdjustRate * static_cast<float>(delta));

            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                for (auto& layer : layers)
                    layer.SetScrollSpeed(layer.GetScrollSpeed() - scrollAdjustRate * static_cast<float>(delta));

            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                for (auto& layer : layers)
                    layer.SetScrollSpeedY(layer.GetScrollSpeedY() + scrollAdjustRate * static_cast<float>(delta));

            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                for (auto& layer : layers)
                    layer.SetScrollSpeedY(layer.GetScrollSpeedY() - scrollAdjustRate * static_cast<float>(delta));

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_RELEASE)
                break;
        }
    }

    glfwTerminate();
    return 0;
}
