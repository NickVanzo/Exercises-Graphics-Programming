//
// Created by Nicolò Vanzo on 31/03/24.
//

#pragma once
#include <ituGL/application/Application.h>
#include <ituGL/camera/Camera.h>
#include <ituGL/geometry/Mesh.h>

class Texture2DObject;

class MinecraftTerrainGeneration : public Application {
public:
    MinecraftTerrainGeneration();
protected:
    void Initialize() override;
    void Update() override;
    void Render() override;
    void Cleanup() override;
    std::shared_ptr<Texture2DObject> CreateHeightMap(unsigned int width, unsigned int height, unsigned int depth)
private:
    unsigned int m_gridX, m_gridY;
    Camera m_camera;
    Mesh m_terrainPatch;

};
