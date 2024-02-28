#include "Shader.h"
#include <utility>
#include <fstream>
#include <vector>

constexpr inline static std::size_t INFO_LOG_LENGTH = 1024;

Shader::~Shader()
{
	if(shader_program)
		glDeleteProgram(shader_program);
}

Shader::Shader(Shader &&sh) noexcept
	: shader_program(std::exchange(sh.shader_program, 0)) {}

Shader & Shader::operator=(Shader &&sh) noexcept
{
	if(shader_program)
		glDeleteProgram(shader_program);

	shader_program = std::exchange(sh.shader_program, 0);
	return *this;
}

void Shader::Create(const std::filesystem::path &vertex_shader_path,
					const std::filesystem::path &fragment_shader_path)
{
	GLint vertex_shader = 0;
	GLint fragment_shader = 0;
	GLint program = 0;
	try
	{
		vertex_shader = create_shader_unit(vertex_shader_path, GL_VERTEX_SHADER);
		fragment_shader = create_shader_unit(fragment_shader_path, GL_FRAGMENT_SHADER);
		program = create_program(vertex_shader, fragment_shader);

		shader_program = program;
	}
	catch(std::runtime_error &er)
	{
		if(program)
			glDeleteProgram(program);

		if(fragment_shader)
			glDeleteShader(fragment_shader);

		if(vertex_shader)
			glDeleteShader(vertex_shader);

		throw er;
	}
}

GLint Shader::GetProgram() const noexcept
{
	return shader_program;
}

void Shader::Use() const noexcept
{
	glUseProgram(shader_program);
}

GLint Shader::create_shader_unit(const std::filesystem::path &shader_path, GLenum type)
{
	std::fstream ifs;
	ifs.open(shader_path);
	if(!ifs.is_open())
		throw std::runtime_error("Bad file name!");

	std::vector<char> shader_source(std::filesystem::file_size(shader_path));
	ifs.read(shader_source.data(), shader_source.size());
	ifs.close();

	auto ptr = shader_source.data();
	GLint size = shader_source.size();

	auto shader = glCreateShader(type);
	if(shader == 0)
		throw std::runtime_error("Shader creation error");

	glShaderSource(shader, 1, &ptr, &size);
	glCompileShader(shader);

	GLint success = 0;
	char info[INFO_LOG_LENGTH];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(shader, INFO_LOG_LENGTH, nullptr, info);
		glDeleteShader(shader);
		throw std::runtime_error(info);
	}

	return shader;

}

GLint Shader::create_program(GLint vertex_shader, GLint fragment_shader)
{
	auto program = glCreateProgram();
	if(program == 0)
		throw std::runtime_error("Program creation error");
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	GLint success = 0;
	char info[INFO_LOG_LENGTH];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(program, INFO_LOG_LENGTH, nullptr, info);
		glDeleteProgram(program);
		throw std::runtime_error(info);
	}

	return program;
}
