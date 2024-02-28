 #pragma once

#include <fstream>
#include <filesystem>
#include "MaterialLib.h"

enum class MtlParserResult
{
	BadFile,
	BadNewMaterial,
	BadDiffuseMap
};

constexpr auto MtlParserResultToString(MtlParserResult res) noexcept
{
	switch(res)
	{
		case MtlParserResult::BadFile:
			return "BadFile";
			break;
		case MtlParserResult::BadNewMaterial:
			return "BadNewMaterial";
			break;
		case MtlParserResult::BadDiffuseMap:
			return "BadDiffuseMap";
			break;
	}
}

struct MtlParserError
{
	MtlParserResult result;
	std::size_t col;

	constexpr MtlParserError(MtlParserResult _result, std::size_t _col) noexcept
		: result(_result), col(_col) {}
};

class MtlParser
{
public:
	MtlParser() = default;
	~MtlParser();
	MtlParser(const MtlParser &) = delete;
	MtlParser(MtlParser &&p) = default;
	MtlParser & operator=(const MtlParser &) = delete;
	MtlParser & operator=(MtlParser &&p) = default;

	MaterialLib Parse(const std::filesystem::path &file_name, std::string_view material_lib_name);

private:

	std::string_view parse_new_material(std::string_view str, std::size_t col);
	std::string_view parse_diffuse_map(std::string_view str, std::size_t col);

private:
	std::fstream fs;
};
