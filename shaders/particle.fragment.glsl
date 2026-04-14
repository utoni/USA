#version 330 core
in vec2 TexCoord;
in vec4 ParticleColor;
out vec4 FragColor;

uniform sampler2D tex;

void main()
{
    vec4 texel = texture(tex, TexCoord);
    FragColor = texel * ParticleColor;
    if (FragColor.a < 0.01)
        discard;
}
