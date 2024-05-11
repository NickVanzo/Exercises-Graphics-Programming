#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

uniform mat4 ViewProjMatrix;

out vec2 TexCoord;
out vec3 WorldPosition;
flat in uint VoxelGeom[];
flat in int Mask[];
flat out uint VoxelType;

const float size = 1.0;
int VoxelMaskGeom;

void createVertex(vec3 offset, vec2 texCoord){
    vec4 actualOffset = vec4(offset * size, 1.0);
    vec4 worldPos = gl_in[0].gl_Position + actualOffset;
    TexCoord = texCoord;
    gl_Position = ViewProjMatrix * worldPos;
    WorldPosition = gl_Position.xyz;
    EmitVertex();
}

void build_cube()
{

    // Front face
    if((VoxelMaskGeom & 0x16) == 0) {
        createVertex(vec3(-0.5, 0.5, 0.5), vec2(0.0, 1.0));
        createVertex(vec3(-0.5, -0.5, 0.5), vec2(0.0, 0.0));
        createVertex(vec3(0.5, 0.5, 0.5), vec2(1.0, 1.0));
        createVertex(vec3(0.5, -0.5, 0.5), vec2(1.0, 0.0));
        EndPrimitive();
    }

        if((VoxelMaskGeom & 0x02) == 0) {
    // Right face
    createVertex(vec3(0.5, 0.5, 0.5), vec2(0.0, 1.0));
    createVertex(vec3(0.5, -0.5, 0.5), vec2(0.0, 0.0));
    createVertex(vec3(0.5, 0.5, -0.5), vec2(1.0, 1.0));
    createVertex(vec3(0.5, -0.5, -0.5), vec2(1.0, 0.0));
    EndPrimitive();
        }


    // Back face
        if((VoxelMaskGeom & 0x32) == 0) {
    createVertex(vec3(0.5, 0.5, -0.5), vec2(0.0, 1.0));
    createVertex(vec3(0.5, -0.5, -0.5), vec2(0.0, 0.0));
    createVertex(vec3(-0.5, 0.5, -0.5), vec2(1.0, 1.0));
    createVertex(vec3(-0.5, -0.5, -0.5), vec2(1.0, 0.0));
    EndPrimitive();
        }


    // Left face
        if((VoxelMaskGeom & 0x01) == 0) {
    createVertex(vec3(-0.5, 0.5, -0.5), vec2(0.0, 1.0));
    createVertex(vec3(-0.5, -0.5, -0.5), vec2(0.0, 0.0));
    createVertex(vec3(-0.5, 0.5, 0.5), vec2(1.0, 1.0));
    createVertex(vec3(-0.5, -0.5, 0.5), vec2(1.0, 0.0));
    EndPrimitive();
}


    // Top face
    if((VoxelMaskGeom & 0x04) == 0) {
        createVertex(vec3( 0.5,  0.5,  0.5), vec2(0.0, 1.0));
        createVertex(vec3( 0.5,  0.5, -0.5), vec2(0.0, 0.0));
        createVertex(vec3(-0.5,  0.5,  0.5), vec2(1.0, 1.0));
        createVertex(vec3(-0.5,  0.5, -0.5), vec2(1.0, 0.0));
        EndPrimitive();
    }

    // Bottom face
    if((VoxelMaskGeom & 0x08) == 0) {
        createVertex(vec3(-0.5, -0.5,  0.5), vec2(0.0, 1.0));
        createVertex(vec3(-0.5, -0.5, -0.5), vec2(0.0, 0.0));
        createVertex(vec3( 0.5, -0.5,  0.5), vec2(1.0, 1.0));
        createVertex(vec3( 0.5, -0.5, -0.5), vec2(1.0, 0.0));
        EndPrimitive();
    }

}

void main() {
    VoxelType = VoxelGeom[0];
    VoxelMaskGeom = Mask[0];
    build_cube();
}
