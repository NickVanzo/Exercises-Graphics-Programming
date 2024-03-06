#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;
in float Height;

out vec4 FragColor;

uniform vec4 Color;
uniform vec2 ColorTextureScale;
uniform vec2 ColorTextureRange01;
uniform vec2 ColorTextureRange12;
uniform vec2 ColorTextureRange23;
uniform sampler2D ColorTexture;

void main()
{
	vec4 color0 = texture(ColorTexture, TexCoord * ColorTextureScale);

	FragColor = Color * color0;
}
