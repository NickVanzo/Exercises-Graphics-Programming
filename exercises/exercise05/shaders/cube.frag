#version 330 core

out vec4 FragColor;
uniform sampler2D GrassTexture;

void main()
{
    FragColor = texture(GrassTexture, vec2(1,0));
}
