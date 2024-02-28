#include "MaterialLib.h"

MaterialLib::MaterialLib(std::string_view _name,
						 std::vector<NewMaterial> &&_materials)
	: name(_name), materials(std::move(_materials)) {}


const std::string & MaterialLib::GetName() const noexcept
{
	return name;
}

const std::vector<NewMaterial> & MaterialLib::GetMaterials() const noexcept
{
	return materials;
}
