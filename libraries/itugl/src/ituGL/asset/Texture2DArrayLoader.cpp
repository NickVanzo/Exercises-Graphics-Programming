
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <ituGL/asset/Texture2DArrayLoader.h>

Texture2DArrayLoader::Texture2DArrayLoader(): m_flipVertical(false) {}

Texture2DArrayLoader::Texture2DArrayLoader(TextureObject::Format format, TextureObject::InternalFormat internalFormat)
        : TextureLoader(format, internalFormat)
        , m_flipVertical(false)
{
}

Texture2DArrayObject Texture2DArrayLoader::Load(const char *path) {
    throw std::logic_error("Load(const char* path) not implemented in derived class");
}

Texture2DArrayObject Texture2DArrayLoader::Load(std::span<const char*> paths) {
    Texture2DArrayObject textureArray;
    unsigned char* totalData = nullptr;
    stbi_set_flip_vertically_on_load(m_flipVertical ? 1 : 0);
    int componentCount = TextureObject::GetComponentCount(m_format);
    int totalDataSize = 0;
    int currentOffset = 0;
    int width, height, originalComponentCount;

    // Do I need to bind the textureArray before entering the loop?
    textureArray.Bind();

    for(const char* path : paths) {
        unsigned char* data = stbi_load(path, &width, &height, &originalComponentCount, componentCount);
        assert(data);
        if(data) {
            int dataSize = width * height * componentCount;
            totalData = static_cast<unsigned char*>(std::realloc(totalData, totalDataSize + dataSize));
            std::memcpy(totalData + currentOffset, data, dataSize);
            currentOffset += dataSize;
            totalDataSize += dataSize;
            stbi_image_free(data);
        }
    }

    if(m_generateMipmap) {
        textureArray.GenerateMipmap();
    } else {
        textureArray.SetParameter(TextureObject::ParameterEnum::MinFilter, GL_LINEAR);
    }

    textureArray.SetImage<unsigned char>(0, width, height, paths.size(), m_format, m_internalFormat, std::span(totalData, totalDataSize));

    // Do I need to unbind the textureArray at the end? Or at every loop?
    textureArray.Unbind();

    return textureArray;
}

