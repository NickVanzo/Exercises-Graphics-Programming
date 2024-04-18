#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2DArray TextureArray;

void main()
{
    FragColor = texture(TextureArray, vec3(TexCoord, 0));
}
