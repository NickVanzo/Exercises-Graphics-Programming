#pragma once

#include <ituGL/application/Application.h>
#include <ituGl/geometry/VertexBufferObject.h>
#include <ituGl/geometry/VertexArrayObject.h>
#include <ituGL/geometry/ElementBufferObject.h>
#include "Vector3.h"
// (todo) 01.1: Include the libraries you need


class TerrainApplication : public Application
{
public:
    TerrainApplication();
    const std::vector<Vector3>& GetVertices() const {return vertices;};
    std::vector<Vector3>& GetVertices() {return vertices;};

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;
    void BuildShaderProgram();
    void UpdateOutputMode();

private:
    unsigned int m_gridX, m_gridY;
    unsigned int m_shaderProgram;

    // (todo) 01.1: Declare an VBO, VAO
    VertexBufferObject m_vbo;
    VertexArrayObject m_vao;

    // (todo) 01.5: Declare an EBO
    ElementBufferObject m_ebo;
    std::vector<Vector3> vertices;

};
