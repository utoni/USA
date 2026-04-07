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
#include "Shader.hpp"
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

        Shader shader;
        shader.CompileAndLink("shaders/vertex.glsl", "shaders/fragment.glsl");

        Framebuffer fb(INTERNAL_WIDTH, INTERNAL_HEIGHT, shader);
        fb.Init();

        Quad quad;
        quad.Create();

        std::vector<Layer> layers = {
            Layer{ { texMgr.Get("background") }, shader, 0.05f },
            Layer{ { texMgr.Get("midground") }, shader, 0.10f },
            Layer{ { texMgr.Get("foreground3") }, shader, 0.15f },
            Layer{ { texMgr.Get("foreground2") }, shader, 0.20f },
            Layer{ { texMgr.Get("foreground") }, shader, 0.35f },
        };

        float lastTime = glfwGetTime();

        while (!glfwWindowShouldClose(window)) {
            float currentTime = glfwGetTime();
            float delta = currentTime - lastTime;
            lastTime = currentTime;

            fb.Use();
            for (auto& layer : layers) {
                layer.Render(INTERNAL_WIDTH, INTERNAL_HEIGHT, quad);
            }

            for (auto& layer : layers) {
                layer.Update(delta);
            }

            int winW, winH;
            glfwGetFramebufferSize(window, &winW, &winH);
            fb.RenderToScreen(quad, winW, winH);

            glfwSwapBuffers(window);
            glfwPollEvents();
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_RELEASE)
                break;
        }
    }

    glfwTerminate();
    return 0;
}
