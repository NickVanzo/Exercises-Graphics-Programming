#pragma once

#include <ituGL/application/Application.h>

#include <ituGL/camera/Camera.h>
#include <ituGL/geometry/Model.h>
#include <ituGL/utils/DearImGui.h>
#include <ituGl/geometry/VertexBufferObject.h>
#include <ituGl/geometry/VertexArrayObject.h>
#include "Vector3.h"
#include "ituGL/geometry/Mesh.h"
#include <unordered_set>
class Texture2DArrayObject;

struct Vertex {
    Vector3 position;
};

//I have to use this because glm::vec3 has some problems when used inside an unordered_map
struct Face {
    // Store vertex indices or positions here
    float vertex1, vertex2, vertex3;
    bool operator==(const Face& other) const {
        bool areEqual =vertex1 == other.vertex1 && vertex2 == other.vertex2 && vertex3 == other.vertex3;
        return areEqual;
    }
    struct HashFunction {
        size_t operator()(const Face& face) const {
            size_t seed = 0;
            // Combine hashes of individual vertex indices
            hash_combine(seed, face.vertex1);
            hash_combine(seed, face.vertex2);
            hash_combine(seed, face.vertex3);
            return seed;
        }

        // Utility function to combine hashes
        template <typename T>
        void hash_combine(size_t& seed, const T& v) const {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    };

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
    float GenerateVoxelDensity(glm::vec3 voxelPos);
    void DrawObject(const Mesh& mesh, Material& material, const glm::mat4& worldMatrix);
    int GetVoxelType(float height, float density);
    void InsertFace(int x, int y, int z, std::unordered_set<Face, Face::HashFunction>& faces);
    void UpdateCamera();
    void RenderGUI();
private:
    // Helper object for debug GUI
    DearImGui m_imGui;
    std::unordered_set<Face, Face::HashFunction> duplicatedPositions;
    // Mouse position for camera controller
    glm::vec2 m_mousePosition;

    // Camera controller parameters
    Camera m_camera;
    glm::vec3 m_cameraPosition;
    float m_cameraTranslationSpeed;
    float m_cameraRotationSpeed;
    int m_cloudHeight;
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
