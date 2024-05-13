#include "MinecraftApplication.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>
#include <ituGL/shader/Material.h>
#include <glm/gtx/euler_angles.hpp>
#include <ituGL/asset/Texture2DArrayLoader.h>
#include <glm/gtx/transform.hpp>
#include <ituGL/geometry/VertexFormat.h>
#include <time.h>

#define STB_PERLIN_IMPLEMENTATION

#include "stb_perlin.h"

#define STB_IMAGE_IMPLEMENTATION

#include <algorithm>
#include <set>
#include <iostream>


MinecraftApplication::MinecraftApplication()
        : Application(1024, 1024, "Viewer demo"), m_cameraPosition(125, 75, 75), m_gridY(50), m_gridX(50), m_gridZ(50),
          m_cameraTranslationSpeed(20.0f), m_cameraRotationSpeed(0.5f), m_cameraEnabled(true),
          m_cameraEnablePressed(true), m_mousePosition(GetMainWindow().GetMousePosition(true)), m_ambientColor(0.0f),
          m_lightColor(0.0f), m_lightIntensity(0.0f), m_lightPosition(0.0f), m_specularExponentGrass(100.0f),
          m_cloudHeight(150) {}

void MinecraftApplication::Initialize() {
    Application::Initialize();
    srand((unsigned) time(NULL));

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeTextures();
    InitializeCamera();
    InitializeLights();
    InitializeMaterials();
    InitializeMeshes();

    DeviceGL &device = GetDevice();
    device.EnableFeature(GL_DEPTH_TEST);
    device.SetVSyncEnabled(true);
}

void MinecraftApplication::InitializeMeshes() {
    CreateTerrainMesh(m_terrainPatch, m_gridX, m_gridY, m_gridZ);
}

void MinecraftApplication::CreateTerrainMesh(Mesh &mesh, unsigned int gridX, unsigned int gridY, unsigned int gridZ) {
    struct Vertex {
        Vertex() = default;

        Vertex(const glm::vec3 &position, unsigned int voxelType, int mask = 0)
                : position(position), voxelType(voxelType), mask(mask) {}

        glm::vec3 position;
        unsigned int voxelType;
        int mask;
    };

    // Define the vertex format (should match the vertex structure)
    VertexFormat vertexFormat;
    vertexFormat.AddVertexAttribute<float>(3);
    vertexFormat.AddVertexAttribute<unsigned int>(1, false);
    vertexFormat.AddVertexAttribute<int>(1, false);

    // List of vertices (VBO)
    std::vector<Vertex> vertices;

    // Number of columns and rows
    unsigned int columnCount = gridX;
    unsigned int rowCount = gridY;
    unsigned int depthCount = gridZ;
    //The height from which we'll start to apply the noise
    int heightThresholdNoise = 1;
    float noiseThreshold = 0.08;

    // all taken positions
    std::unordered_set<Face, Face::HashFunction> takenPositions;

    for (int j = 0; j < rowCount; ++j) {
        for (int i = 0; i < columnCount; ++i) {
            for (int z = 0; z < depthCount; ++z) {
                float normalizedX = (float) i / (float) rowCount - 1;
                float normalizedY = (float) j / (float) columnCount - 1;
                float normalizedZ = (float) z / (float) depthCount - 1;

                float noise = stb_perlin_fbm_noise3(normalizedX * 3, normalizedY * 3, normalizedZ * 2, 1.9f, 0.5f, 8) * 25.0f;

                if(noise > noiseThreshold || j >= rowCount - heightThresholdNoise) {
                    // Vertex data for this vertex only
                    glm::vec3 position(i, j, z);

                    InsertFace(i, j, z, takenPositions);
                    vertices.emplace_back(position, GetVoxelType(j, GenerateVoxelDensity(glm::vec3(j, i, z))));
                }
            }
        }
    }

    //generate mountain vertex info
    for(int j = 0; j < gridX; ++j) {
        for(int i = 0; i < gridZ; ++i) {
            float normalizedX = (float) j / (float) gridX - 1;
            float normalizedZ = (float) i / (float) gridZ - 1;
            float noise = stb_perlin_fbm_noise3(normalizedX * 2, 0.0f, normalizedZ * 2, 1.9f, 0.5f, 8) * 35.0f;
            float height = noise + rowCount;

            for(int h = 0; h < height; ++h) {
                if(h >= rowCount - heightThresholdNoise) {
                    if(h > height - 3) {
                        vertices.emplace_back(glm::vec3(j, h, i), 0);
                    } else {
                        vertices.emplace_back(glm::vec3(j, h, i), 1);
                    }
                    InsertFace(j, h, i , takenPositions);
                }
            }
        }
    }

    for(int i = 0; i < gridX; i++) {
        for(int j = 0; j < gridZ; ++j) {
            float normalizedX = (float) i / (float) gridX - 1;
            float normalizedZ = (float) j / (float) gridZ - 1;
            float noise = stb_perlin_fbm_noise3(normalizedX * 1.5, 0.0f, normalizedZ * 1.5, 2.0f, 1.5f, 4);
            if(noise > 0.35f) {
                vertices.emplace_back(glm::vec3(i, m_cloudHeight, j), GetVoxelType(m_cloudHeight, GenerateVoxelDensity(glm::vec3(i,
                                                                                                                                 m_cloudHeight, j))));
                InsertFace(i, m_cloudHeight, j, takenPositions);
            }
        }
    }

    if (!vertices.empty()) {
        for (int i = 0; i < vertices.size(); ++i) {
            auto v = vertices[i];

            float x = static_cast<float>(v.position.x);
            float y = static_cast<float>(v.position.y);
            float z = static_cast<float>(v.position.z);
            float range = 0.5f;

            Face up = {x, y + range, z};
            Face down = {x, y - range, z};
            Face right = {x + range, y, z};
            Face left = {x - range, y, z};
            Face front = {x, y, z + range};
            Face back = {x, y, z - range};

            if (duplicatedPositions.contains(front)) {
                vertices[i].mask |= 0x00000001;
            }
            if (duplicatedPositions.contains(back)) {
                vertices[i].mask |= 0x00000002;
            }
            if (duplicatedPositions.contains(right)) {
                vertices[i].mask |= 0x00000004;
            }
            if (duplicatedPositions.contains(left)) {
                vertices[i].mask |= 0x00000008;
            }
            if (duplicatedPositions.contains(up)) {
                vertices[i].mask |= 0x00000010;
            }
            if (duplicatedPositions.contains(down)) {
                vertices[i].mask |= 0x00000020;
            }
        }
    }


    mesh.AddSubmesh<Vertex, VertexFormat::LayoutIterator>(
            Drawcall::Primitive::Points,
            vertices,
            vertexFormat.LayoutBegin(static_cast<int>(vertices.size()), true),
            vertexFormat.LayoutEnd()
    );
}

void MinecraftApplication::InsertFace(int x, int y, int z, std::unordered_set<Face, Face::HashFunction> &takenPositions) {
    Face up = {(float) x, y + 0.5f, (float) z};
    Face down = {(float) x, y - 0.5f, (float) z};
    Face left = {x - 0.5f, (float) y, (float) z};
    Face right = {x + 0.5f, (float) y, (float) z};
    Face front = {(float) x, (float) y, z + 0.5f};
    Face back = {(float) x, (float) y, z - 0.5f};

    std::vector<Face> newFaces = {up, down, left, right, front, back};

    for (const auto &face: newFaces) {
        if (!takenPositions.contains(face)) {
            takenPositions.insert(face);
        } else {
            duplicatedPositions.insert(face);
        }
    }
}


enum VoxelTypes {
    GRASS = 0,
    STONE = 1,
    DIAMOND = 2,
    CLOUD = 3,
    WATER = 4,
    COAL = 5,
    GOLD = 6
};

int MinecraftApplication::GetVoxelType(float height, float density) {
    // Height thresholds for different layers
    float diamondHeight = m_gridY / 5;
    float cloudHeight = m_cloudHeight;

    if (height == cloudHeight) {
        return VoxelTypes::CLOUD;
    }

    if (height < m_gridY - 4) {
        if (height < diamondHeight) {
            if (density > 0.35) {
                return VoxelTypes::DIAMOND;
            } else if (density > 0.34 && density < 0.35) {
                return VoxelTypes::GOLD;
            } else {
                return VoxelTypes::STONE;
            }
        } else {
            if (density < -0.30) {
                return VoxelTypes::COAL;
            } else {
                return VoxelTypes::STONE;
            }
        }
    } else {
        return VoxelTypes::STONE;
    }
}

float MinecraftApplication::GenerateVoxelDensity(glm::vec3 voxelPos) {
    return stb_perlin_fbm_noise3(voxelPos.x * 2, voxelPos.y * 2, voxelPos.z * 2, 1.9f, 0.5f, 16);
}

void MinecraftApplication::DrawObject(const Mesh &mesh, Material &material, const glm::mat4 &worldMatrix) {
    material.Use();

    ShaderProgram &shaderProgram = *material.GetShaderProgram();
    ShaderProgram::Location locationWorldMatrix = shaderProgram.GetUniformLocation("WorldMatrix");
    material.GetShaderProgram()->SetUniform(locationWorldMatrix, worldMatrix);
    ShaderProgram::Location locationViewProjMatrix = shaderProgram.GetUniformLocation("ViewProjMatrix");
    material.GetShaderProgram()->SetUniform(locationViewProjMatrix, m_camera.GetViewProjectionMatrix());

    mesh.DrawSubmesh(0);
}


void MinecraftApplication::Update() {
    Application::Update();

    // Update camera controller
    UpdateCamera();
}

void MinecraftApplication::Render() {
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
    DrawObject(m_terrainPatch, *m_grassMaterial, glm::mat4(1.0f));

    // Render the debug user interface
    RenderGUI();
}

void MinecraftApplication::Cleanup() {
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    Application::Cleanup();
}

void MinecraftApplication::InitializeTextures() {
    Texture2DArrayLoader loader(TextureObject::FormatRGB, TextureObject::InternalFormatRGB8);
    std::vector<const char *> paths = {"textures/grass.png", "textures/stone.png", "textures/diamond.png",
                                       "textures/cloud.png", "textures/water.jpeg", "textures/coal.png",
                                       "textures/gold.png"};
    m_textures = std::make_shared<Texture2DArrayObject>(loader.Load(paths));
}

void MinecraftApplication::InitializeMaterials() {
    Shader vertexShader = ShaderLoader::Load(Shader::VertexShader, "shaders/cube.vert");
    Shader fragmentShader = ShaderLoader::Load(Shader::FragmentShader, "shaders/cube.frag");
    Shader geometryShader = ShaderLoader::Load(Shader::GeometryShader, "shaders/cube.geom");

    std::shared_ptr<ShaderProgram> terrainShaderProgram = std::make_shared<ShaderProgram>();
    terrainShaderProgram->Build(vertexShader, fragmentShader, geometryShader);

    m_grassMaterial = std::make_shared<Material>(terrainShaderProgram);
    m_grassMaterial->SetUniformValue("TextureArray", m_textures);
}

void MinecraftApplication::InitializeCamera() {
    // Set view matrix, from the camera position looking to the origin
    m_camera.SetViewMatrix(m_cameraPosition, glm::vec3(0.0f));

    // Set perspective matrix
    float aspectRatio = GetMainWindow().GetAspectRatio();
    m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, 0.1f, 1000.0f);
}

void MinecraftApplication::InitializeLights() {
    // Initialize light variables
    m_ambientColor = glm::vec3(0.25f);
    m_lightColor = glm::vec3(1.0f);
    m_lightIntensity = 1.0f;
    m_lightPosition = glm::vec3(-10.0f, 20.0f, 10.0f);
}

void MinecraftApplication::RenderGUI() {
    m_imGui.BeginFrame();
    m_imGui.EndFrame();
}

void MinecraftApplication::UpdateCamera() {
    Window &window = GetMainWindow();

    // Update if camera is enabled (controlled by SPACE key)
    {
        bool enablePressed = window.IsKeyPressed(GLFW_KEY_SPACE);
        if (enablePressed && !m_cameraEnablePressed) {
            m_cameraEnabled = !m_cameraEnabled;

            window.SetMouseVisible(!m_cameraEnabled);
            m_mousePosition = window.GetMousePosition(true);
        }
        m_cameraEnablePressed = enablePressed;
    }

    if (!m_cameraEnabled)
        return;

    glm::mat4 viewTransposedMatrix = glm::transpose(m_camera.GetViewMatrix());
    glm::vec3 viewRight = viewTransposedMatrix[0];
    glm::vec3 viewForward = -viewTransposedMatrix[2];

    // Update camera translation
    {
        glm::vec2 inputTranslation(0.0f);

        if (window.IsKeyPressed(GLFW_KEY_A))
            inputTranslation.x = -1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_D))
            inputTranslation.x = 1.0f;

        if (window.IsKeyPressed(GLFW_KEY_W))
            inputTranslation.y = 1.0f;
        else if (window.IsKeyPressed(GLFW_KEY_S))
            inputTranslation.y = -1.0f;

        inputTranslation *= m_cameraTranslationSpeed;
        inputTranslation *= GetDeltaTime();

        // Double speed if SHIFT is pressed
        if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
            inputTranslation *= 2.0f;

        m_cameraPosition += inputTranslation.x * viewRight + inputTranslation.y * viewForward;
    }

    // Update camera rotation
    {
        glm::vec2 mousePosition = window.GetMousePosition(true);
        glm::vec2 deltaMousePosition = mousePosition - m_mousePosition;
        m_mousePosition = mousePosition;

        glm::vec3 inputRotation(-deltaMousePosition.x, deltaMousePosition.y, 0.0f);

        inputRotation *= m_cameraRotationSpeed;

        viewForward =
                glm::rotate(inputRotation.x, glm::vec3(0, 1, 0)) * glm::rotate(inputRotation.y, glm::vec3(viewRight)) *
                glm::vec4(viewForward, 0);
    }

    // Update view matrix
    m_camera.SetViewMatrix(m_cameraPosition, m_cameraPosition + viewForward);
}

