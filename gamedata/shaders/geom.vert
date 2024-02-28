#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textures;
layout(location = 2) in vec3 normals;

layout(location = 0) out vec2 out_vert_texture_coords;

uniform mat4x4 perspective_matrix;
uniform mat4x4 view_matrix;
uniform mat4x4 model_matrix;

void main()
{
	gl_Position = perspective_matrix * view_matrix * model_matrix * vec4(position, 1.0);
	out_vert_texture_coords = textures;
}
