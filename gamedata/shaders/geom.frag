#version 450

layout(location = 0) in vec2 out_vert_texture_coords;

layout(location = 0) out vec4 out_frag_color;

layout(location = 0) uniform sampler2D diffuse_map;

void main()
{
	out_frag_color = texture(diffuse_map, out_vert_texture_coords);/*vec4(vec3(gl_FragCoord.z), 1.0);*/
}
