#pragma once

#include "Layer.hpp"
#include "ParticleSystem.hpp"

#include <glm/vec2.hpp>
#include <vector>

class ParticleEmitter
{
public:
    struct EmitterAnchor
    {
        unsigned int EmitterIndex = -1;
        unsigned int LayerIndex = -1;
        unsigned int RenderAfterLayer = -1;
        unsigned int TextureIndexInLayer = 0;
        glm::vec2 BaseSpawnPoint;
    };

public:
    ParticleEmitter(ParticleSystem& ps)
        : Particles(ps)
    {}
    void RenderAfterLayer(int width, int height, unsigned int currentLayerIndex);
    void AddEmitter(const ParticleSystem::EmitterConfig& cfg, const EmitterAnchor& anchor);
    void Update(const std::vector<Layer>& layers);

    static std::tuple<ParticleSystem::EmitterConfig, EmitterAnchor> MakeLeafEmitter(
        float x, float y, unsigned int emitterIndex,
        unsigned int layerIndex, unsigned int renderAfterLayer,
        unsigned int seed
    );

private:
    float Wrap01(float value);

    ParticleSystem& Particles;
    std::vector<EmitterAnchor> Anchors;
};
