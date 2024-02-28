#include "RenderableMesh.h"
#include <stdexcept>

RenderableMesh::~RenderableMesh()
{
	if(vao)
		glDeleteVertexArrays(1, &vao);

	if(ebo)
		glDeleteBuffers(1, &ebo);

	if(vertices_vbo)
		glDeleteBuffers(1, &vertices_vbo);

}

RenderableMesh::RenderableMesh(RenderableMesh &&rm) noexcept
	: vao(std::exchange(rm.vao, 0)),
	  vertices_vbo(std::exchange(rm.vertices_vbo, 0)),
	  ebo(std::exchange(rm.ebo, 0)),
	  parts(std::move(rm.parts)) {}

RenderableMesh & RenderableMesh::operator=(RenderableMesh &&rm) noexcept
{
	this->~RenderableMesh();

	vao = std::exchange(rm.vao, 0);
	vertices_vbo = std::exchange(rm.vertices_vbo, 0);
	ebo = std::exchange(rm.ebo, 0);
	parts = std::move(rm.parts);

	return *this;
}

void RenderableMesh::Create(const MeshVertexIndexData &data,
							const std::map<MaterialTreeKey, std::unique_ptr<Material>> &materials)
{
	GLuint _vao;
	GLuint _vbo;
	GLuint _ebo;
	std::vector<RenderablePart> _parts;
	_parts.reserve(data.part_indices.size());
	try
	{
		glCreateVertexArrays(1, &_vao);
		if(!_vao)
			throw std::runtime_error("VAO creation failied!");

		glCreateBuffers(1, &_vbo);
		if(!_vbo)
			throw std::runtime_error("VBO creation failied!");

		glCreateBuffers(1, &_ebo);
		if(!_ebo)
			throw std::runtime_error("EBO creation failied!");

		glNamedBufferData(_vbo,
						  data.vertex_attributes.size() * sizeof(MeshVertexAttribute),
						  data.vertex_attributes.data(), GL_STATIC_DRAW);

		GLsizei common_indices_size = 0;
		for(const auto &ind : data.part_indices)
			common_indices_size += ind.indices.size();

		common_indices_size *= 4;

		glNamedBufferData(_ebo,
						  common_indices_size,
						  nullptr, GL_STATIC_DRAW);

		GLsizei offset = 0;
		for(const auto &ind : data.part_indices)
		{
			_parts.push_back(RenderablePart{.count = static_cast<GLsizei>(ind.indices.size()),
											.offset = offset,
											.material = materials.find(MaterialTreeKey(ind.material_lib_name, ind.material_name))->second.get()});
			glNamedBufferSubData(_ebo, offset, ind.indices.size() * 4, ind.indices.data());
			offset += ind.indices.size() * 4;
		}

		glEnableVertexArrayAttrib(_vao, 0);//vertices
		glVertexArrayAttribBinding(_vao, 0, 0);
		glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

		glEnableVertexArrayAttrib(_vao, 1);//textures
		glVertexArrayAttribBinding(_vao, 1, 0);
		glVertexArrayAttribFormat(_vao, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));

		glEnableVertexArrayAttrib(_vao, 2);//normals
		glVertexArrayAttribBinding(_vao, 2, 0);
		glVertexArrayAttribFormat(_vao, 2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float));

		glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, 8 * sizeof(float));
		glVertexArrayElementBuffer(_vao, _ebo);
	}
	catch(std::runtime_error &err)
	{
		if(_ebo)
			glDeleteBuffers(1, &_ebo);

		if(_vbo)
			glDeleteBuffers(1, &_vbo);

		if(_vao)
			glDeleteVertexArrays(1, &_vao);

		throw err;

	}

	vao = _vao;
	vertices_vbo = _vbo;
	ebo = _ebo;
	parts = std::move(_parts);
}

void RenderableMesh::Bind() noexcept
{
	glBindVertexArray(vao);
}

const std::vector<RenderablePart> & RenderableMesh::GetParts() const noexcept
{
	return parts;
}

/*private:
GLint vao;
GLint vertices_vbo;
GLint ebo;
std::vector<RenderablePart> parts;
*/
