#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex;
uniform float offset;

void main() {
    float texelSize = 1.0 / 1024.0;
    vec2 coords = TexCoord + vec2(offset, 0.0);
    coords.x = floor(coords.x / texelSize) * texelSize;
    FragColor = texture(tex, coords);
    if (FragColor.a < 0.01) discard;
    //FragColor = vec4(fract(TexCoord.x), fract(TexCoord.y), 0.0, 1.0);
}
