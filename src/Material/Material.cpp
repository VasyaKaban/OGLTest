#include "Material.h"
#include <utility>
#include <stdexcept>

Material::~Material()
{
	Destroy();
}

Material::Material(Material &&m) noexcept
	: diffuse_texture(std::exchange(m.diffuse_texture, 0)) {}

Material & Material::operator=(Material &&m) noexcept
{
	Destroy();

	diffuse_texture = std::exchange(m.diffuse_texture, 0);

	return *this;
}

void Material::Create(const void *diffuse_texture_data,
					  GLsizei diffuse_width,
					  GLsizei diffuse_height,
					  GLenum diffuse_format)
{
	GLuint _diffuse_texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &_diffuse_texture);
	if(!_diffuse_texture)
		throw std::runtime_error("Diffuse texture creation failure");

	glTextureParameteri(_diffuse_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(_diffuse_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(_diffuse_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(_diffuse_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTextureStorage2D(_diffuse_texture, 1, GL_RGBA8, diffuse_height, diffuse_height);
	glTextureSubImage2D(_diffuse_texture, 0, 0, 0,
						diffuse_width, diffuse_height, diffuse_format, GL_UNSIGNED_BYTE, diffuse_texture_data);

	glGenerateTextureMipmap(_diffuse_texture);

	diffuse_texture = _diffuse_texture;
}

void Material::Destroy()
{
	if(diffuse_texture)
		glDeleteTextures(1, &diffuse_texture);
}

void Material::Bind(GLuint diffuse_bind_point) const noexcept
{
	glBindTextureUnit(diffuse_bind_point, diffuse_texture);
}
