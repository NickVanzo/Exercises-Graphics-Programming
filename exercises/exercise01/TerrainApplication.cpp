#include "TerrainApplication.h"
#include <ituGl/geometry/VertexArrayObject.h>
#include <ituGl/geometry/VertexAttribute.h>
#include "Vector3.h"
#include <cmath>
#include <iostream>
#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"
// (todo) 01.8: Declare an struct with the vertex format

struct Vertex {
    Vector3 position;
    Vector2 textureCoordinate;
    Vector3 color;
    Vector3 normal;
};

TerrainApplication::TerrainApplication()
    : Application(1024, 1024, "Terrain demo"), m_gridX(64), m_gridY(64)
{
}

void TerrainApplication::Initialize()
{
    const Vector3 green(0,255,0);
    const Vector3 red(0,0,255);
    Application::Initialize();

    BuildShaderProgram();

    // (todo) 01.1: Create containers for the vertex position
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    unsigned int columnCount = m_gridX + 1;
    unsigned int rowCount = m_gridY + 1;

    Vector2 gridSize(1.0f/m_gridX, 1.0f/m_gridY);

    for (int j = 0; j < rowCount; ++j)
    {
        for (int i = 0; i < columnCount; ++i)
        {

            auto x = i * gridSize.x - 0.5f;
            auto y = j * gridSize.y - 0.5f;
            auto z = stb_perlin_fbm_noise3(x, y, 0.0f, 2.0f, 0.5f, 6);
            // Index data for quad formed by previous vertices and current
            if (i > 0 && j > 0)
            {
                unsigned int top_right = j * columnCount + i; // Current vertex
                unsigned int top_left = top_right - 1;
                unsigned int bottom_right = top_right - columnCount;
                unsigned int bottom_left = bottom_right - 1;

                //Triangle 1
                indices.push_back(bottom_left);
                indices.push_back(bottom_right);
                indices.push_back(top_left);

                //Triangle 2
                indices.push_back(bottom_right);
                indices.push_back(top_left);
                indices.push_back(top_right);
            }

            Vector3 pos(x, y, z);
            Vector2 textCoordinate(i,j);
            Vertex v;

            v.position = pos;
            v.textureCoordinate = textCoordinate;

            if(z < 0.5) {
                v.color = green;
            } else {
                v.color = red;
            }

            vertices.push_back(v);
        }
    }

    for (int j = 0; j < rowCount; ++j)
    {
        for (int i = 0; i < columnCount; ++i)
        {
            auto& currVertex = vertices[i + j * columnCount];
            Vertex leftVertex = currVertex;
            Vertex rightVertex = currVertex;
            Vertex upVertex = currVertex;
            Vertex downVertex = currVertex;

            //compute the left vertex
            if(i > 0) {
                leftVertex = vertices[i - 1 + j * columnCount];
            }

            //compute the right vertex
            if(i < m_gridX) {
                rightVertex = vertices[i + 1 + j * columnCount];
            }

            //compute the up vertex
            if(j > 0) {
                upVertex = vertices[i + j * columnCount - columnCount];
            }

            if(j < m_gridY) {
                downVertex = vertices[i + j * columnCount + columnCount];
            }
            auto deltaX = (rightVertex.position.z - leftVertex.position.z) / (rightVertex.position.x - leftVertex.position.x);
            auto deltaY = (upVertex.position.z - downVertex.position.z) / (upVertex.position.y - downVertex.position.y);

            currVertex.normal = Vector3(deltaX, deltaY, 1).Normalize();
        }
    }


    // Declare attributes
    VertexAttribute positionAttribute(Data::Type::Float, 3);
    VertexAttribute texCoordAttribute(Data::Type::Float, 2);
    VertexAttribute colorAttribute(Data::Type::Float, 3);
    VertexAttribute normalAttribute(Data::Type::Float, 3);

    m_vbo.Bind();
    m_vbo.AllocateData(std::span(vertices));

    m_vao.Bind();

    float posOffset = 0u;
    float textOffset = posOffset + positionAttribute.GetSize();
    float colorOffset = textOffset + colorAttribute.GetSize();
    float normalOffset = colorOffset + normalAttribute.GetSize();

    m_vao.SetAttribute(0, positionAttribute, posOffset, sizeof(Vertex));
    m_vao.SetAttribute(1, texCoordAttribute, textOffset, sizeof(Vertex));
    m_vao.SetAttribute(2, colorAttribute, colorOffset, sizeof(Vertex));
    m_vao.SetAttribute(3, normalAttribute, normalOffset, sizeof(Vertex));

    // (todo) 01.5: Initialize EBO
    m_ebo.Bind();
    m_ebo.AllocateData(std::span(indices));

    // (todo) 01.1: Unbind VAO, and VBO
    m_vbo.Unbind();
    m_vao.Unbind();
    m_ebo.Unbind();

    // (todo) 01.5: Unbind EBO

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
}

void TerrainApplication::Update()
{
    Application::Update();
    UpdateOutputMode();
    
}

void TerrainApplication::Render()
{
    Application::Render();

    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
    glUseProgram(m_shaderProgram);
    m_vao.Bind();
    glDrawElements(GL_TRIANGLES, m_gridX * m_gridY * 6, GL_UNSIGNED_INT, nullptr);
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
void TerrainApplication::UpdateOutputMode()
{
    for (int i = 0; i <= 4; ++i)
    {
        if (GetMainWindow().IsKeyPressed(GLFW_KEY_0 + i))
        {
            int modeLocation = glGetUniformLocation(m_shaderProgram, "Mode");
            glUseProgram(m_shaderProgram);
            glUniform1ui(modeLocation, i);
            break;
        }
    }
    if (GetMainWindow().IsKeyPressed(GLFW_KEY_TAB))
    {
        const float projMatrix[16] = { 0, -1.294f, -0.721f, -0.707f, 1.83f, 0, 0, 0, 0, 1.294f, -0.721f, -0.707f, 0, 0, 1.24f, 1.414f };
        int matrixLocation = glGetUniformLocation(m_shaderProgram, "Matrix");
        glUseProgram(m_shaderProgram);
        glUniformMatrix4fv(matrixLocation, 1, false, projMatrix);
    }
}