//
// Created by Nicolò Vanzo on 15/04/24.
//

#pragma once

#include <ituGL/texture/TextureObject.h>
#include <ituGL/core/Data.h>

class Texture2DArrayObject : public TextureObjectBase<TextureObject::Texture2DArray>
{
public:
    Texture2DArrayObject();

    void SetImage(GLint level, GLsizei width, GLsizei depth,GLsizei height, Format format, InternalFormat internalFormat);

    template <typename T>
    void SetImage(GLint level, GLsizei width, GLsizei depth,GLsizei height, Format format, InternalFormat internalFormat, std::span<const T> data,
                  Data::Type type = Data::Type::None);
};

template <>
void Texture2DArrayObject::SetImage<std::byte>(GLint level, GLsizei width, GLsizei depth, GLsizei height, Format format, InternalFormat internalFormat, std::span<const std::byte> data, Data::Type type);

// Template method to set image with any kind of data
template <typename T>
inline void Texture2DArrayObject::SetImage(GLint level, GLsizei width, GLsizei height, GLsizei depth,
                                      Format format, InternalFormat internalFormat, std::span<const T> data, Data::Type type)
{
    if (type == Data::Type::None)
    {
        type = Data::GetType<T>();
    }
    SetImage(level, width, height , depth, format, internalFormat, Data::GetBytes(data), type);
}
