#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;
uniform vec2 uvOffset;
uniform vec2 uvScale;
uniform mat4 mvp;

void main() {
    TexCoord = aTex * uvScale + uvOffset;
    gl_Position = mvp * vec4(aPos, 0.0, 1.0);
}
