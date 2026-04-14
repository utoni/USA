#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec4 aColor;

out vec2 TexCoord;
out vec4 ParticleColor;
uniform mat4 mvp;

void main()
{
    TexCoord = aUV;
    ParticleColor = aColor;
    gl_Position = mvp * vec4(aPos, 0.0, 1.0);
}
