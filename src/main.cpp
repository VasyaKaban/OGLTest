#include "Wavefront/ObjParser.h"
#include "Wavefront/MtlParser.h"
#include <iostream>
#include <format>
#include <map>

#include "Shader/Shader.h"
#include "Render/RenderableMesh.h"
#include "hrs/math/matrix.hpp"
#include "hrs/math/vector.hpp"
#include "hrs/math/quaternion.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define GLAD_GL_IMPLEMENTATION
#include "../sdk/glad/gl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../sdk/stb_image/stb_image.h"

bool is_run = true;
constexpr inline static float NEAR = 0.01f;
constexpr inline static float FAR = 1000.0f;
constexpr inline static float FOV = 60.0f;

auto view_rotate = hrs::math::glsl::std430::mat4x4::identity();
auto view_translate = hrs::math::glsl::std430::mat4x4::identity();
hrs::math::glsl::std430::mat4x4 projection_mat;

GLenum polygon_mode = GL_LINE;

float deg_to_rad(float deg)
{
	return deg * std::numbers::pi_v<float> / 180;
}

float GetAspect(SDL_Window *window) noexcept
{
	int w, h;
	SDL_GL_GetDrawableSize(window, &w, &h);
	return static_cast<float>(w) / h;
}

hrs::math::glsl::std430::mat4x4 Perspective(float fov, float aspect, float near, float far) noexcept
{
	float half_fov_tan = std::tan(fov / 2);
	float top = near * half_fov_tan;
	float right = top * aspect;

	hrs::math::glsl::std430::mat4x4 out_mat;
	out_mat[0][0] = near / right;
	out_mat[1][1] = near / top;
	out_mat[2][2] = -(far + near) / (near - far);
	out_mat[2][3] = 1;
	out_mat[3][2] = (2 * far * near) / (near - far);

	return out_mat;
}

void SDLEventPoll(SDL_Window *window)
{
	static bool is_camera_active = false;
	static int x_start = 0;
	static int y_start = 0;
	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		switch(ev.type)
		{
			case SDL_QUIT:
				is_run = false;
				break;
			case SDL_KEYDOWN:
				switch(ev.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						if(!is_camera_active)
						{
							SDL_GetMouseState(&x_start, &y_start);
							SDL_ShowCursor(SDL_DISABLE);
						}
						else
							SDL_ShowCursor(SDL_ENABLE);

						is_camera_active = !is_camera_active;
						break;
					case SDLK_q:
						is_run = false;
						break;
					case SDLK_f:
						polygon_mode = (polygon_mode == GL_LINE ? GL_FILL : GL_LINE);
						break;
				}
				break;
			case SDL_MOUSEMOTION:
				{
					if(is_camera_active)
					{
						static float x_angle = 0;
						static float y_angle = 0;
						float dtx = x_start - ev.motion.x;
						float dty = y_start - ev.motion.y;
						x_angle += static_cast<float>(dtx) * 0.05f;
						y_angle += static_cast<float>(dty) * 0.05f;
						hrs::math::glsl::vec3 x_axis(1, 0, 0);
						hrs::math::quaternion<float> q_x(x_axis, deg_to_rad(y_angle));
						auto rotate_matrix_x = q_x.to_matrix();
						//const auto &y_axis = rotate_matrix_x[1];
						hrs::math::glsl::vec3 y_axis(0, 1, 0);
						hrs::math::quaternion<float> q_y(y_axis, deg_to_rad(x_angle));
						view_rotate = rotate_matrix_x * q_y.to_matrix();

						SDL_WarpMouseInWindow(window, x_start, y_start);
					}
				}
				break;
			case SDL_WINDOWEVENT:
				switch(ev.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
						projection_mat = Perspective(deg_to_rad(FOV),
													 static_cast<float>(ev.window.data1) / ev.window.data2,
													 NEAR,
													 FAR);
						glViewport(0, 0, ev.window.data1, ev.window.data2);
						break;
				}

				break;

		}
	}
}

void HandleMovement()
{
	auto state = SDL_GetKeyboardState(nullptr);
	if(state[SDL_GetScancodeFromKey(SDLK_w)])
		view_translate[3] -= view_rotate[2] * 0.1f;

	if(state[SDL_GetScancodeFromKey(SDLK_s)])
		view_translate[3] += view_rotate[2] * 0.1f;

	if(state[SDL_GetScancodeFromKey(SDLK_d)])
		view_translate[3] -= view_rotate[0] * 0.1f;

	if(state[SDL_GetScancodeFromKey(SDLK_a)])
		view_translate[3] += view_rotate[0] * 0.1f;

	if(state[SDL_GetScancodeFromKey(SDLK_u)])
		view_translate[3] -= view_rotate[1] * 0.1f;

	if(state[SDL_GetScancodeFromKey(SDLK_b)])
		view_translate[3] += view_rotate[1] * 0.1f;
}

hrs::math::glsl::std430::mat4x4 Translate(float x = 0, float y = 0, float z = 0) noexcept
{
	auto out_mat = hrs::math::glsl::std430::mat4x4::identity();
	out_mat[3][0] = x;
	out_mat[3][1] = y;
	out_mat[3][2] = z;

	return out_mat;
}

void AppendMaterialLib(const MaterialLib &lib, std::map<MaterialTreeKey, std::unique_ptr<Material>> &materials)
{
	for(const auto &mtl : lib.GetMaterials())
	{
		MaterialTreeKey key(lib.GetName(), mtl.name);
		auto it = materials.find(key);
		if(it != materials.end())
			continue;

		std::filesystem::path diffuse_path = "../../gamedata/textures/" + mtl.diffuse_map;

		int diffuse_width, diffuse_height, diffuse_channels;
		auto diffuse_format = GL_RGB;
		auto diffuse_data = stbi_load(diffuse_path.c_str(), &diffuse_width, &diffuse_height, &diffuse_channels, 0);
		if(!diffuse_data)
			throw std::runtime_error("Bad texture!");


		std::cout<<"Channels: "<<diffuse_channels<<std::endl;
		//read!!!

		//const void *data = nullptr;
		//int width, height, format;
		std::unique_ptr<Material> u_material(new Material);
		u_material->Create(diffuse_data, diffuse_width, diffuse_height, diffuse_format);
		materials.insert({key, std::move(u_material)});

		stbi_image_free(diffuse_data);
	}
}

int main()
{
	stbi_set_flip_vertically_on_load(true);

	auto init_res = SDL_Init(SDL_INIT_EVERYTHING);
	if(init_res)
	{
		std::cout<<SDL_GetError()<<std::endl;
		exit(-1);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window *window = SDL_CreateWindow("title",
										  SDL_WINDOWPOS_UNDEFINED,
										  SDL_WINDOWPOS_UNDEFINED,
										  800,
										  600,
										  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if(!window)
	{
		std::cout<<SDL_GetError()<<std::endl;
		exit(-1);
	}

	//SDL_SetRelativeMouseMode(SDL_TRUE);

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);

	int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
	std::cout<<version<<std::endl;
	if(version == 0)
	{
		std::cout<<"OpenGL loading failure!"<<std::endl;
		exit(-1);
	}

	ObjParser obj_parser;
	MtlParser mtl_parser;
	MeshVertexIndexData mesh_data;
	std::map<MaterialTreeKey, std::unique_ptr<Material>> materials;

	try
	{
		auto mesh = obj_parser.Parse("../../gamedata/objects/stk.obj");
		auto material_lib = mtl_parser.Parse("../../gamedata/materials/" + mesh.GetMaterialLib(), mesh.GetMaterialLib());

		AppendMaterialLib(material_lib, materials);

		std::cout<<"Vertices:\n";
		for(const auto &vert : mesh.GetVertices())
			std::cout<<"("<<vert[0]<<", "<<vert[1]<<", "<<vert[2]<<")"<<std::endl;

		std::cout<<"Textures:\n";
		for(const auto &text : mesh.GetTextures())
			std::cout<<"("<<text[0]<<", "<<text[1]<<")"<<std::endl;

		std::cout<<"Normals:\n";
		for(const auto &norm : mesh.GetNormals())
			std::cout<<"("<<norm[0]<<", "<<norm[1]<<", "<<norm[2]<<")"<<std::endl;

		std::cout<<"Parts:\n";
		for(const auto &part : mesh.GetParts())
		{
			std::cout<<"Name: "<<part.name<<" Material: "<<part.material_name<<std::endl;
			for(const auto &surf : part.surfaces)
			{
				for(std::size_t i = 0; i < 3; i++)
					std::cout<<"("<<surf[i][0]<<", "<<surf[i][1]<<", "<<surf[i][2]<<")"<<" ";

				std::cout<<"\n";
			}
		}

		mesh_data = mesh.CreateData();

		std::cout<<"Part indices:\n";
		for(std::size_t i = 0; i < mesh_data.part_indices.size(); i++)
		{
			std::cout<<"Name: "<<mesh.GetParts()[i].name<<" Material: "<<mesh.GetParts()[i].material_name<<std::endl;
			for(auto index : mesh_data.part_indices[i].indices)
				std::cout<<index<<" ";

			std::cout<<"\n";
		}

		std::cout<<"Vertex attributes:\n";
		for(const auto &data : mesh_data.vertex_attributes)
		{
			std::cout<<std::format("({}, {}, {}); ({}, {}); ({}, {}, {})\n",
									 data.vertex[0], data.vertex[1], data.vertex[2],
									 data.texture[0], data.texture[1],
									 data.normal[0], data.normal[1], data.normal[2]);
		}

	}
	catch(const ObjParserError &ex)
	{
		std::cout<<ObjParserResultToString(ex.result)<<" on "<<ex.col<<std::endl;
		exit(-1);
	}
	catch(const MtlParserError &ex)
	{
		std::cout<<MtlParserResultToString(ex.result)<<" on "<<ex.col<<std::endl;
		exit(-1);
	}
	catch(const std::exception &ex)
	{
		std::cout<<ex.what()<<std::endl;
		exit(-1);
	}
	catch(...)
	{
		std::cout<<"Unmanaged exception!"<<std::endl;
		exit(-1);
	}

	Shader shader;
	try
	{
		shader.Create("../../gamedata/shaders/geom.vert", "../../gamedata/shaders/geom.frag");
	}
	catch(std::runtime_error &err)
	{
		std::cout<<err.what()<<std::endl;
		exit(-1);
	}

	RenderableMesh render_mesh;
	try
	{
		render_mesh.Create(mesh_data, materials);
	}
	catch(std::runtime_error &err)
	{
		std::cout<<err.what()<<std::endl;
		exit(-1);
	}

	auto uniform_projection_mat_location = glGetUniformLocation(shader.GetProgram(), "perspective_matrix");
	auto uniform_view_mat_location = glGetUniformLocation(shader.GetProgram(), "view_matrix");
	auto uniform_model_mat_location = glGetUniformLocation(shader.GetProgram(), "model_matrix");

	projection_mat = Perspective(deg_to_rad(FOV), GetAspect(window), NEAR, FAR);
	auto model_mat = hrs::math::glsl::std430::mat4x4::identity();
	model_mat[3][1] = -1;
	model_mat[3][2] = 2;

	glEnable(GL_DEPTH_TEST);
	const Material *target_material = nullptr;
	while(is_run)
	{
		SDLEventPoll(window);
		HandleMovement();

		glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.Use();
		render_mesh.Bind();

		auto view_mat = view_translate * view_rotate.transpose();
		glUniformMatrix4fv(uniform_projection_mat_location, 1, false, projection_mat.data[0].data);
		glUniformMatrix4fv(uniform_view_mat_location, 1, false, view_mat.data[0].data);
		glUniformMatrix4fv(uniform_model_mat_location, 1, false, model_mat.data[0].data);
		for(const auto &part : render_mesh.GetParts())
		{
			if(part.material != target_material)
			{
				part.material->Bind(0);
				target_material = part.material;
			}

			glDrawElements(GL_TRIANGLES, part.count, GL_UNSIGNED_INT, reinterpret_cast<void *>(part.offset));
		}

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
