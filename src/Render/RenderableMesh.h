#pragma once

#include "../../sdk/glad/gl.h"
#include "../Wavefront/Mesh.h"
#include "../Material/Material.h"
#include <vector>
#include <map>

struct RenderablePart
{
	GLsizei count;
	GLsizei offset;
	const Material *material;
};

class RenderableMesh
{
public:
	RenderableMesh() = default;
	~RenderableMesh();
	RenderableMesh(const RenderableMesh &) = delete;
	RenderableMesh(RenderableMesh &&rm) noexcept;
	RenderableMesh & operator=(const RenderableMesh &) = delete;
	RenderableMesh & operator=(RenderableMesh &&rm) noexcept;

	void Create(const MeshVertexIndexData &data, const std::map<MaterialTreeKey, std::unique_ptr<Material>> &materials);

	void Bind() noexcept;

	const std::vector<RenderablePart> & GetParts() const noexcept;

private:
	GLuint vao;
	GLuint vertices_vbo;
	GLuint ebo;
	std::vector<RenderablePart> parts;
};
