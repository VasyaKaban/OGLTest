#pragma once

#include "../../sdk/glad/gl.h"
#include <filesystem>

class Shader
{
public:
	Shader() = default;
	~Shader();
	Shader(const Shader &) = delete;
	Shader(Shader &&sh) noexcept;
	Shader & operator=(const Shader &) = delete;
	Shader & operator=(Shader &&sh) noexcept;

	GLint GetProgram() const noexcept;
	void Use() const noexcept;

	void Create(const std::filesystem::path &vertex_shader_path,
				const std::filesystem::path &fragment_shader_path);

private:

	GLint create_shader_unit(const std::filesystem::path &shader_path, GLenum type);
	GLint create_program(GLint vertex_shader, GLint fragment_shader);

private:
	GLint shader_program = 0;
};
