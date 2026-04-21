#pragma once

#include "Shader.hpp"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <random>
#include <vector>

class ParticleSystem
{
public:
    struct Particle
    {
        glm::vec2 Position = {0.0f, 0.0f};
        glm::vec2 Velocity = {0.0f, 0.0f};
        glm::vec2 Acceleration = {0.0f, 0.0f};
        float Rotation = 0.0f;
        float AngularVelocity = 0.0f;
        float Size = 0.0f;
        glm::vec4 Color = {1.0f, 1.0f, 1.0f, 1.0f};
        float Lifetime = 0.0f;
        float Age = 0.0f;
        float WindPhase = 0.0f;
        bool Active = false;
    };

    struct EmitterConfig
    {
        float SpawnRate = 0.0f; // Particles/sec.
        int BurstCount = 0;
        bool BurstOnStart = false;
        int MaxParticles = 64;
        glm::vec2 LifetimeRange = {2.0f, 4.0f};
        glm::vec2 InitialVelocityXRange = {-0.01f, 0.01f};
        glm::vec2 InitialVelocityYRange = {-0.2f, -0.1f};
        bool UseBoxSpawn = false;
        glm::vec2 SpawnPoint = {0.0f, 0.0f};
        glm::vec2 SpawnBoxHalfSize = {0.0f, 0.0f};
        glm::vec2 Gravity = {0.0f, -0.1f};
        float Damping = 0.0f;
        glm::vec2 SizeRange = {0.01f, 0.02f};
        glm::vec2 AngularVelocityRange = {-1.0f, 1.0f};
        glm::vec4 Color = {1.0f, 1.0f, 1.0f, 1.0f};
        float FadeOutFraction = 0.35f;
        float WindStrength = 0.015f;
        float WindFrequency = 1.6f;
        unsigned int Seed = 1;
    };

    explicit ParticleSystem(const Shader& shader);
    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem(ParticleSystem&&) = delete;
    ParticleSystem& operator=(const ParticleSystem&) = delete;
    ParticleSystem& operator=(ParticleSystem&&) = delete;
    ~ParticleSystem();

    size_t AddEmitter(const EmitterConfig& config);
    void SetEmitterSpawnPoint(size_t emitterIndex, float x, float y);
    void ShiftEmitterParticles(size_t emitterIndex, float dx, float dy);
    void Init();
    void Update(float deltaTimeSeconds);
    void RenderEmitter(size_t emitterIndex, int width, int height);

    void SetEnabled(bool enabled) { Enabled = enabled; }
    [[nodiscard]]
    bool IsEnabled() const { return Enabled; }
    void ToggleEnabled() { SetEnabled(!IsEnabled()); }
    void ResetBurst(size_t emitterIndex);
    void SetSpawnEnabled(size_t emitterIndex, bool enabled);

private:
    struct ParticleVertex
    {
        float X;
        float Y;
        float U;
        float V;
        float R;
        float G;
        float B;
        float A;
    };

    struct EmitterState
    {
        EmitterConfig Config;
        std::vector<Particle> Particles;
        std::mt19937 Random;
        float SpawnAccumulator = 0.0f;
        int Cursor = 0;
        bool BurstDone = false;
        bool SpawnEnabled = true;
    };

    static float RandomRange(std::mt19937& random, float min, float max);
    static float Clamp01(float value);
    void CreateLeafTexture();
    void SpawnParticle(EmitterState& emitter);
    void EnsureVertexBufferCapacity(size_t requiredVertexCount);
    void PushParticleQuad(const Particle& particle,
                          float alphaScale);

    const Shader& ParticleShader;
    struct {
        int MVP = -1;
        int Texture = -1;
    } Locations;
    std::vector<EmitterState> Emitters;
    std::vector<ParticleVertex> Vertices;
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int ParticleTextureID = 0;
    size_t VertexCapacity = 0;
    bool Enabled = true;
};
