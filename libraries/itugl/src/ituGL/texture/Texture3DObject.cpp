//
// Created by Nicolò Vanzo on 01/04/24.
//
#include <ituGL/texture/Texture3DObject.h>
#include <cassert>

Texture3DObject::Texture3DObject() {

}

template<>
void Texture3DObject::SetImage(GLint level, GLsizei width, GLsizei height, GLsizei depth, TextureObject::Format format,
                               InternalFormat internalFormat, std::span<const std::byte> data, Data::Type type) {
    //Is the texture bound?
    assert(IsBound());
    assert(data.empty() || type != Data::Type::None);
    assert(IsValidFormat(format, internalFormat));
    assert(data.empty() || data.size_bytes() == width * height * GetDataComponentCount(internalFormat) * Data::GetTypeSize(type));
    glTexImage3D(
            GetTarget(),
            level,
            internalFormat,
            width,
            height,
            depth,
            0,
            format,
            static_cast<GLenum>(type),
            data.data()
            );
}

void Texture3DObject::SetImage(GLint level, GLsizei width, GLsizei height, GLsizei depth, Format format,
                               TextureObject::InternalFormat internalFormat) {
    SetImage<float>(level, width, height, depth,format, internalFormat, std::span<float>());
}