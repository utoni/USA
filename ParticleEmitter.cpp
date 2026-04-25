#include "ParticleEmitter.hpp"

void ParticleEmitter::RenderAfterLayer(int width, int height, unsigned int layerIndex)
{
    for (auto& anchor : Anchors) {
        if (anchor.RenderAfterLayer == layerIndex)
            Particles.RenderEmitter(anchor.EmitterIndex, width, height);
    }
}

void ParticleEmitter::AddEmitter(const ParticleSystem::EmitterConfig& cfg, const EmitterAnchor& anchor)
{
    Particles.AddEmitter(cfg);
    Anchors.push_back(anchor);
}

void ParticleEmitter::Update(const std::vector<Layer>& layers)
{
    for (const auto& anchor : Anchors) {
        if (anchor.LayerIndex >= layers.size()) continue;

        // --- Horizontal ---
        float currOffsetX = layers[anchor.LayerIndex].GetScrollOffset();
        float prevOffsetX = layers[anchor.LayerIndex].GetPreviousScrollOffset();
        float scrollDeltaX = currOffsetX - prevOffsetX;
        if (scrollDeltaX >  0.5f) scrollDeltaX -= 1.0f;
        if (scrollDeltaX < -0.5f) scrollDeltaX += 1.0f;

        // --- Vertical ---
        float currOffsetY = layers[anchor.LayerIndex].GetScrollOffsetY();
        float prevOffsetY = layers[anchor.LayerIndex].GetPreviousScrollOffsetY();
        float scrollDeltaY = currOffsetY - prevOffsetY;
        if (scrollDeltaY >  0.5f) scrollDeltaY -= 1.0f;
        if (scrollDeltaY < -0.5f) scrollDeltaY += 1.0f;

        Particles.ShiftEmitterParticles(anchor.EmitterIndex, -scrollDeltaX, -scrollDeltaY);
        float x = Wrap01(anchor.BaseSpawnPoint.x - currOffsetX);
        float y = anchor.BaseSpawnPoint.y - currOffsetY;
        Particles.SetEmitterSpawnPoint(anchor.EmitterIndex, x, y);
    }
}

std::tuple<ParticleSystem::EmitterConfig, ParticleEmitter::EmitterAnchor>
ParticleEmitter::MakeLeafEmitter(
    float x, float y, unsigned int emitterIndex,
    unsigned int layerIndex, unsigned int renderAfterLayer,
    unsigned int seed
) {
    ParticleEmitter::EmitterAnchor anchor;
    anchor.EmitterIndex = emitterIndex;
    anchor.LayerIndex = layerIndex;
    anchor.RenderAfterLayer = renderAfterLayer;
    anchor.BaseSpawnPoint = {x, y};

    ParticleSystem::EmitterConfig config;
    config.SpawnRate = 10.0f;
    config.BurstCount = 20;
    config.BurstOnStart = true;
    config.MaxParticles = 60;
    config.LifetimeRange = {4.8f, 4.6f};
    config.InitialVelocityXRange = {-0.018f, 0.018f};
    config.InitialVelocityYRange = {-0.05f, -0.02f};
    config.UseBoxSpawn = true;
    config.SpawnPoint = {x, y};
    config.SpawnBoxHalfSize = {0.05f, 0.03f};
    config.Gravity = {0.0f, -0.05f};
    config.Damping = 0.15f;
    config.SizeRange = {0.007f, 0.011f};
    config.AngularVelocityRange = {-0.9f, 0.9f};
    config.Color = {0.1f, 0.1f, 0.1f, 0.5f};
    config.FadeOutFraction = 0.35f;
    config.WindStrength = 0.055f;
    config.WindFrequency = 2.0f;
    config.Seed = seed;

    return {config, anchor};
}

float ParticleEmitter::Wrap01(float value)
{
    value = std::fmod(value, 1.0f);
    if (value < 0.0f)
        value += 1.0f;
    return value;
}
