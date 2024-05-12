#version 330 core

flat in uint VoxelType;
in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2DArray TextureArray;

void main()
{
    FragColor = texture(TextureArray, vec3(TexCoord.xy, VoxelType));
//    FragColor = vec4((Normal.xyz + vec3(1)) / 2.0, 1.0);
}
