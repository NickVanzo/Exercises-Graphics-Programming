#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/camera/Camera.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/utils/DearImGui.h>
#include <ituGl/geometry/VertexBufferObject.h>
#include <ituGl/geometry/VertexArrayObject.h>
#include "Vector3.h"
#include "ituGL/geometry/Mesh.h"

class Texture2DArrayObject;

struct Vertex {
    Vector3 position;
};

class MinecraftApplication : public Application
{
public:
    MinecraftApplication();

protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;

private:
    void InitializeCamera();
    void InitializeLights();
    void InitializeMaterials();
    void InitializeTextures();
    void InitializeMeshes();
    void CreateTerrainMesh(Mesh& mesh, unsigned int gridX, unsigned int gridY, unsigned int gridZ);
    std::vector<Vertex> GenerateMountains(Mesh& mesh, unsigned int gridX, unsigned int gridZ);
    float GenerateVoxelDensity(glm::vec2 voxelPos);
    void DrawObject(const Mesh& mesh, Material& material, const glm::mat4& worldMatrix);
    int GetVoxelType(float height, float density);

    void UpdateCamera();
    void RenderGUI();
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
    unsigned int m_cloudHeight;
    bool m_cameraEnabled;
    bool m_cameraEnablePressed;

    // Loaded model
    Model m_model;
    Mesh m_terrainPatch;

    // Add light variables
    glm::vec3 m_ambientColor;
    glm::vec3 m_lightColor;
    float m_lightIntensity;
    glm::vec3 m_lightPosition;

    // Specular exponent debug
    float m_specularExponentGrass;
    unsigned int m_gridX, m_gridY, m_gridZ;

    VertexBufferObject m_vbo;
    VertexArrayObject m_vao;

    std::shared_ptr<Material> m_grassMaterial;
    std::shared_ptr<Texture2DArrayObject> m_textures;
};
