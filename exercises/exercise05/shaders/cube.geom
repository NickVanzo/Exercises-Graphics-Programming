#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 24) out;

uniform mat4 ViewProjMatrix;

out vec3 WorldPosition;
const float size = 1.0;

void createVertex(vec3 offset){
    vec4 actualOffset = vec4(offset * size, 1.0);
    vec4 worldPos = gl_in[0].gl_Position + actualOffset;
    gl_Position = ViewProjMatrix * worldPos;
    WorldPosition = gl_Position.xyz;
    EmitVertex();
}

void build_cube()
{
    // Front face
    createVertex(vec3(-1.0,  1.0,  1.0));
    createVertex(vec3(-1.0, -1.0,  1.0));
    createVertex(vec3( 1.0,  1.0,  1.0));
    createVertex(vec3( 1.0, -1.0,  1.0));
    EndPrimitive();

    // Right face
    createVertex(vec3( 1.0,  1.0,  1.0));
    createVertex(vec3( 1.0, -1.0,  1.0));
    createVertex(vec3( 1.0,  1.0, -1.0));
    createVertex(vec3( 1.0, -1.0, -1.0));
    EndPrimitive();

    // Back face
    createVertex(vec3( 1.0,  1.0, -1.0));
    createVertex(vec3( 1.0, -1.0, -1.0));
    createVertex(vec3(-1.0,  1.0, -1.0));
    createVertex(vec3(-1.0, -1.0, -1.0));
    EndPrimitive();

    // Left face
    createVertex(vec3(-1.0,  1.0, -1.0));
    createVertex(vec3(-1.0, -1.0, -1.0));
    createVertex(vec3(-1.0,  1.0,  1.0));
    createVertex(vec3(-1.0, -1.0,  1.0));
    EndPrimitive();

    // Top face
    createVertex(vec3( 1.0,  1.0,  1.0));
    createVertex(vec3( 1.0,  1.0, -1.0));
    createVertex(vec3(-1.0,  1.0,  1.0));
    createVertex(vec3(-1.0,  1.0, -1.0));
    EndPrimitive();

    // Bottom face
    createVertex(vec3(-1.0, -1.0,  1.0));
    createVertex(vec3(-1.0, -1.0, -1.0));
    createVertex(vec3( 1.0, -1.0,  1.0));
    createVertex(vec3( 1.0, -1.0, -1.0));
    EndPrimitive();
}

void main() {
    build_cube();
}
