#pragma once
#include <ituGL/texture/TextureObject.h>
#include <ituGL/core/Data.h>

class Texture3DObject: public TextureObjectBase<TextureObject::Texture3D>
{
public:
    Texture3DObject();
    /**
     * @param level is the level of details, 0 is for the base image level
     * @param width of the texture image
     * @param height of the texture image
     * @param format of the pixel data
     * @param internlFormat number of color components in the texture
     */
    void SetImage(GLint level,
                  GLsizei width,
                  GLsizei height,
                  GLsizei depth,
                  Format format,
                  InternalFormat internalFormat
                  );
    template <typename T>
    void SetImage(GLint level, GLsizei width, GLsizei height, GLsizei depth,
                  Format format, InternalFormat internalFormat,
                  std::span<const T> data, Data::Type type = Data::Type::None);
};

template <>
void Texture3DObject::SetImage<std::byte>(
        GLint level,
        GLsizei width,
        GLsizei height,
        GLsizei depth,
        Format format,
        InternalFormat internalFormat,
        std::span<const std::byte> data,
        Data::Type type
        );
template <typename T>
inline void Texture3DObject::SetImage(
        GLint level,
        GLsizei width,
        GLsizei height,
        GLsizei depth,
        Format format,
        InternalFormat internalFormat,
        std::span<const T> data,
        Data::Type type
        ) {
    if(type == Data::Type::None)
    {
        type = Data::GetType<T>();
    }
    SetImage(level, width, height, depth,format, internalFormat, Data::GetBytes(data), type);
}
