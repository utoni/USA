#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex;
uniform int enableGodrays;
uniform int showGodraysMask;
uniform int godraysSourceMode; // 0 = moon sprite source, 1 = directional off-screen source
uniform vec2 moonScreenPos;    // normalized screen-space source position
uniform vec2 moonDirection;    // normalized light direction for directional mode
uniform int godraysLightCount;
uniform vec2 godraysLightPositions[16];

// Godray tuning parameters:
uniform float godraysIntensity;
uniform float godraysExposure;
uniform float godraysDecay;
uniform float godraysDensity;
uniform float godraysWeight;
uniform int godraysSamples;
uniform vec3 godraysColor;
uniform float godraysNoiseAmount;
uniform int enableFireflies;
uniform float fireflyIntensity;
uniform float fireflyDensity;
uniform float fireflySize;
uniform float fireflySpeed;
uniform float timeSeconds;

const int MAX_GODRAY_SAMPLES = 96;
const int MAX_GODRAY_LIGHT_SOURCES = 16;
const int MAX_GODRAY_SOURCES_PER_PIXEL = 4;

float hash(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

vec2 hash2(vec2 p)
{
    return vec2(
        hash(p + vec2(17.0, 31.0)),
        hash(p + vec2(59.0, 83.0))
    );
}

float luminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

vec2 getLightSourceUV()
{
    if (godraysSourceMode == 0)
        return moonScreenPos;

    vec2 dir = moonDirection;
    float dirLen = length(dir);
    if (dirLen <= 0.0001)
        // Fallback to a vertical moonlight direction when direction is invalid.
        dir = vec2(0.0, -1.0);
    else
        dir /= dirLen;

    // Convert directional moonlight into an off-screen source point.
    // 0.85 keeps the source just outside the screen so shafts look natural.
    return vec2(0.5, 0.5) - dir * 0.85;
}

float getOcclusionMask(vec2 uv, vec2 sourceUV)
{
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
        return 0.0;

    vec3 color = texture(tex, uv).rgb;
    // Luminance thresholds selecting brighter pixels as godray contributors.
    float bright = smoothstep(0.22, 0.95, luminance(color));
    // Distance falloff from source to keep rays localized and pixel-art friendly.
    float sourceFalloff = 1.0 - clamp(length(uv - sourceUV) * 1.35, 0.0, 1.0);
    return bright * sourceFalloff;
}

vec3 sampleRaysFromSource(vec2 sourceUV, int sampleCount)
{
    vec2 delta = (TexCoord - sourceUV) * (godraysDensity / float(sampleCount));
    vec3 rays = vec3(0.0);
    float illuminationDecay = 1.0;
    vec2 sampleUV = TexCoord;

    for (int i = 0; i < MAX_GODRAY_SAMPLES; ++i) {
        if (i >= sampleCount)
            break;

        sampleUV -= delta;
        float sampleMask = getOcclusionMask(sampleUV, sourceUV);
        rays += sampleMask * illuminationDecay * godraysWeight * godraysColor;
        illuminationDecay *= godraysDecay;
    }

    return rays;
}

vec3 sampleFireflies()
{
    if (enableFireflies == 0 || fireflyIntensity <= 0.0001)
        return vec3(0.0);

    vec2 gridScale = vec2(46.0, 26.0) * max(fireflyDensity, 0.25);
    vec2 gridPos = TexCoord * gridScale;
    vec2 baseCell = floor(gridPos);
    vec3 particles = vec3(0.0);

    for (int y = -1; y <= 1; ++y) {
        for (int x = -1; x <= 1; ++x) {
            vec2 cell = baseCell + vec2(float(x), float(y));
            vec2 rnd = hash2(cell);

            float phase = timeSeconds * fireflySpeed * (0.45 + rnd.x * 1.35);
            vec2 drift = vec2(
                sin(phase + rnd.y * 6.2831853),
                cos(phase * 0.72 + rnd.x * 6.2831853)
            ) * 0.22;

            vec2 particleUV = (cell + rnd + drift) / gridScale;
            vec2 toParticle = TexCoord - particleUV;
            toParticle.x *= gridScale.x / gridScale.y;

            float radius = fireflySize * (0.0045 + rnd.x * 0.0025);
            float glow = smoothstep(radius, 0.0, length(toParticle));
            float twinkle = 0.35 + 0.65 * (0.5 + 0.5 * sin(timeSeconds * (1.7 + rnd.y * 3.2) + rnd.x * 17.0));

            particles += vec3(0.48, 0.62, 0.30) * glow * twinkle;
        }
    }

    return particles * fireflyIntensity;
}

void main()
{
    vec4 scene = texture(tex, TexCoord);
    vec3 rays = vec3(0.0);

    if (enableGodrays != 0) {
        int sampleCount = clamp(godraysSamples, 1, MAX_GODRAY_SAMPLES);
        vec2 maskDebugSourceUV = vec2(0.0);

        if (godraysSourceMode == 0) {
            int sourceCount = clamp(godraysLightCount, 0, MAX_GODRAY_LIGHT_SOURCES);
            if (sourceCount == 0) {
                rays += sampleRaysFromSource(moonScreenPos, sampleCount);
                maskDebugSourceUV = moonScreenPos;
            } else {
                int cappedSourceCount = min(sourceCount, MAX_GODRAY_SOURCES_PER_PIXEL);
                if (cappedSourceCount > 0) {
                    int perSourceSampleCount = sampleCount / cappedSourceCount;
                    int remainingSamples = sampleCount % cappedSourceCount;

                    for (int i = 0; i < MAX_GODRAY_LIGHT_SOURCES; ++i) {
                        if (i >= cappedSourceCount)
                            break;
                        int sourceSamples = perSourceSampleCount;
                        if (remainingSamples > 0) {
                            sourceSamples += 1;
                            remainingSamples -= 1;
                        }

                        if (sourceSamples <= 0)
                            continue;

                        rays += sampleRaysFromSource(godraysLightPositions[i], sourceSamples);
                    }
                }
                maskDebugSourceUV = godraysLightPositions[0];
            }
        } else {
            vec2 sourceUV = getLightSourceUV();
            rays += sampleRaysFromSource(sourceUV, sampleCount);
            maskDebugSourceUV = sourceUV;
        }

        float dither = (hash(gl_FragCoord.xy + vec2(timeSeconds * 17.0)) - 0.5) * godraysNoiseAmount;
        rays *= godraysExposure * godraysIntensity;
        // Keep dithering subtle to avoid overpowering sprite details.
        rays += dither * godraysColor * 0.15;

        if (showGodraysMask != 0) {
            float mask = getOcclusionMask(TexCoord, maskDebugSourceUV);
            FragColor = vec4(vec3(mask), 1.0);
            return;
        }
    }

    vec3 fireflies = sampleFireflies();
    FragColor = vec4(scene.rgb + rays + fireflies, scene.a);
}
