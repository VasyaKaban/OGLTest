#include "ObjParser.h"
#include "Common.hpp"
#include <charconv>
#include <array>
#include <ranges>

ObjParser::~ObjParser()
{
	fs.close();
}

Mesh ObjParser::Parse(std::filesystem::path file_name)
{
	fs.open(file_name);
	if(!fs.is_open())
		throw ObjParserError(ObjParserResult::BadFile, 0);

	std::vector<hrs::math::glsl::vec3> vertices;
	vertices.reserve(64);
	std::vector<hrs::math::glsl::vec2> textures;
	textures.reserve(64);
	std::vector<hrs::math::glsl::vec3> normals;
	normals.reserve(64);
	std::vector<Part> parts;
	parts.reserve(8);
	std::string material_lib;

	std::string str;
	std::size_t col = 1;
	str.reserve(512);
	while(!fs.eof())
	{
		std::getline(fs, str);
		if(fs.eof())
			break;

		auto trimmed_line = trim_spaces_front(str);
		if(trimmed_line.empty())
			continue;

		if(trimmed_line.starts_with("v "))
			vertices.push_back(parse_vertex({trimmed_line.begin() + 2, trimmed_line.end()}, col));
		else if(trimmed_line.starts_with("vt "))
			textures.push_back(parse_texture({trimmed_line.begin() + 3, trimmed_line.end()}, col));
		else if(trimmed_line.starts_with("vn "))
			normals.push_back(parse_normal({trimmed_line.begin() + 3, trimmed_line.end()}, col));
		else if(trimmed_line.starts_with("f "))
		{
			if(parts.empty())
				throw ObjParserError(ObjParserResult::BadGroup, col);

			parts.back().surfaces.push_back(parse_surface({trimmed_line.begin() + 2, trimmed_line.end()}, col));
		}
		else if(trimmed_line.starts_with("g "))
		{
			auto part_name = parse_group({trimmed_line.begin() + 2, trimmed_line.end()}, col);
			parts.push_back(Part{{part_name.begin(), part_name.end()}, "", {}});

			if(parts.size() != 1)
				parts.back().material_name = std::prev(parts.end(), 2)->material_name;
		}
		else if(trimmed_line.starts_with("usemtl "))
		{
			auto mtl_name = parse_material({trimmed_line.begin() + 7, trimmed_line.end()}, col);
			if(parts.empty())
				throw ObjParserError(ObjParserResult::BadGroup, col);

			parts.back().material_name = mtl_name;
		}
		else if(trimmed_line.starts_with("mtllib "))
			material_lib = parse_material_lib({trimmed_line.begin() + 7, trimmed_line.end()}, col);

		col++;
	}

	return Mesh(std::move(vertices), std::move(textures), std::move(normals), std::move(parts), material_lib);
}


hrs::math::glsl::vec3 ObjParser::parse_vertex(std::string_view str, std::size_t col)
{
	hrs::math::glsl::vec3 out_vec;
	for(std::size_t i = 0; i < 3; i++)
	{
		str = trim_spaces_front(str);
		if(str.empty())
			throw ObjParserError(ObjParserResult::BadVertex, col);

		std::string_view::size_type space_it;
		if(i == 2)
			space_it = str.size() - 1;
		else
			space_it = str.find(' ');
		if(space_it == std::string_view::npos)
			throw ObjParserError(ObjParserResult::BadVertex, col);

		float val = 0;
		auto [ptr, err] = std::from_chars(str.begin(), str.begin() + space_it, val);
		if((err != std::errc(0)) || (ptr != str.begin() + space_it))
			throw ObjParserError(ObjParserResult::BadVertex, col);

		out_vec[i] = val;
		str = std::string_view(str.begin() + space_it, str.end());
	}

	return out_vec;
}

hrs::math::glsl::vec2 ObjParser::parse_texture(std::string_view str, std::size_t col)
{
	hrs::math::glsl::vec2 out_vec;
	for(std::size_t i = 0; i < 2; i++)
	{
		str = trim_spaces_front(str);
		//if(str.empty())
		//	throw ParserError(ParserResult::BadTexture, col);

		std::string_view::size_type space_it;
		space_it = str.find(' ');
		if(space_it == std::string_view::npos)
		{
			if(i == 0)
				throw ObjParserError(ObjParserResult::BadTexture, col);
			else if(str.empty())
			{
				out_vec[i] = 0;
				break;
			}
			else
				space_it = str.size();
		}

		float val = 0;
		auto [ptr, err] = std::from_chars(str.begin(), str.begin() + space_it, val);
		if((err != std::errc(0)) || (ptr != str.begin() + space_it))
			throw ObjParserError(ObjParserResult::BadTexture, col);

		out_vec[i] = val;
		str = std::string_view(str.begin() + space_it, str.end());
	}

	return out_vec;
}

hrs::math::glsl::vec3 ObjParser::parse_normal(std::string_view str, std::size_t col)
{
	hrs::math::glsl::vec3 out_vec;
	for(std::size_t i = 0; i < 3; i++)
	{
		str = trim_spaces_front(str);
		if(str.empty())
			throw ObjParserError(ObjParserResult::BadNormal, col);

		std::string_view::size_type space_it;
		if(i == 2)
			space_it = str.size() - 1;
		else
			space_it = str.find(' ');
		if(space_it == std::string_view::npos)
			throw ObjParserError(ObjParserResult::BadNormal, col);

		float val = 0;
		auto [ptr, err] = std::from_chars(str.begin(), str.begin() + space_it, val);
		if((err != std::errc(0)) || (ptr != str.begin() + space_it))
			throw ObjParserError(ObjParserResult::BadNormal, col);

		out_vec[i] = val;
		str = std::string_view(str.begin() + space_it, str.end());
	}

	return out_vec;
}

#include <iostream>

std::array<hrs::math::glsl::ivec3, 3> ObjParser::parse_surface(std::string_view str, std::size_t col)
{
	//assume that we work with triangles!
	std::array<hrs::math::glsl::ivec3, 3> out_surfaces;
	for(std::size_t i = 0; i < 3; i++)
	{
		str = trim_spaces_front(str);
		if(str.empty())
			throw ObjParserError(ObjParserResult::BadSurface, col);

		std::string_view::size_type space_it;
		if(i == 2)
			space_it = str.size() - 1;
		else
			space_it = str.find(' ');
		if(space_it == std::string_view::npos)
			throw ObjParserError(ObjParserResult::BadSurface, col);

		std::string_view surface_str(str.begin(), str.begin() + space_it);
		hrs::math::glsl::ivec3 surface_vec;
		std::size_t j = 0;
		for(auto sub_str : std::ranges::split_view(surface_str, '/'))
		{
			if(j >= 3)
				break;

			std::size_t sub_str_size = sub_str.size();
			if(sub_str.size() == 0)
				sub_str_size = 1;

			std::string_view sub_str_view(sub_str.begin(), sub_str.begin() + sub_str_size);

			int val = 0;
			auto [ptr, err] = std::from_chars(sub_str_view.begin(), sub_str_view.end(), val);
			if((err != std::errc(0)) || (ptr != sub_str_view.end()))
				throw ObjParserError(ObjParserResult::BadSurface, col);

			surface_vec[j] = val;
			j++;
		}

		out_surfaces[i] = surface_vec;
		str = std::string_view(str.begin() + space_it, str.end());
	}

	return out_surfaces;
}

std::string_view ObjParser::parse_group(std::string_view str, std::size_t col)
{
	str = trim_spaces_back(trim_spaces_front(str));
	if(str.empty())
		throw ObjParserError(ObjParserResult::BadGroup, col);

	return str;
}

std::string_view ObjParser::parse_material(std::string_view str, std::size_t col)
{
	str = trim_spaces_back(trim_spaces_front(str));
	if(str.empty())
		throw ObjParserError(ObjParserResult::BadMaterial, col);

	return str;
}

std::string_view ObjParser::parse_material_lib(std::string_view str, std::size_t col)
{
	str = trim_spaces_back(trim_spaces_front(str));
	if(str.empty())
		throw ObjParserError(ObjParserResult::BadMaterialLib, col);

	return str;
}
