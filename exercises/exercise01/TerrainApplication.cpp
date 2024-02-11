#include "TerrainApplication.h"
#include <ituGl/geometry/VertexArrayObject.h>
#include <ituGl/geometry/VertexAttribute.h>
#include "Vector3.h"
#include <cmath>
#include <iostream>

// (todo) 01.8: Declare an struct with the vertex format

TerrainApplication::TerrainApplication()
    : Application(1024, 1024, "Terrain demo"), m_gridX(16), m_gridY(16)
{
}

void TerrainApplication::Initialize()
{
    Application::Initialize();

    BuildShaderProgram();

    // (todo) 01.1: Create containers for the vertex position
    std::vector<Vector3> vertices;

    Vector2 gridSize(1.0f/m_gridX, 1.0f/m_gridY);
    float offset = 0.5;

    // (todo) 01.1: Fill in vertex data
    for(size_t j = 0; j < m_gridX; ++j) {
        for(size_t i = 0; i < m_gridY; ++i) {
            float left = i * gridSize.x - offset;
            float right = (i + 1) * gridSize.x - offset;
            float bottom = j * gridSize.y - offset;
            float top = (j + 1) * gridSize.y - offset;

            vertices.push_back(Vector3(left, bottom, 0));
            vertices.push_back(Vector3(right,bottom, 0));
            vertices.push_back(Vector3(left, top, 0));

            vertices.push_back(Vector3(right,bottom, 0));
            vertices.push_back(Vector3(left, top, 0)),
            vertices.push_back(Vector3(right,top, 0));
        }
    }

    m_vbo.Bind();
    m_vbo.AllocateData<Vector3>(vertices);

    m_vao.Bind();
    VertexAttribute vAttribute(Data::Type::Float, 3);
    m_vao.SetAttribute(0, vAttribute, 0);

    // (todo) 01.5: Initialize EBO

    // (todo) 01.1: Unbind VAO, and VBO
    m_vbo.Unbind();
    m_vao.Unbind();


    // (todo) 01.5: Unbind EBO

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void TerrainApplication::Update()
{
    Application::Update();
}

void TerrainApplication::Render()
{
    Application::Render();

    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
    glUseProgram(m_shaderProgram);
    m_vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, m_gridY * m_gridX * 6);
}

void TerrainApplication::Cleanup()
{
    Application::Cleanup();
}

// build the shader program
// ------------------------
void TerrainApplication::BuildShaderProgram()
{
    const char* vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "layout (location = 1) in vec2 aTexCoord;\n"
                                     "layout (location = 2) in vec3 aColor;\n"
                                     "layout (location = 3) in vec3 aNormal;\n"
                                     "uniform mat4 Matrix = mat4(1);\n"
                                     "out vec2 texCoord;\n"
                                     "out vec3 color;\n"
                                     "out vec3 normal;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   texCoord = aTexCoord;\n"
                                     "   color = aColor;\n"
                                     "   normal = aNormal;\n"
                                     "   gl_Position = Matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
                                       "uniform uint Mode = 0u;\n"
                                       "in vec2 texCoord;\n"
                                       "in vec3 color;\n"
                                       "in vec3 normal;\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   switch (Mode)\n"
                                       "   {\n"
                                       "   default:\n"
                                       "   case 0:\n"
                                       "       FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
                                       "       break;\n"
                                       "   case 1:\n"
                                       "       FragColor = vec4(fract(texCoord), 0.0f, 1.0f);\n"
                                       "       break;\n"
                                       "   case 2:\n"
                                       "       FragColor = vec4(color, 1.0f);\n"
                                       "       break;\n"
                                       "   case 3:\n"
                                       "       FragColor = vec4(normalize(normal), 1.0f);\n"
                                       "       break;\n"
                                       "   case 4:\n"
                                       "       FragColor = vec4(color * max(dot(normalize(normal), normalize(vec3(1,0,1))), 0.2f), 1.0f);\n"
                                       "       break;\n"
                                       "   }\n"
                                       "}\n\0";

    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    m_shaderProgram = shaderProgram;
}
