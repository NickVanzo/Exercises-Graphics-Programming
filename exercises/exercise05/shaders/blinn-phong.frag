#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform vec3 AmbientColor;
uniform float AmbientReflection;
uniform float DiffuseReflection;
uniform vec3 LightColor;
uniform vec3 LightPosition;

vec3 GetDiffuseReflection(vec3 color, vec3 lightVector, vec3 worldNormal)
{
    return LightColor * DiffuseReflection * color * max(dot(lightVector, worldNormal), 0.0f);
}

vec3 GetAmbientReflection(vec3 color)
{
    return (AmbientColor * AmbientReflection * color);
}

vec3 GetBlinnPhongReflection(vec3 color, vec3 lightVector, vec3 worldNormal)
{
    return GetAmbientReflection(color) + GetDiffuseReflection(color, lightVector, worldNormal);
}

void main()
{
    vec4 objectColor = Color * texture(ColorTexture, TexCoord);
    vec3 lightVector = normalize(LightPosition - WorldPosition);
    vec3 normalVector = normalize(WorldNormal);
    FragColor = vec4(GetBlinnPhongReflection(objectColor.rgb, lightVector, normalVector), 1.0f);
}

