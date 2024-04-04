#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/camera/Camera.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/utils/DearImGui.h>
#include <ituGl/geometry/VertexBufferObject.h>
#include <ituGl/geometry/VertexArrayObject.h>
#include "Vector3.h"

class Texture2DObject;

struct Vertex {
    Vector3 position;
};

class ViewerApplication : public Application
{
public:
    ViewerApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:
    void InitializeCamera();
    void InitializeLights();
    void InitializeShaders();

    void UpdateCamera();
    void RenderGUI();

    /**
     * Use this function to setup VBO and VAO with vertex data
     * @return
     */
    void GenerateVertexData();


private:
    // Helper object for debug GUI
    DearImGui m_imGui;

    // Mouse position for camera controller
    glm::vec2 m_mousePosition;

    // Camera controller parameters
    Camera m_camera;
    glm::vec3 m_cameraPosition;
    float m_cameraTranslationSpeed;
    float m_cameraRotationSpeed;
    bool m_cameraEnabled;
    bool m_cameraEnablePressed;

    // Loaded model
    Model m_model;

    // Add light variables
    glm::vec3 m_ambientColor;
    glm::vec3 m_lightColor;
    float m_lightIntensity;
    glm::vec3 m_lightPosition;

    // Specular exponent debug
    float m_specularExponentGrass;

    VertexBufferObject m_vbo;
    VertexArrayObject m_vao;
};
