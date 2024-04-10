#version 330 core

layout (location = 0) in vec3 VertexPosition;

out vec3 WorldPosition;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

void main()
{
    WorldPosition = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz;
    gl_Position = vec4(WorldPosition, 1.0);
}
