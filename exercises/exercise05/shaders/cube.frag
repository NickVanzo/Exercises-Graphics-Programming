#version 330 core

flat in uint VoxelType;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2DArray TextureArray;

void main()
{
    FragColor = texture(TextureArray, vec3(TexCoord.xy, VoxelType));
}
