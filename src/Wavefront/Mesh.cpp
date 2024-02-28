#include "Mesh.h"
#include <map>
#include <cassert>

Mesh::Mesh(std::vector<hrs::math::glsl::vec3> &&_vertices,
		   std::vector<hrs::math::glsl::vec2> &&_textures,
		   std::vector<hrs::math::glsl::vec3> &&_normals,
		   std::vector<Part> &&_parts,
		   std::string_view _material_lib) noexcept
	: vertices(std::move(_vertices)),
	  textures(std::move(_textures)),
	  normals(std::move(_normals)),
	  parts(std::move(_parts)),
	  material_lib(_material_lib) {}

const std::vector<hrs::math::glsl::vec3> & Mesh::GetVertices() const noexcept
{
	return vertices;
}

const std::vector<hrs::math::glsl::vec2> & Mesh::GetTextures() const noexcept
{
	return textures;
}

const std::vector<hrs::math::glsl::vec3> & Mesh::GetNormals() const noexcept
{
	return normals;
}

const std::vector<Part> & Mesh::GetParts() const noexcept
{
	return parts;
}

const std::string & Mesh::GetMaterialLib() const noexcept
{
	return material_lib;
}

namespace hrs
{
	namespace math
	{
		constexpr bool operator<(const hrs::math::glsl::ivec3 &v1, const hrs::math::glsl::ivec3 &v2) noexcept
		{
			if(v1[0] == v2[0])
			{
				if(v1[1] == v2[1])
					return v1[2] < v2[2];
				else
					return v1[1] < v2[1];
			}
			else
				return v1[0] < v2[0];
		}
	}
}

MeshVertexIndexData Mesh::CreateData() const
{
	MeshVertexIndexData out_data;
	out_data.part_indices.reserve(parts.size());
	std::size_t reserve_vert = 0;
	for(const auto &part : parts)
		reserve_vert += (part.surfaces.size() * 3);

	out_data.vertex_attributes.reserve((reserve_vert * 2) / 3);

	std::map<hrs::math::glsl::ivec3, std::size_t> surfaces_map;
	std::size_t index = 0;
	for(const auto &part : parts)
	{
		out_data.part_indices.push_back({});
		out_data.part_indices.back().material_lib_name = material_lib;
		out_data.part_indices.back().material_name = part.material_name;
		for(const auto &surf : part.surfaces)
		{
			for(const auto &surf_vertex : surf)
			{
				auto [it, inserted] = surfaces_map.insert({surf_vertex, 0});
				if(inserted)
				{
					it->second = index;
					index++;
					out_data.vertex_attributes.push_back({.vertex = vertices[surf_vertex[0] - 1],
														  .texture = textures[surf_vertex[1] - 1],
														  .normal = normals[surf_vertex[2] - 1]});
				}

				out_data.part_indices.back().indices.push_back(it->second);
			}
		}
	}

#ifndef NDEBUG
	for(std::size_t i = 0; i < parts.size(); i++)
		assert(parts[i].surfaces.size() * 3 == out_data.part_indices[i].indices.size());
#endif

	return out_data;
}
