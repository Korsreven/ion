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
#include <tuple>
#include <vector>

#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics
{
	namespace materials
	{
		class Material;
	}

	namespace scene
	{
		class Model;
	}

	namespace shaders
	{
		class ShaderProgram;
	}
}

namespace ion::graphics::render
{
	using namespace types::type_literals;
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

		enum class MeshTexCoordMode
		{
			Manual,
			Auto
		};

		struct Vertex final
		{
			Vector3 Position;
			Vector3 Normal;
			Color BaseColor;
			Vector2 TexCoord;


			//Construct a new vertex with the given position
			Vertex(const Vector3 &position) noexcept;

			//Construct a new vertex with the given position, normal and base color (default white)
			Vertex(const Vector3 &position, const Vector3 &normal, const Color &base_color = color::White) noexcept;

			//Construct a new vertex with the given position, normal, tex coord and base color (default white)
			Vertex(const Vector3 &position, const Vector3 &normal, const Vector2 &tex_coord, const Color &base_color = color::White) noexcept;
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

			std::tuple<Aabb, Obb, Sphere> generate_bounding_volumes(int vertex_count, const vertex_storage_type &vertex_data);
			void generate_tex_coords(int vertex_count, vertex_storage_type &vertex_data, const Aabb &aabb) noexcept;
			void normalize_tex_coords(int vertex_count, vertex_storage_type &vertex_data, const materials::Material *material) noexcept;


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
		
			mesh::MeshDrawMode draw_mode_ = mesh::MeshDrawMode::Triangles;
			mesh::detail::vertex_storage_type vertex_data_;
			materials::Material *material_ = nullptr;
			mesh::MeshTexCoordMode tex_coord_mode_ = mesh::MeshTexCoordMode::Auto;
			bool show_wireframe_ = false;
			bool visible_ = true;
			
			int vertex_count_ = 0;
			duration time_ = 0.0_sec;
			Aabb aabb_;
			Obb obb_;
			Sphere sphere_;

			scene::Model *parent_model_ = nullptr;
			std::optional<int> vao_handle_;
			std::optional<int> vbo_handle_;
			int vertex_buffer_offset_ = 0;

			bool reload_vertex_data_ = false;
			bool rebind_vertex_attributes_ = false;
			bool update_bounding_volumes_ = false;
			bool update_tex_coords_ = false;

		public:

			//Construct a new mesh with the given vertices and visibility
			explicit Mesh(const mesh::Vertices &vertices, bool visible = true);

			//Construct a new mesh with the given vertices, material, tex coord mode and visibility
			Mesh(const mesh::Vertices &vertices, materials::Material &material,
				mesh::MeshTexCoordMode tex_coord_mode = mesh::MeshTexCoordMode::Auto, bool visible = true);

			//Construct a new mesh with the given draw mode, vertices and visibility
			Mesh(mesh::MeshDrawMode draw_mode, const mesh::Vertices &vertices, bool visible = true);

			//Construct a new mesh with the given draw mode, vertices, material, tex coord mode and visibility
			Mesh(mesh::MeshDrawMode draw_mode, const mesh::Vertices &vertices, materials::Material &material,
				mesh::MeshTexCoordMode tex_coord_mode = mesh::MeshTexCoordMode::Auto, bool visible = true);


			//Construct a new mesh with the given raw vertex data and visibility
			explicit Mesh(mesh::detail::vertex_storage_type vertex_data, bool visible = true);

			//Construct a new mesh with the given raw vertex data, material, tex coord mode and visibility
			Mesh(mesh::detail::vertex_storage_type vertex_data, materials::Material &material,
				mesh::MeshTexCoordMode tex_coord_mode = mesh::MeshTexCoordMode::Auto, bool visible = true);

			//Construct a new mesh with the given draw mode, raw vertex data and visibility
			Mesh(mesh::MeshDrawMode draw_mode, mesh::detail::vertex_storage_type vertex_data, bool visible = true);

			//Construct a new mesh with the given draw mode, raw vertex data, material, tex coord mode and visibility
			Mesh(mesh::MeshDrawMode draw_mode, mesh::detail::vertex_storage_type vertex_data, materials::Material &material,
				mesh::MeshTexCoordMode tex_coord_mode = mesh::MeshTexCoordMode::Auto, bool visible = true);

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

			//Sets the material used by this mesh to the given material
			inline void MaterialPtr(materials::Material *material) noexcept
			{
				if (material_ != material)
				{
					material_ = material;
					reload_vertex_data_ = vbo_handle_ && vertex_count_ > 0;
					update_tex_coords_ = vertex_count_ > 0;
				}
			}

			//Sets the tex coord mode of this mesh to the given mode
			inline void TexCoordMode(mesh::MeshTexCoordMode tex_coord_mode) noexcept
			{
				if (tex_coord_mode_ != tex_coord_mode)
				{
					tex_coord_mode_ = tex_coord_mode;
					update_tex_coords_ = vertex_count_ > 0;
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


			//Sets parent model of this mesh to the given model
			inline void ParentModel(scene::Model *model) noexcept
			{
				parent_model_ = model;
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

			//Returns the draw mode of this mesh
			[[nodiscard]] inline auto DrawMode() const noexcept
			{
				return draw_mode_;
			}

			//Returns all of the vertex data from this mesh
			[[nodiscard]] inline const auto& VertexData() const noexcept
			{
				return vertex_data_;
			}

			//Returns a pointer to the material (mutable) used by this mesh
			//Returns nullptr if this mesh does not have a material
			[[nodiscard]] inline auto MaterialPtr() noexcept
			{
				return material_;
			}

			//Returns a pointer to the material (immutable) used by this mesh
			//Returns nullptr if this mesh does not have a material
			[[nodiscard]] inline const auto MaterialPtr() const noexcept
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


			//Returns the vertex count of this mesh
			[[nodiscard]] inline auto VertexCount() const noexcept
			{
				return vertex_count_;
			}

			//Returns the local axis-aligned bounding box (AABB) for this mesh
			[[nodiscard]] inline auto AxisAlignedBoundingBox() const noexcept
			{
				return aabb_;
			}

			//Returns the local oriented bounding box (OBB) for this mesh
			[[nodiscard]] inline auto OrientedBoundingBox() const noexcept
			{
				return obb_;
			}

			//Returns the local bounding sphere for this mesh
			[[nodiscard]] inline auto BoundingSphere() const noexcept
			{
				return sphere_;
			}


			//Returns a pointer to a mutable parent model for this mesh
			[[nodiscard]] inline auto ParentModel() noexcept
			{
				return parent_model_;
			}

			//Returns a pointer to an immutable parent model for this mesh
			[[nodiscard]] inline const auto ParentModel() const noexcept
			{
				return parent_model_;
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


			/*
				Elapse time
			*/

			//Elapse mesh by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;
	};
} //ion::graphics::render

#endif