#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

uniform mat4 ViewProjMatrix;

out vec2 TexCoord;
out vec3 WorldPosition;
out vec3 Normal;
flat in uint VoxelGeom[];
flat in int Mask[];
flat out uint VoxelType;

const float size = 1.0;
int VoxelMaskGeom;

const int FRONT_MASK = 0x00000001;
const int BACK_MASK = 0x00000002;
const int RIGHT_MASK = 0x00000004;
const int LEFT_MASK = 0x00000008;
const int TOP_MASK = 0x00000010;
const int BOTTOM_MASK = 0x00000020;

void createVertex(vec3 offset, vec2 texCoord, vec3 normal){
    vec4 actualOffset = vec4(offset * size, 1.0);
    vec4 worldPos = gl_in[0].gl_Position + actualOffset;
    TexCoord = texCoord;
    Normal = normal;
    gl_Position = ViewProjMatrix * worldPos;
    WorldPosition = gl_Position.xyz;
    EmitVertex();
}

void build_cube()
{
    // Front face
    if ((VoxelMaskGeom & FRONT_MASK) != FRONT_MASK) {
        createVertex(vec3(-0.5, 0.5, 0.5), vec2(0.0, 1.0), vec3(0.0, 0.0, 1.0));
        createVertex(vec3(-0.5, -0.5, 0.5), vec2(0.0, 0.0), vec3(0.0, 0.0, 1.0));
        createVertex(vec3(0.5, 0.5, 0.5), vec2(1.0, 1.0), vec3(0.0, 0.0, 1.0));
        createVertex(vec3(0.5, -0.5, 0.5), vec2(1.0, 0.0), vec3(0.0, 0.0, 1.0));
        EndPrimitive();
    }

    // Right face
    if ((VoxelMaskGeom & RIGHT_MASK) != RIGHT_MASK) {
        createVertex(vec3(0.5, 0.5, 0.5), vec2(0.0, 1.0), vec3(1, 0, 0));
        createVertex(vec3(0.5, -0.5, 0.5), vec2(0.0, 0.0), vec3(1, 0, 0));
        createVertex(vec3(0.5, 0.5, -0.5), vec2(1.0, 1.0), vec3(1, 0, 0));
        createVertex(vec3(0.5, -0.5, -0.5), vec2(1.0, 0.0), vec3(1, 0, 0));
        EndPrimitive();
    }

    // Back face
    if ((VoxelMaskGeom & BACK_MASK) != BACK_MASK) {
        createVertex(vec3(0.5, 0.5, -0.5), vec2(0.0, 1.0), vec3(0, 0, -1));
        createVertex(vec3(0.5, -0.5, -0.5), vec2(0.0, 0.0), vec3(0, 0, -1));
        createVertex(vec3(-0.5, 0.5, -0.5), vec2(1.0, 1.0), vec3(0, 0, -1));
        createVertex(vec3(-0.5, -0.5, -0.5), vec2(1.0, 0.0), vec3(0, 0, -1));
        EndPrimitive();
    }

    // Left face
    if ((VoxelMaskGeom & LEFT_MASK) != LEFT_MASK) {
        createVertex(vec3(-0.5, 0.5, -0.5), vec2(0.0, 1.0), vec3(-1, 0, 0));
        createVertex(vec3(-0.5, -0.5, -0.5), vec2(0.0, 0.0), vec3(-1, 0, 0));
        createVertex(vec3(-0.5, 0.5, 0.5), vec2(1.0, 1.0), vec3(-1, 0, 0));
        createVertex(vec3(-0.5, -0.5, 0.5), vec2(1.0, 0.0), vec3(-1, 0, 0));
        EndPrimitive();
    }

    // Top face
    if ((VoxelMaskGeom & TOP_MASK) != TOP_MASK) {
        createVertex(vec3(0.5, 0.5, 0.5), vec2(0.0, 1.0), vec3(0, 1, 0));
        createVertex(vec3(0.5, 0.5, -0.5), vec2(0.0, 0.0), vec3(0, 1, 0));
        createVertex(vec3(-0.5, 0.5, 0.5), vec2(1.0, 1.0), vec3(0, 1, 0));
        createVertex(vec3(-0.5, 0.5, -0.5), vec2(1.0, 0.0), vec3(0, 1, 0));
        EndPrimitive();
    }

    // Bottom face
    if ((VoxelMaskGeom & BOTTOM_MASK) != BOTTOM_MASK) {
        createVertex(vec3(-0.5, -0.5, 0.5), vec2(0.0, 1.0), vec3(0, -1, 0));
        createVertex(vec3(-0.5, -0.5, -0.5), vec2(0.0, 0.0), vec3(0, -1, 0));
        createVertex(vec3(0.5, -0.5, 0.5), vec2(1.0, 1.0), vec3(0, -1, 0));
        createVertex(vec3(0.5, -0.5, -0.5), vec2(1.0, 0.0), vec3(0, -1, 0));
        EndPrimitive();
    }
}

void main() {
    VoxelType = VoxelGeom[0];
    VoxelMaskGeom = Mask[0];
    build_cube();
}
