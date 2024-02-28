cmake_minimum_required(VERSION 3.5)

project(mdeng LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Sources

	main.cpp

	Wavefront/Common.hpp
	Wavefront/ObjParser.h
	Wavefront/ObjParser.cpp
	Wavefront/MtlParser.h
	Wavefront/MtlParser.cpp
	Wavefront/Mesh.h
	Wavefront/Mesh.cpp
	Wavefront/MaterialLib.h
	Wavefront/MaterialLib.cpp

	hrs/math/math_common.hpp
	hrs/math/matrix_common.hpp
	hrs/math/matrix_view.hpp
	hrs/math/matrix.hpp
	hrs/math/quaternion.hpp
	hrs/math/vector_common.hpp
	hrs/math/vector_view.hpp
	hrs/math/vector.hpp

	Shader/Shader.h
	Shader/Shader.cpp

	Render/RenderableMesh.h
	Render/RenderableMesh.cpp

	Material/Material.h
	Material/Material.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "../../out/debug/")

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME} ${Sources})

set(Libs ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${Libs})

