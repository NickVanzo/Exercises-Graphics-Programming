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
uniform sampler2D ColorTexture1;
uniform sampler2D ColorTexture2;
uniform sampler2D ColorTexture3;

float inverseMix(vec2 range, float height) {
	if(height <= range.x) {
		return 0;
	} else if(height >= range.y) {
		return 1;
	} else {
		return (height - range.x) / (range.y - range.x);
	}
}

void main()
{
	vec4 color0 = texture(ColorTexture, TexCoord * ColorTextureScale);
	vec4 color1 = texture(ColorTexture1, TexCoord * ColorTextureScale);
	vec4 color2 = texture(ColorTexture2, TexCoord * ColorTextureScale);
	vec4 color3 = texture(ColorTexture3, TexCoord * ColorTextureScale);

	vec4 color = color0;
	color = mix(color, color1, inverseMix(ColorTextureRange01, Height));
	color = mix(color, color2, inverseMix(ColorTextureRange12, Height));
	color = mix(color, color3, inverseMix(ColorTextureRange23, Height));

	FragColor = Color * color;
}
