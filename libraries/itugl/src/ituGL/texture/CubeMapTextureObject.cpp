#include <ituGL/texture/CubeMapTextureObject.h>

#include <cassert>

CubeMapTextureObject::CubeMapTextureObject()
{
}

template <>
void CubeMapTextureObject::SetImage<std::byte>(GLint level, GLsizei width, GLsizei height, Format format, InternalFormat internalFormat, std::span<const std::byte> data, Data::Type type)
{
    assert(IsBound());
    assert(data.empty() || type != Data::Type::None);
    assert(IsValidFormat(format, internalFormat));
    assert(data.empty() || data.size_bytes() == width * height * GetDataComponentCount(internalFormat) * Data::GetTypeSize(type));
    glTexImage2D(GetTarget(), level, internalFormat, width, height, 0, format, static_cast<GLenum>(type), data.data());
}

void CubeMapTextureObject::SetImage(GLint level, GLsizei width, GLsizei height, Format format, InternalFormat internalFormat)
{
    SetImage<float>(level, width, height, format, internalFormat, std::span<float>());
}
