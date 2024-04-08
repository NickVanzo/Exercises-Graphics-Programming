#include "MinecraftApplication.h"

#include <ituGL/asset/ShaderLoader.h>
#include <ituGL/asset/ModelLoader.h>
#include <ituGL/shader/Material.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <ituGL/geometry/VertexFormat.h>


MinecraftApplication::MinecraftApplication()
    : Application(1024, 1024, "Viewer demo")
    , m_cameraPosition(0, 30, 30)
    , m_gridY(16)
    , m_gridX(16)
    , m_cameraTranslationSpeed(20.0f)
    , m_cameraRotationSpeed(0.5f)
    , m_cameraEnabled(false)
    , m_cameraEnablePressed(false)
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
    CreateTerrainMesh(m_terrainPatch, m_gridX, m_gridY);
}

void MinecraftApplication::CreateTerrainMesh(Mesh& mesh, unsigned int gridX, unsigned int gridY)
{
    // Define the vertex structure
    struct Vertex
    {
        Vertex() = default;
        Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2 texCoord)
                : position(position), normal(normal), texCoord(texCoord) {}
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
    };

    // Define the vertex format (should match the vertex structure)
    VertexFormat vertexFormat;
    vertexFormat.AddVertexAttribute<float>(3);
    vertexFormat.AddVertexAttribute<float>(3);
    vertexFormat.AddVertexAttribute<float>(2);

    // List of vertices (VBO)
    std::vector<Vertex> vertices;

    // List of indices (EBO)
    std::vector<unsigned int> indices;

    // Grid scale to convert the entire grid to size 1x1
    glm::vec2 scale(1.0f / (gridX - 1), 1.0f / (gridY - 1));

    // Number of columns and rows
    unsigned int columnCount = gridX;
    unsigned int rowCount = gridY;

    // Iterate over each VERTEX
    for (unsigned int j = 0; j < rowCount; ++j)
    {
        for (unsigned int i = 0; i < columnCount; ++i)
        {
            // Vertex data for this vertex only
            glm::vec3 position(i * scale.x, 0.0f, j * scale.y);
            glm::vec3 normal(0.0f, 1.0f, 0.0f);
            glm::vec2 texCoord(i, j);
            vertices.emplace_back(position, normal, texCoord);
        }
    }

    mesh.AddSubmesh<Vertex, unsigned int, VertexFormat::LayoutIterator>(Drawcall::Primitive::Points, vertices, indices,
                                                                        vertexFormat.LayoutBegin(static_cast<int>(vertices.size()), true /* interleaved */), vertexFormat.LayoutEnd());
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

    ShaderUniformCollection::NameSet filteredUniforms;
    filteredUniforms.insert("WorldMatrix");
    filteredUniforms.insert("ViewProjMatrix");

    m_grassMaterial = std::make_shared<Material>(terrainShaderProgram, filteredUniforms);
    // Setup function
    ShaderProgram::Location worldMatrixLocation = terrainShaderProgram->GetUniformLocation("WorldMatrix");
    ShaderProgram::Location viewProjMatrixLocation = terrainShaderProgram->GetUniformLocation("ViewProjMatrix");
    m_grassMaterial->SetShaderSetupFunction([=](ShaderProgram& shaderProgram)
                                     {
                                         shaderProgram.SetUniform(worldMatrixLocation, glm::scale(glm::vec3(0.1f)));
                                         shaderProgram.SetUniform(viewProjMatrixLocation, glm::mat4(1));
                                     });
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

