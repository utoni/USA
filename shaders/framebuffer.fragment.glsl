#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex;
uniform int enableGodrays;
uniform int showGodraysMask;
uniform int godraysSourceMode; // 0 = moon sprite source, 1 = directional off-screen source
uniform vec2 moonScreenPos;    // normalized screen-space source position
uniform vec2 moonDirection;    // normalized light direction for directional mode

// Godray tuning parameters:
uniform float godraysIntensity;
uniform float godraysExposure;
uniform float godraysDecay;
uniform float godraysDensity;
uniform float godraysWeight;
uniform int godraysSamples;
uniform vec3 godraysColor;
uniform float godraysNoiseAmount;
uniform float timeSeconds;

const int MAX_GODRAY_SAMPLES = 96;

float hash(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
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

void main()
{
    vec4 scene = texture(tex, TexCoord);
    if (enableGodrays == 0) {
        FragColor = scene;
        return;
    }

    vec2 sourceUV = getLightSourceUV();
    vec2 delta = (TexCoord - sourceUV) * (godraysDensity / float(max(godraysSamples, 1)));

    vec3 rays = vec3(0.0);
    float illuminationDecay = 1.0;
    vec2 sampleUV = TexCoord;
    int sampleCount = clamp(godraysSamples, 1, MAX_GODRAY_SAMPLES);

    for (int i = 0; i < MAX_GODRAY_SAMPLES; ++i) {
        if (i >= sampleCount)
            break;

        sampleUV -= delta;
        float sampleMask = getOcclusionMask(sampleUV, sourceUV);
        rays += sampleMask * illuminationDecay * godraysWeight * godraysColor;
        illuminationDecay *= godraysDecay;
    }

    float dither = (hash(gl_FragCoord.xy + vec2(timeSeconds * 17.0)) - 0.5) * godraysNoiseAmount;
    rays *= godraysExposure * godraysIntensity;
    // Keep dithering subtle to avoid overpowering sprite details.
    rays += dither * godraysColor * 0.15;

    if (showGodraysMask != 0) {
        float mask = getOcclusionMask(TexCoord, sourceUV);
        FragColor = vec4(vec3(mask), 1.0);
        return;
    }

    FragColor = vec4(scene.rgb + rays, scene.a);
}
