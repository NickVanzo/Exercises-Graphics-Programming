//
// Created by Nicolò Vanzo on 01/04/24.
//
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <ituGL/texture/Texture2DObject.h>
#include "TextureLoader.h"

std::shared_ptr<Texture2DObject> TextureLoader::LoadTexture(const char* path) {
    std::shared_ptr<Texture2DObject> texture = std::make_shared<Texture2DObject>();

    int width = 0;
    int height = 0;
    int components = 0;

    unsigned char* data = stbi_load(
            path,
            &width,
            &height,
            &components,
            4
            );

    texture->Bind();
    texture->SetImage(
            0,
            width,
            height,
            TextureObject::FormatRGBA,
            TextureObject::InternalFormatRGBA,
            std::span<const unsigned char>(data, width * height * 4)
            );

    texture->GenerateMipmap();
    stbi_image_free(data);

    return texture;
}