#pragma once

#include <vector>
#include <array>
#include <string>
#include "../hrs/math/vector.hpp"

struct MeshVertexAttribute
{
	hrs::math::glsl::vec3 vertex;
	hrs::math::glsl::vec2 texture;
	hrs::math::glsl::vec3 normal;
};

struct PartIndexData
{
	std::string material_lib_name;
	std::string material_name;
	std::vector<std::uint32_t> indices;
};

struct MeshVertexIndexData
{
	std::vector<MeshVertexAttribute> vertex_attributes;
	std::vector<PartIndexData> part_indices;
};

struct Part
{
	std::string name;
	std::string material_name;
	std::vector<std::array<hrs::math::glsl::ivec3, 3>> surfaces;
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(std::vector<hrs::math::glsl::vec3> &&_vertices = {},
		 std::vector<hrs::math::glsl::vec2> &&_textures = {},
		 std::vector<hrs::math::glsl::vec3> &&_normals = {},
		 std::vector<Part> &&_parts = {},
		 std::string_view _material_lib = {}) noexcept;

	~Mesh() = default;
	Mesh(const Mesh &) = default;
	Mesh(Mesh &&) = default;
	Mesh & operator=(const Mesh &) = default;
	Mesh & operator=(Mesh &&) = default;

	const std::vector<hrs::math::glsl::vec3> & GetVertices() const noexcept;
	const std::vector<hrs::math::glsl::vec2> & GetTextures() const noexcept;
	const std::vector<hrs::math::glsl::vec3> & GetNormals() const noexcept;
	const std::vector<Part> & GetParts() const noexcept;
	const std::string & GetMaterialLib() const noexcept;

	MeshVertexIndexData CreateData() const;

private:
	std::vector<hrs::math::glsl::vec3> vertices;
	std::vector<hrs::math::glsl::vec2> textures;
	std::vector<hrs::math::glsl::vec3> normals;
	std::vector<Part> parts;
	std::string material_lib;
};
