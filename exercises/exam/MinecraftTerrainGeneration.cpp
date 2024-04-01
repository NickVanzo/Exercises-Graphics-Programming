//
// Created by Nicolò Vanzo on 31/03/24.
//

#include "MinecraftTerrainGeneration.h"
#include "TextureLoader.h"
MinecraftTerrainGeneration::MinecraftTerrainGeneration()
: Application(1024, 1024, "Exam project Minecraft terrain"),
    m_gridX(128),
    m_gridY(128)
{}

void MinecraftTerrainGeneration::Initialize() {
    Application::Initialize();
}
void MinecraftTerrainGeneration::InitialiseTextures() {
    std::shared_ptr<TextureLoader> textureLoader = std::make_shared<TextureLoader>();
    m_stoneTexture = textureLoader.get()->LoadTexture("resources/stone_texture.png");
}
void MinecraftTerrainGeneration::Update() {

}
void MinecraftTerrainGeneration::Render() {}
void MinecraftTerrainGeneration::Cleanup() {}