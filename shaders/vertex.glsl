#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;
uniform float offset;
uniform mat4 mvp;

void main() {
    TexCoord = vec2(aTex.x + offset, aTex.y);
    gl_Position = mvp * vec4(aPos, 0.0, 1.0);
}
