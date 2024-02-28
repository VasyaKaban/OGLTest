#pragma once

#include "../../sdk/glad/gl.h"
#include <filesystem>

struct MaterialTreeKey
{
	std::string material_lib_name;
	std::string material_name;

	MaterialTreeKey(std::string_view _material_lib_name = {}, std::string_view _material_name = {})
		: material_lib_name(_material_lib_name), material_name(_material_name) {}

	bool operator<(const MaterialTreeKey &key) const noexcept
	{
		if(material_lib_name == key.material_lib_name)
			return material_name < key.material_name;

		return material_lib_name < key.material_lib_name;
	}
};

class Material
{
public:
	Material() = default;
	~Material();
	Material(const Material &) = delete;
	Material(Material &&m) noexcept;
	Material & operator=(const Material &) = delete;
	Material & operator=(Material &&m) noexcept;

	void Create(const void *diffuse_texture_data,
				GLsizei diffuse_width,
				GLsizei diffuse_height,
				GLenum diffuse_format);

	void Destroy();

	void Bind(GLuint diffuse_bind_point) const noexcept;

private:
	GLuint diffuse_texture = 0;
};
