#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

uniform mat4 ViewProjMatrix;

out vec2 TexCoord;
out vec3 WorldPosition;
flat in uint VoxelGeom[];
flat out uint VoxelType;

const float size = 1.0;

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
    createVertex(vec3(-0.5,  0.5,  0.5), vec2(0.0, 1.0) );
    createVertex(vec3(-0.5, -0.5,  0.5), vec2(0.0, 0.0) );
    createVertex(vec3( 0.5,  0.5,  0.5), vec2(1.0, 1.0) );
    createVertex(vec3( 0.5, -0.5,  0.5), vec2(1.0, 0.0) );
    EndPrimitive();

    // Right face
    createVertex(vec3( 0.5, 0.5, 0.5), vec2(0.0, 1.0));
    createVertex(vec3( 0.5, -0.5,  0.5), vec2(0.0, 0.0));
    createVertex(vec3( 0.5,  0.5, -0.5), vec2(1.0, 1.0));
    createVertex(vec3( 0.5, -0.5, -0.5), vec2(1.0, 0.0));
    EndPrimitive();

    // Back face
    createVertex(vec3( 0.5,  0.5, -0.5), vec2(0.0, 1.0));
    createVertex(vec3( 0.5, -0.5, -0.5), vec2(0.0, 0.0));
    createVertex(vec3(-0.5,  0.5, -0.5), vec2(1.0, 1.0));
    createVertex(vec3(-0.5, -0.5, -0.5), vec2(1.0, 0.0));
    EndPrimitive();

    // Left face
    createVertex(vec3(-0.5,  0.5, -0.5), vec2(0.0, 1.0));
    createVertex(vec3(-0.5, -0.5, -0.5), vec2(0.0, 0.0));
    createVertex(vec3(-0.5,  0.5,  0.5), vec2(1.0, 1.0));
    createVertex(vec3(-0.5, -0.5,  0.5), vec2(1.0, 0.0));
    EndPrimitive();

    // Top face
    createVertex(vec3( 0.5,  0.5,  0.5), vec2(0.0, 1.0));
    createVertex(vec3( 0.5,  0.5, -0.5), vec2(0.0, 0.0));
    createVertex(vec3(-0.5,  0.5,  0.5), vec2(1.0, 1.0));
    createVertex(vec3(-0.5,  0.5, -0.5), vec2(1.0, 0.0));
    EndPrimitive();

    // Bottom face
    createVertex(vec3(-0.5, -0.5,  0.5), vec2(0.0, 1.0));
    createVertex(vec3(-0.5, -0.5, -0.5), vec2(0.0, 0.0));
    createVertex(vec3( 0.5, -0.5,  0.5), vec2(1.0, 1.0));
    createVertex(vec3( 0.5, -0.5, -0.5), vec2(1.0, 0.0));
    EndPrimitive();
}

void main() {
    VoxelType = VoxelGeom[0];
    build_cube();
}
