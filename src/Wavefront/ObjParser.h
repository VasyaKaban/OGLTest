#pragma once

#include <fstream>
#include <filesystem>
#include "../hrs/math/vector.hpp"
#include "Mesh.h"

enum class ObjParserResult
{
	BadFile,
	BadVertex,
	BadTexture,
	BadNormal,
	BadSurface,
	BadGroup,
	BadMaterial,
	BadMaterialLib
};

constexpr auto ObjParserResultToString(ObjParserResult res) noexcept
{
	switch(res)
	{
		case ObjParserResult::BadFile:
			return "BadFile";
			break;
		case ObjParserResult::BadVertex:
			return "BadVertex";
			break;
		case ObjParserResult::BadTexture:
			return "BadTexture";
			break;
		case ObjParserResult::BadNormal:
			return "BadNormal";
			break;
		case ObjParserResult::BadSurface:
			return "BadSurface";
			break;
		case ObjParserResult::BadGroup:
			return "BadGroup";
			break;
		case ObjParserResult::BadMaterial:
			return "BadMaterial";
			break;
		case ObjParserResult::BadMaterialLib:
			return "BadMaterialLib";
			break;
	}
}

struct ObjParserError
{
	ObjParserResult result;
	std::size_t col;

	constexpr ObjParserError(ObjParserResult _result, std::size_t _col) noexcept
		: result(_result), col(_col) {}
};

class ObjParser
{
public:
	ObjParser() = default;
	~ObjParser();
	ObjParser(const ObjParser &) = delete;
	ObjParser(ObjParser &&p) = default;
	ObjParser & operator=(const ObjParser &) = delete;
	ObjParser & operator=(ObjParser &&p) = default;

	Mesh Parse(std::filesystem::path file_name);

private:

	hrs::math::glsl::vec3 parse_vertex(std::string_view str, std::size_t col);
	hrs::math::glsl::vec2 parse_texture(std::string_view str, std::size_t col);
	hrs::math::glsl::vec3 parse_normal(std::string_view str, std::size_t col);
	std::array<hrs::math::glsl::ivec3, 3> parse_surface(std::string_view str, std::size_t col);
	std::string_view parse_group(std::string_view str, std::size_t col);
	std::string_view parse_material(std::string_view str, std::size_t col);
	std::string_view parse_material_lib(std::string_view str, std::size_t col);

private:
	std::fstream fs;
};
