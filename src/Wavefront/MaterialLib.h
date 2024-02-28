#pragma once

#include <string>
#include <vector>

struct NewMaterial
{
	std::string name;
	std::string diffuse_map;

	NewMaterial(std::string_view _name = {}, std::string_view _diffuse_map = {})
		: name(_name), diffuse_map(_diffuse_map) {}
};

class MaterialLib
{
public:
	MaterialLib(std::string_view _name = {},
				std::vector<NewMaterial> &&_materials = {});
	~MaterialLib() = default;
	MaterialLib(const MaterialLib &) = default;
	MaterialLib(MaterialLib &&) = default;
	MaterialLib & operator=(const MaterialLib &) = default;
	MaterialLib & operator=(MaterialLib &&) = default;

	const std::string & GetName() const noexcept;
	const std::vector<NewMaterial> & GetMaterials() const noexcept;

private:
	std::string name;
	std::vector<NewMaterial> materials;
};
