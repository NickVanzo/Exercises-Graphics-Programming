#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform vec2 ColorTextureScale;
uniform vec4 Color;
uniform sampler2D ColorTexture;

void main()
{
    FragColor = Color * texture(ColorTexture, TexCoord * ColorTextureScale);
}
