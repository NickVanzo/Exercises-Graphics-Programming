#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in uint VoxelType;

out vec3 WorldPosition;
flat out uint VoxelGeom;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;


void main()
{
    VoxelGeom = VoxelType;
    WorldPosition = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz;
    gl_Position = vec4(WorldPosition, 1.0);
}
