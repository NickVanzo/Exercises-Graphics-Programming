//
// Created by Nicolò Vanzo on 01/04/24.
//

#pragma once
#include <iostream>
class Texture2DObject;

class TextureLoader {
public:
    std::shared_ptr<Texture2DObject> LoadTexture(const char* path);
};


