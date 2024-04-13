#include "MinecraftApplication.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>
#include <ituGL/shader/Material.h>
#include <glm/gtx/euler_angles.hpp>
#include <ituGL/texture/Texture2DObject.h>
#include <glm/gtx/transform.hpp>
#include <ituGL/geometry/VertexFormat.h>
#include "iostream"
#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <algorithm>



MinecraftApplication::MinecraftApplication()
    : Application(1024, 1024, "Viewer demo")
    , m_cameraPosition(50, 30, 30)
    , m_gridY(50)
    , m_gridX(100)
    , m_gridZ(100)
    , m_cameraTranslationSpeed(20.0f)
    , m_cameraRotationSpeed(0.5f)
    , m_cameraEnabled(true)
    , m_cameraEnablePressed(true)
    , m_mousePosition(GetMainWindow().GetMousePosition(true))
    , m_ambientColor(0.0f)
    , m_lightColor(0.0f)
    , m_lightIntensity(0.0f)
    , m_lightPosition(0.0f)
    , m_specularExponentGrass(100.0f)
{}

void MinecraftApplication::Initialize()
{
    Application::Initialize();

    // Initialize DearImGUI
    m_imGui.Initialize(GetMainWindow());

    InitializeCamera();
    InitializeLights();
    InitializeMaterials();
    InitializeMeshes();

    DeviceGL& device = GetDevice();
    device.EnableFeature(GL_DEPTH_TEST);
    device.SetVSyncEnabled(true);
}

void MinecraftApplication::InitializeMeshes() {
    CreateTerrainMesh(m_terrainPatch, m_gridX, m_gridY, m_gridZ);
}

void MinecraftApplication::CreateTerrainMesh(Mesh& mesh, unsigned int gridX, unsigned int gridY, unsigned int gridZ)
{
    // Define the vertex structure
    struct Vertex
    {
        Vertex() = default;
        Vertex(const glm::vec3& position)
                : position(position) {}
        glm::vec3 position;
    };

    // Define the vertex format (should match the vertex structure)
    VertexFormat vertexFormat;
    vertexFormat.AddVertexAttribute<float>(3);

    // List of vertices (VBO)
    std::vector<Vertex> vertices;

    // Number of columns and rows
    unsigned int columnCount = gridX;
    unsigned int rowCount = gridY;
    unsigned int depthCount = gridZ;
    //The height from which we'll start to apply the noise
    int heightThresholdNoise = 5;
    float noiseThreshold = 0.08;

    for (unsigned int j = 0; j < rowCount; ++j)
    {
        for (unsigned int i = 0; i < columnCount; ++i)
        {
            for(unsigned int z = 0; z < depthCount; ++z) {
                float normalizedX = (float) i / (float) rowCount - 1;
                float normalizedY = (float) j / (float) columnCount - 1;
                float normalizedZ = (float) z / (float) depthCount - 1;

                float noise = stb_perlin_fbm_noise3(normalizedX * 2, normalizedY * 2, normalizedZ * 2, 1.9f, 0.5f, 8) * 0.5f;

                if(noise > noiseThreshold || j > rowCount - heightThresholdNoise) {
                    // Vertex data for this vertex only
                    glm::vec3 position(i, j, z);
                    vertices.emplace_back(position);
                }
            }
        }
    }

    //generate mountain vertex info
    for(int j = 0; j < gridX; ++j) {
        for(int i = 0; i < gridZ; ++i) {
            float normalizedX = (float) j / (float) gridX - 1;
            float normalizedZ = (float) i / (float) gridZ - 1;
            float noise = stb_perlin_fbm_noise3(normalizedX * 2, 0.0f, normalizedZ * 2, 1.9f, 0.5f, 8) * 50.0f;
            float height =  noise + rowCount;
            for(int h = 0; h < height; ++h) {
                if(h > rowCount - heightThresholdNoise)
                    vertices.emplace_back(glm::vec3(j, h, i));
            }
        }
    }

    mesh.AddSubmesh<Vertex, VertexFormat::LayoutIterator>(
            Drawcall::Primitive::Points,
            vertices,
            vertexFormat.LayoutBegin(static_cast<int>(vertices.size()), false),
            vertexFormat.LayoutEnd()
            );
}


void MinecraftApplication::DrawObject(const Mesh& mesh, Material& material, const glm::mat4& worldMatrix)
{
    material.Use();

    ShaderProgram& shaderProgram = *material.GetShaderProgram();
    ShaderProgram::Location locationWorldMatrix = shaderProgram.GetUniformLocation("WorldMatrix");
    material.GetShaderProgram()->SetUniform(locationWorldMatrix, worldMatrix);
    ShaderProgram::Location locationViewProjMatrix = shaderProgram.GetUniformLocation("ViewProjMatrix");
    material.GetShaderProgram()->SetUniform(locationViewProjMatrix, m_camera.GetViewProjectionMatrix());

    mesh.DrawSubmesh(0);
}


void MinecraftApplication::Update()
{
    Application::Update();

    // Update camera controller
    UpdateCamera();
}

void MinecraftApplication::Render()
{
    Application::Render();

    // Clear color and depth
    GetDevice().Clear(true, Color(0.0f, 0.0f, 0.0f, 1.0f), true, 1.0f);
    DrawObject(m_terrainPatch, *m_grassMaterial, glm::mat4(1.0f));

    // Render the debug user interface
    RenderGUI();
}

void MinecraftApplication::Cleanup()
{
    // Cleanup DearImGUI
    m_imGui.Cleanup();

    Application::Cleanup();
}

void MinecraftApplication::InitializeMaterials() {
    Shader vertexShader = ShaderLoader::Load(Shader::VertexShader, "shaders/cube.vert");
    Shader fragmentShader = ShaderLoader::Load(Shader::FragmentShader, "shaders/cube.frag");
    Shader geometryShader = ShaderLoader::Load(Shader::GeometryShader, "shaders/cube.geom");

    std::shared_ptr<ShaderProgram> terrainShaderProgram = std::make_shared<ShaderProgram>();
    terrainShaderProgram->Build(vertexShader, fragmentShader, geometryShader);

    m_grassTexture = LoadTexture("textures/grass.png");

    m_grassMaterial = std::make_shared<Material>(terrainShaderProgram);
    m_grassMaterial->SetUniformValue("GrassTexture", m_grassTexture);
}

std::shared_ptr<Texture2DObject> MinecraftApplication::LoadTexture(const char *path) {
    std::shared_ptr<Texture2DObject> texture = std::make_shared<Texture2DObject>();
    int width = 0;
    int height = 0;
    int components = 0;

    unsigned char* data = stbi_load(path, &width, &height, &components, 4);

    texture->Bind();
    texture->SetImage(0, width, height, TextureObject::FormatRGBA, Texture2DObject::InternalFormatRGBA, std::span<const unsigned char>(data, width * height * 4));

    texture->GenerateMipmap();

    return texture;
}

void MinecraftApplication::InitializeCamera()
{
    // Set view matrix, from the camera position looking to the origin
    m_camera.SetViewMatrix(m_cameraPosition, glm::vec3(0.0f));

    // Set perspective matrix
    float aspectRatio = GetMainWindow().GetAspectRatio();
    m_camera.SetPerspectiveProjectionMatrix(1.0f, aspectRatio, 0.1f, 1000.0f);
}

void MinecraftApplication::InitializeLights()
{
    // Initialize light variables
    m_ambientColor = glm::vec3(0.25f);
    m_lightColor = glm::vec3(1.0f);
    m_lightIntensity = 1.0f;
    m_lightPosition = glm::vec3(-10.0f, 20.0f, 10.0f);
}

void MinecraftApplication::RenderGUI()
{
    m_imGui.BeginFrame();
    m_imGui.EndFrame();
}

void MinecraftApplication::UpdateCamera()
{
    Window& window = GetMainWindow();

    // Update if camera is enabled (controlled by SPACE key)
    {
        bool enablePressed = window.IsKeyPressed(GLFW_KEY_SPACE);
        if (enablePressed && !m_cameraEnablePressed)
        {
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

        viewForward = glm::rotate(inputRotation.x, glm::vec3(0,1,0)) * glm::rotate(inputRotation.y, glm::vec3(viewRight)) * glm::vec4(viewForward, 0);
    }

   // Update view matrix
   m_camera.SetViewMatrix(m_cameraPosition, m_cameraPosition + viewForward);
}

