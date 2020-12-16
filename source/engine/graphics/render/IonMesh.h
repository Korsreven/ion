/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonMesh.h
-------------------------------------------
*/

#ifndef ION_MESH_H
#define ION_MESH_H

#include <optional>
#include <vector>

#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "types/IonTypes.h"

namespace ion::graphics
{
	namespace materials
	{
		class Material;
	}

	namespace shaders
	{
		class ShaderProgram;
	}
}

namespace ion::graphics::render
{
	using utilities::Aabb;
	using utilities::Obb;
	using utilities::Sphere;

	namespace mesh
	{
		enum class MeshDrawMode
		{
			Points,
			Lines,
			LineLoop,
			LineStrip,
			Triangles,
			TriangleFan,
			TriangleStrip,
			Quads,
			Polygon
		};

		namespace detail
		{
			using vertex_storage_type = std::vector<real>;


			constexpr auto vertex_components_ = 3; //x,y,z
			constexpr auto normal_components_ = 3; //x,y,z
			constexpr auto color_components_ = 4; //r,g,b,a
			constexpr auto tex_coord_components_ = 2; //s,t

			constexpr auto vertex_data_size(int vertex_count) noexcept
			{
				return vertex_count * vertex_components_ +
					   vertex_count * normal_components_ +
					   vertex_count * color_components_ +
					   vertex_count * tex_coord_components_;
			}

			constexpr auto normal_data_offset(int vertex_count) noexcept
			{
				return vertex_count * vertex_components_;
			}

			constexpr auto color_data_offset(int vertex_count) noexcept
			{
				return normal_data_offset(vertex_count) + vertex_count * normal_components_;
			}

			constexpr auto tex_coord_data_offset(int vertex_count) noexcept
			{
				return color_data_offset(vertex_count) + vertex_count * color_components_;
			}


			int mesh_draw_mode_to_gl_draw_mode(MeshDrawMode draw_mode) noexcept;


			std::optional<int> create_vertex_array_object(int vbo_handle) noexcept;
			void delete_vertex_array_object(int vao_handle) noexcept;

			void use_shader_program(int program_handle) noexcept;
			void bind_vertex_buffer_object(int vbo_handle) noexcept;

			void set_vertex_attribute_pointers(int vertex_count, int vbo_offset) noexcept;
			void set_vertex_attribute_pointers(int vertex_count, const vertex_storage_type &vertex_data) noexcept;

			void set_vertex_pointers(int vertex_count, int vbo_offset) noexcept;
			void set_vertex_pointers(int vertex_count, const vertex_storage_type &vertex_data) noexcept;
		} //detail
	} //mesh


	class Mesh final
	{
		private:

			int vertex_count_ = 0;
			mesh::MeshDrawMode draw_mode_ = mesh::MeshDrawMode::Triangles;
			materials::Material *material_ = nullptr;
			bool show_wireframe_ = false;
			bool visible_ = true;

			std::optional<int> vao_handle_;
			std::optional<int> vbo_handle_;
			int vertex_buffer_offset_ = 0;

			std::optional<Aabb> aabb_;
			std::optional<Obb> obb_;
			std::optional<Sphere> sphere_;
			
			mesh::detail::vertex_storage_type vertex_data_;
			bool reload_vertex_array_ = false;

		public:

			//Constructor
			Mesh(std::optional<int> vbo_handle);

			//Destructor
			~Mesh();


			/*
				Modifiers
			*/




			/*
				Observers
			*/




			/*
				Drawing
			*/

			//
			void Draw(shaders::ShaderProgram *shader_program);
	};
} //ion::graphics::render

#endif