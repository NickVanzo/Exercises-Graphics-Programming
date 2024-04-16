//
// Created by Nicolò Vanzo on 15/04/24.
//

#pragma once
#include <ituGL/asset/TextureLoader.h>
#include <ituGL/texture/Texture2DArrayObject.h>

class Texture2DArrayLoader : public TextureLoader<Texture2DArrayObject> {
public:
    Texture2DArrayLoader();
    Texture2DArrayLoader(TextureObject::Format format, TextureObject::InternalFormat internalFormat);

    //Override required by the AssetLoader
    Texture2DArrayObject Load(const char* path) override;
    Texture2DArrayObject Load(std::span<const char*> paths);

    static std::shared_ptr<Texture2DArrayObject> LoadTextureShared(std::span<const char*> paths,
                                                                   TextureObject::Format format, TextureObject::InternalFormat internalFormat, bool generateMipmap = true);

    inline bool GetFlipVertical() const { return m_flipVertical; }
    inline void SetFlipVertical(bool flipVertical) { m_flipVertical = flipVertical; }
private:
    // If true, the texture will be flipped vertically on load
    // This option exists because some systems define the vertical origin as "up", and others as "down"
    bool m_flipVertical;
};


