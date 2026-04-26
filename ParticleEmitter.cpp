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

        const auto& layer = layers[anchor.LayerIndex];

        float scrollDelta = layer.GetScrollDelta();
        Particles.ShiftEmitterParticles(anchor.EmitterIndex, -scrollDelta, 0.0f);

        float currOffset  = layer.GetScrollOffset();
        int   texCount    = layer.GetTextureCount();
        float localOffset = currOffset - std::floor(currOffset);
        int   texIndex    = static_cast<int>(std::floor(currOffset));
        int slot = (static_cast<int>(anchor.TextureIndexInLayer) - texIndex + texCount) % texCount;
        float x = static_cast<float>(slot) + anchor.BaseSpawnPoint.x - localOffset;

        if (x >= 0.0f && x <= 1.0f)
            Particles.SetEmitterSpawnPoint(anchor.EmitterIndex, x, anchor.BaseSpawnPoint.y);
        else
            Particles.SetEmitterSpawnPoint(anchor.EmitterIndex, -10.0f, anchor.BaseSpawnPoint.y);
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
