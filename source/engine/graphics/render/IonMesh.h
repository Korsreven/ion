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
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "graphics/utilities/IonVector2.h"
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
	using namespace utilities;

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

		struct Vertex
		{
			Vector2 position;
			Vector2 normal;
			Color color;
			Vector2 tex_coord;
		};

		using Vertices = std::vector<Vertex>;


		namespace detail
		{
			using vertex_storage_type = std::vector<real>;

			constexpr auto position_components = 3; //x,y,z
			constexpr auto normal_components = 3; //x,y,z
			constexpr auto color_components = 4; //r,g,b,a
			constexpr auto tex_coord_components = 2; //s,t

			constexpr auto vertex_components =
				position_components + normal_components + color_components + tex_coord_components;


			constexpr auto vertex_data_size(int vertex_count) noexcept
			{
				return vertex_count * vertex_components;
			}

			constexpr auto normal_data_offset(int vertex_count) noexcept
			{
				return vertex_count * position_components;
			}

			constexpr auto color_data_offset(int vertex_count) noexcept
			{
				return normal_data_offset(vertex_count) + vertex_count * normal_components;
			}

			constexpr auto tex_coord_data_offset(int vertex_count) noexcept
			{
				return color_data_offset(vertex_count) + vertex_count * color_components;
			}


			int mesh_draw_mode_to_gl_draw_mode(MeshDrawMode draw_mode) noexcept;
			vertex_storage_type vertices_to_vertex_data(const Vertices &vertices);


			/*
				Graphics API
			*/

			std::optional<int> create_vertex_array_object() noexcept;
			void delete_vertex_array_object(int vao_handle) noexcept;
			
			void bind_vertex_array_object(int vao_handle) noexcept;
			void bind_vertex_buffer_object(int vbo_handle) noexcept;
			void bind_vertex_attributes(int vao_handle, int vbo_handle, int vertex_count, int vbo_offset) noexcept;

			void set_vertex_buffer_sub_data(int vbo_handle, int vbo_offset, const vertex_storage_type &vertex_data) noexcept;	
			void set_vertex_attribute_pointers(int vertex_count, int vbo_offset) noexcept;
			void set_vertex_attribute_pointers(int vertex_count, const vertex_storage_type &vertex_data) noexcept;
			void set_vertex_pointers(int vertex_count, int vbo_offset) noexcept;
			void set_vertex_pointers(int vertex_count, const vertex_storage_type &vertex_data) noexcept;

			void use_shader_program(int program_handle) noexcept;		
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
			bool reload_vertex_data_ = false;
			bool rebind_vertex_attributes_ = false;

		public:

			//Construct a new mesh with the given vertices
			Mesh(const mesh::Vertices &vertices);

			//Construct a new mesh with the given raw vertex data
			Mesh(mesh::detail::vertex_storage_type vertex_data);

			//Destructor
			~Mesh() noexcept;


			/*
				Modifiers
			*/

			//Sets the draw mode of this mesh to the given mode
			inline void DrawMode(mesh::MeshDrawMode draw_mode) noexcept
			{
				draw_mode_ = draw_mode;
			}

			//Attach the given material to this mesh
			//Detach an already attached material by passing nullptr
			inline void AttachMaterial(materials::Material *material) noexcept
			{
				if (material_ != material)
				{
					material_ = material;
					reload_vertex_data_ = vbo_handle_ && vertex_count_ > 0;
				}
			}

			//Sets if this mesh should be shown in wireframe or not
			inline void ShowWireframe(bool show) noexcept
			{
				show_wireframe_ = show;
			}

			//Sets the visibility of this mesh to the given value
			inline void Visible(bool visible) noexcept
			{
				visible_ = visible;
			}


			//Sets the VBO handle and offset to the given values
			inline void VboHandle(std::optional<int> handle, int offset) noexcept
			{
				if (vbo_handle_ != handle || vertex_buffer_offset_ != offset)
				{
					vbo_handle_ = handle;
					vertex_buffer_offset_ = offset;
					rebind_vertex_attributes_ = vbo_handle_ && vertex_count_ > 0;
				}
			}


			/*
				Observers
			*/

			//Returns the vertex count of this mesh
			[[nodiscard]] inline auto VertexCount() const noexcept
			{
				return vertex_count_;
			}

			//Returns the draw mode of this mesh
			[[nodiscard]] inline auto DrawMode() const noexcept
			{
				return draw_mode_;
			}

			//Returns a pointer to the material (mutable) attached to this mesh
			//Returns nullptr if this mesh does not have a material attached
			[[nodiscard]] inline auto AttachedMaterial() noexcept
			{
				return material_;
			}

			//Returns a pointer to the material (immutable) attached to this mesh
			//Returns nullptr if this mesh does not have a material attached
			[[nodiscard]] inline const auto AttachedMaterial() const noexcept
			{
				return material_;
			}

			//Returns true if this mesh is shown in wireframe
			[[nodiscard]] inline auto ShowWireframe() const noexcept
			{
				return show_wireframe_;
			}

			//Returns true if this mesh is visible
			[[nodiscard]] inline auto Visible() const noexcept
			{
				return visible_;
			}


			//Returns the VAO handle this mesh uses
			[[nodiscard]] inline auto VaoHandle() const noexcept
			{
				return vbo_handle_;
			}

			//Returns the VBO handle this mesh uses
			[[nodiscard]] inline auto VboHandle() const noexcept
			{
				return vbo_handle_;
			}


			//Returns all of the vertex data from this mesh
			[[nodiscard]] inline const auto& VertexData() const noexcept
			{
				return vertex_data_;
			}


			/*
				Vertices
			*/

			//Sets all vertices of the mesh to the given color
			//Applies only if no materials is attached to this mesh
			void VertexColor(const Color &color) noexcept;


			/*
				Drawing
			*/

			//Prepare this mesh such that it is ready to be drawn
			//This is called once regardless of passes
			void Prepare() noexcept;

			//Draw this mesh with the given shader program (optional)
			//This can be called multiple times if more than one pass
			void Draw(shaders::ShaderProgram *shader_program = nullptr) noexcept;
	};
} //ion::graphics::render

#endif