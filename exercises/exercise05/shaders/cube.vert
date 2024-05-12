#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in uint VoxelType;
layout (location = 2) in int VoxelMask;

out vec3 WorldPosition;
flat out uint VoxelGeom;
flat out int Mask;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

void main()
{
    VoxelGeom = VoxelType;
    Mask = VoxelMask;
    WorldPosition = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz;
    gl_Position = vec4(WorldPosition, 1.0);
}
