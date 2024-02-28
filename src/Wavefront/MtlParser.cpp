#include "MtlParser.h"
#include "Common.hpp"

MtlParser::~MtlParser()
{
	fs.close();
}

MaterialLib MtlParser::Parse(const std::filesystem::path &file_name, std::string_view material_lib_name)
{
	fs.open(file_name);
	if(!fs.is_open())
		throw MtlParserError(MtlParserResult::BadFile, 0);

	std::vector<NewMaterial> materials;
	materials.reserve(4);

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

		if(trimmed_line.starts_with("newmtl "))
			materials.push_back(NewMaterial{parse_new_material({trimmed_line.begin() + 7, trimmed_line.end()}, col)});
		else if(trimmed_line.starts_with("map_Kd "))
		{
			if(materials.empty())
				throw MtlParserError(MtlParserResult::BadNewMaterial, col);

			materials.back().diffuse_map = parse_diffuse_map({trimmed_line.begin() + 7, trimmed_line.end()}, col);
		}

		col++;
	}

	return MaterialLib(material_lib_name, std::move(materials));
}

std::string_view MtlParser::parse_new_material(std::string_view str, std::size_t col)
{
	str = trim_spaces_back(trim_spaces_front(str));
	if(str.empty())
		throw MtlParserError(MtlParserResult::BadNewMaterial, col);

	return str;
}

std::string_view MtlParser::parse_diffuse_map(std::string_view str, std::size_t col)
{
	str = trim_spaces_back(trim_spaces_front(str));
	if(str.empty())
		throw MtlParserError(MtlParserResult::BadDiffuseMap, col);

	return str;
}
