#include "ParticleSystem.hpp"

#include "Transform.hpp"
#include "glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

namespace
{
    constexpr float Tau = 6.28318530718f;
    constexpr float LeafHalfWidth = 0.245f;
    constexpr float LeafHalfHeight = 0.447f;
    constexpr std::uint8_t LeafBaseRed = 190;
    constexpr std::uint8_t LeafRedTint = 40;
    constexpr std::uint8_t LeafBaseGreen = 90;
    constexpr std::uint8_t LeafGreenTint = 25;
    constexpr std::uint8_t LeafBaseBlue = 45;
    constexpr std::uint8_t LeafBlueTint = 10;
}

ParticleSystem::ParticleSystem(const Shader& shader)
    : ParticleShader(shader)
{
}

ParticleSystem::~ParticleSystem()
{
    if (ParticleTextureID != 0)
        glDeleteTextures(1, &ParticleTextureID);

    if (VBO != 0)
        glDeleteBuffers(1, &VBO);

    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
}

size_t ParticleSystem::AddEmitter(const EmitterConfig& config)
{
    EmitterState emitter;
    emitter.Config = config;
    emitter.Config.MaxParticles = std::max(1, emitter.Config.MaxParticles);
    emitter.Random.seed(config.Seed);
    emitter.Particles.resize(static_cast<size_t>(emitter.Config.MaxParticles));
    Emitters.push_back(std::move(emitter));
    return Emitters.size() - 1;
}

void ParticleSystem::SetEmitterSpawnPoint(size_t emitterIndex, float x, float y)
{
    if (emitterIndex >= Emitters.size())
        return;

    Emitters[emitterIndex].Config.SpawnPoint = {x, y};
}

void ParticleSystem::ShiftEmitterParticles(size_t emitterIndex, float dx, float dy)
{
    if (emitterIndex >= Emitters.size())
        return;
    for (auto& p : Emitters[emitterIndex].Particles) {
        if (!p.Active) continue;
        p.Position.x += dx;
        p.Position.y += dy;
    }
}

void ParticleSystem::ResetBurst(size_t emitterIndex)
{
    if (emitterIndex >= Emitters.size())
        return;
    Emitters[emitterIndex].BurstDone = false;
}

void ParticleSystem::SetSpawnEnabled(size_t emitterIndex, bool enabled)
{
    if (emitterIndex >= Emitters.size())
        return;
    Emitters[emitterIndex].SpawnEnabled = enabled;
}

void ParticleSystem::Init()
{
    Locations.MVP = ParticleShader.GetUniformLocation("mvp");
    Locations.Texture = ParticleShader.GetUniformLocation("tex");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
                          reinterpret_cast<void*>(2 * sizeof(float))); // NOLINT(performance-no-int-to-ptr)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex),
                          reinterpret_cast<void*>(4 * sizeof(float))); // NOLINT(performance-no-int-to-ptr)
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CreateLeafTexture();
}

float ParticleSystem::RandomRange(std::mt19937& random, float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(random);
}

float ParticleSystem::Clamp01(float value)
{
    if (value < 0.0f)
        return 0.0f;
    if (value > 1.0f)
        return 1.0f;
    return value;
}

void ParticleSystem::CreateLeafTexture()
{
    constexpr int textureSize = 8;
    constexpr auto size = static_cast<size_t>(textureSize) * textureSize * 4;
    std::array<std::uint8_t, size> data = {};
    for (int y = 0; y < textureSize; ++y)
    {
        for (int x = 0; x < textureSize; ++x)
        {
            const float px = (static_cast<float>(x) + 0.5f) / static_cast<float>(textureSize) - 0.5f;
            const float py = (static_cast<float>(y) + 0.5f) / static_cast<float>(textureSize) - 0.5f;
            const float leafMask = (px * px) / (LeafHalfWidth * LeafHalfWidth) +
                                   (py * py) / (LeafHalfHeight * LeafHalfHeight);
            const bool inLeaf = leafMask <= 1.0f && std::abs(px) < 0.35f;
            const int index = (y * textureSize + x) * 4;
            if (!inLeaf) {
                data[index + 0] = 0;
                data[index + 1] = 0;
                data[index + 2] = 0;
                data[index + 3] = 0;
                continue;
            }

            const float tint = Clamp01((py + 0.5f) * 1.2f);
            data[index + 0] = static_cast<std::uint8_t>(LeafBaseRed + LeafRedTint * tint);
            data[index + 1] = static_cast<std::uint8_t>(LeafBaseGreen + LeafGreenTint * tint);
            data[index + 2] = static_cast<std::uint8_t>(LeafBaseBlue + LeafBlueTint * tint);
            data[index + 3] = 255;
        }
    }

    glGenTextures(1, &ParticleTextureID);
    glBindTexture(GL_TEXTURE_2D, ParticleTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize, textureSize, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ParticleSystem::SpawnParticle(EmitterState& emitter)
{
    auto& particles = emitter.Particles;
    const auto particleCount = static_cast<int>(particles.size());
    for (int i = 0; i < particleCount; ++i)
    {
        auto& particle = particles[static_cast<size_t>(emitter.Cursor)];
        emitter.Cursor = (emitter.Cursor + 1) % particleCount;
        if (particle.Active)
            continue;

        const auto& cfg = emitter.Config;
        particle.Active = true;
        particle.Age = 0.0f;
        particle.Lifetime = RandomRange(emitter.Random, cfg.LifetimeRange.x, cfg.LifetimeRange.y);
        particle.Size = RandomRange(emitter.Random, cfg.SizeRange.x, cfg.SizeRange.y);
        particle.Color = cfg.Color;
        particle.Position = cfg.SpawnPoint;
        if (cfg.UseBoxSpawn) {
            particle.Position.x += RandomRange(emitter.Random, -cfg.SpawnBoxHalfSize.x, cfg.SpawnBoxHalfSize.x);
            particle.Position.y += RandomRange(emitter.Random, -cfg.SpawnBoxHalfSize.y, cfg.SpawnBoxHalfSize.y);
        }
        particle.Velocity = {
            RandomRange(emitter.Random, cfg.InitialVelocityXRange.x, cfg.InitialVelocityXRange.y),
            RandomRange(emitter.Random, cfg.InitialVelocityYRange.x, cfg.InitialVelocityYRange.y)
        };
        particle.Acceleration = cfg.Gravity;
        particle.Rotation = RandomRange(emitter.Random, 0.0f, Tau);
        particle.AngularVelocity = RandomRange(emitter.Random, cfg.AngularVelocityRange.x, cfg.AngularVelocityRange.y);
        particle.WindPhase = RandomRange(emitter.Random, 0.0f, Tau);
        return;
    }
}

void ParticleSystem::Update(float deltaTimeSeconds)
{
    if (!Enabled)
        return;

    for (auto& emitter : Emitters)
    {
        const auto& cfg = emitter.Config;
        if (cfg.BurstOnStart && !emitter.BurstDone && emitter.SpawnEnabled)
        {
            for (int burst = 0; burst < cfg.BurstCount; ++burst)
                SpawnParticle(emitter);
            emitter.BurstDone = true;
        }

        if (emitter.SpawnEnabled) {
            emitter.SpawnAccumulator += cfg.SpawnRate * deltaTimeSeconds;
            auto spawnCount = static_cast<int>(std::floor(emitter.SpawnAccumulator));
            if (spawnCount > 0)
                emitter.SpawnAccumulator -= static_cast<float>(spawnCount);

            for (int i = 0; i < spawnCount; ++i)
                SpawnParticle(emitter);
        }

        for (auto& particle : emitter.Particles)
        {
            if (!particle.Active)
                continue;

            particle.Age += deltaTimeSeconds;
            if (particle.Age >= particle.Lifetime) {
                particle.Active = false;
                continue;
            }

            particle.Velocity += particle.Acceleration * deltaTimeSeconds;
            const float windForce = std::sin((particle.Age + particle.WindPhase) * cfg.WindFrequency) * cfg.WindStrength;
            particle.Velocity.x += windForce * deltaTimeSeconds;
            const float damping = std::max(0.0f, 1.0f - cfg.Damping * deltaTimeSeconds);
            particle.Velocity *= damping;
            particle.Position += particle.Velocity * deltaTimeSeconds;
            particle.Rotation += particle.AngularVelocity * deltaTimeSeconds;
        }
    }
}

void ParticleSystem::EnsureVertexBufferCapacity(size_t requiredVertexCount)
{
    if (requiredVertexCount <= VertexCapacity)
        return;

    VertexCapacity = requiredVertexCount;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(VertexCapacity * sizeof(ParticleVertex)),
                 nullptr, GL_DYNAMIC_DRAW);
}

void ParticleSystem::PushParticleQuad(const Particle& particle,
                                      float alphaScale)
{
    const float cosTheta = std::cos(particle.Rotation);
    const float sinTheta = std::sin(particle.Rotation);
    const float half = particle.Size * 0.5f;

    const auto rotate = [&](float x, float y) -> glm::vec2 {
        return {
            particle.Position.x + x * cosTheta - y * sinTheta,
            particle.Position.y + x * sinTheta + y * cosTheta
        };
    };

    const glm::vec2 topLeft = rotate(-half, half);
    const glm::vec2 bottomLeft = rotate(-half, -half);
    const glm::vec2 bottomRight = rotate(half, -half);
    const glm::vec2 topRight = rotate(half, half);
    const float alpha = particle.Color.a * alphaScale;

    Vertices.push_back({topLeft.x, topLeft.y, 0.0f, 1.0f,
                        particle.Color.r, particle.Color.g, particle.Color.b, alpha});
    Vertices.push_back({bottomLeft.x, bottomLeft.y, 0.0f, 0.0f,
                        particle.Color.r, particle.Color.g, particle.Color.b, alpha});
    Vertices.push_back({bottomRight.x, bottomRight.y, 1.0f, 0.0f,
                        particle.Color.r, particle.Color.g, particle.Color.b, alpha});

    Vertices.push_back({topLeft.x, topLeft.y, 0.0f, 1.0f,
                        particle.Color.r, particle.Color.g, particle.Color.b, alpha});
    Vertices.push_back({bottomRight.x, bottomRight.y, 1.0f, 0.0f,
                        particle.Color.r, particle.Color.g, particle.Color.b, alpha});
    Vertices.push_back({topRight.x, topRight.y, 1.0f, 1.0f,
                        particle.Color.r, particle.Color.g, particle.Color.b, alpha});
}

void ParticleSystem::RenderEmitter(size_t emitterIndex, int width, int height)
{
    if (!Enabled || emitterIndex >= Emitters.size())
        return;

    Vertices.clear();
    const auto& emitter = Emitters[emitterIndex];
    const auto& cfg = emitter.Config;
    for (const auto& particle : emitter.Particles)
    {
        if (!particle.Active)
            continue;

        float alphaScale = 1.0f;
        const float fadeStartAge = particle.Lifetime * (1.0f - Clamp01(cfg.FadeOutFraction));
        const float fadeWindow = std::max(0.0001f, particle.Lifetime - fadeStartAge);
        if (particle.Age > fadeStartAge)
            alphaScale = Clamp01((particle.Lifetime - particle.Age) / fadeWindow);

        PushParticleQuad(particle, alphaScale);
    }

    if (Vertices.empty())
        return;

    EnsureVertexBufferCapacity(Vertices.size());
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    static_cast<GLsizeiptr>(Vertices.size() * sizeof(ParticleVertex)),
                    Vertices.data());

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width),
                                      0.0f, static_cast<float>(height));
    const auto model = Transform::Create2D(0.0f, 0.0f,
                                           static_cast<float>(width),
                                           static_cast<float>(height));
    glm::mat4 mvp = projection * model;

    ParticleShader.Use();
    ParticleShader.SetUniform(Locations.MVP, glm::value_ptr(mvp));
    ParticleShader.SetUniform(Locations.Texture, 0);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ParticleTextureID);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(Vertices.size()));
}
