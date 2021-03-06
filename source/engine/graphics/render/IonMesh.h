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

#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/render/vertex/IonVertexBufferView.h"
#include "graphics/render/vertex/IonVertexDeclaration.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
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
	using namespace types::type_literals;
	using namespace utilities;	

	namespace mesh
	{
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
		using VertexContainer = std::vector<real>;	


		namespace detail
		{
			constexpr auto position_components = 3; //x,y,z
			constexpr auto normal_components = 3; //x,y,z
			constexpr auto color_components = 4; //r,g,b,a
			constexpr auto tex_coord_components = 2; //s,t

			constexpr auto position_offset = 0;
			constexpr auto normal_offset = position_offset + position_components;
			constexpr auto color_offset = normal_offset + normal_components;
			constexpr auto tex_coord_offset = color_offset + color_components;

			constexpr auto vertex_components =
				position_components + normal_components + color_components + tex_coord_components;


			inline auto get_vertex_declaration() noexcept
			{
				return
					vertex::VertexDeclaration
					{
						{
							{shaders::shader_layout::AttributeName::Vertex_Position,
								vertex::vertex_declaration::VertexElementType::Float3,
								position_offset * sizeof(real), vertex_components * sizeof(real)},

							{shaders::shader_layout::AttributeName::Vertex_Normal,
								vertex::vertex_declaration::VertexElementType::Float3,
								normal_offset * sizeof(real), vertex_components * sizeof(real)},

							{shaders::shader_layout::AttributeName::Vertex_Color,
								vertex::vertex_declaration::VertexElementType::Float4,
								color_offset * sizeof(real), vertex_components * sizeof(real)},

							{shaders::shader_layout::AttributeName::Vertex_TexCoord,
								vertex::vertex_declaration::VertexElementType::Float2,
								tex_coord_offset * sizeof(real), vertex_components * sizeof(real)}
						}
					};
			}


			VertexContainer vertices_to_vertex_data(const Vertices &vertices);

			std::tuple<Aabb, Obb, Sphere> generate_bounding_volumes(const VertexContainer &vertex_data);
			void generate_tex_coords(VertexContainer &vertex_data, const Aabb &aabb) noexcept;
			void normalize_tex_coords(VertexContainer &vertex_data, const materials::Material *material) noexcept;


			/*
				Graphics API
			*/

			void enable_wire_frames() noexcept;
			void disable_wire_frames() noexcept;
		} //detail
	} //mesh


	class Mesh final
	{
		private:

			mesh::VertexContainer vertex_data_;
			mesh::MeshTexCoordMode tex_coord_mode_ = mesh::MeshTexCoordMode::Auto;
			bool show_wireframe_ = false;
			bool visible_ = true;
			
			Aabb aabb_;
			Obb obb_;
			Sphere sphere_;

			vertex::VertexBatch vertex_batch_;
			bool update_bounding_volumes_ = false;
			bool update_tex_coords_ = false;

		public:

			//Construct a new mesh with the given vertices and visibility
			explicit Mesh(const mesh::Vertices &vertices, bool visible = true);

			//Construct a new mesh with the given vertices, material, tex coord mode and visibility
			Mesh(const mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
				mesh::MeshTexCoordMode tex_coord_mode = mesh::MeshTexCoordMode::Auto, bool visible = true);

			//Construct a new mesh with the given draw mode, vertices and visibility
			Mesh(vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices, bool visible = true);

			//Construct a new mesh with the given draw mode, vertices, material, tex coord mode and visibility
			Mesh(vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
				mesh::MeshTexCoordMode tex_coord_mode = mesh::MeshTexCoordMode::Auto, bool visible = true);


			//Construct a new mesh with the given raw vertex data and visibility
			explicit Mesh(mesh::VertexContainer vertex_data, bool visible = true);

			//Construct a new mesh with the given raw vertex data, material, tex coord mode and visibility
			Mesh(mesh::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
				mesh::MeshTexCoordMode tex_coord_mode = mesh::MeshTexCoordMode::Auto, bool visible = true);

			//Construct a new mesh with the given draw mode, raw vertex data and visibility
			Mesh(vertex::vertex_batch::VertexDrawMode draw_mode, mesh::VertexContainer vertex_data, bool visible = true);

			//Construct a new mesh with the given draw mode, raw vertex data, material, tex coord mode and visibility
			Mesh(vertex::vertex_batch::VertexDrawMode draw_mode, mesh::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
				mesh::MeshTexCoordMode tex_coord_mode = mesh::MeshTexCoordMode::Auto, bool visible = true);


			/*
				Modifiers
			*/

			//Sets the draw mode of this mesh to the given mode
			inline void DrawMode(vertex::vertex_batch::VertexDrawMode draw_mode) noexcept
			{
				vertex_batch_.DrawMode(draw_mode);
			}

			//Sets the surface color of this mesh to the given color
			void SurfaceColor(const Color &color) noexcept;

			//Sets the surface material used by this mesh to the given material
			inline void SurfaceMaterial(NonOwningPtr<materials::Material> material) noexcept
			{
				vertex_batch_.BatchMaterial(material);
			}

			//Sets the tex coord mode of this mesh to the given mode
			inline void TexCoordMode(mesh::MeshTexCoordMode tex_coord_mode) noexcept
			{
				if (tex_coord_mode_ != tex_coord_mode)
				{
					tex_coord_mode_ = tex_coord_mode;
					update_tex_coords_ = true;
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


			//Sets the vertex buffer to the given vertex buffer
			inline void VertexBuffer(vertex::VertexBufferView vertex_buffer, bool reload_data = true) noexcept
			{
				vertex_batch_.VertexBuffer(vertex_buffer, reload_data);
			}


			/*
				Observers
			*/

			//Returns the draw mode of this mesh
			[[nodiscard]] inline auto DrawMode() const noexcept
			{
				return vertex_batch_.DrawMode();
			}

			//Returns all of the vertex data from this mesh
			[[nodiscard]] inline auto& VertexData() const noexcept
			{
				return vertex_data_;
			}

			//Returns the vertex count of this mesh
			[[nodiscard]] inline auto VertexCount() const noexcept
			{
				return vertex_batch_.VertexCount();
			}

			//Returns a pointer to the material used by this mesh
			//Returns nullptr if this mesh does not have a material
			[[nodiscard]] inline auto SurfaceMaterial() const noexcept
			{
				return vertex_batch_.BatchMaterial();
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


			//Returns the local axis-aligned bounding box (AABB) for this mesh
			[[nodiscard]] inline auto& AxisAlignedBoundingBox() const noexcept
			{
				return aabb_;
			}

			//Returns the local oriented bounding box (OBB) for this mesh
			[[nodiscard]] inline auto& OrientedBoundingBox() const noexcept
			{
				return obb_;
			}

			//Returns the local bounding sphere for this mesh
			[[nodiscard]] inline auto& BoundingSphere() const noexcept
			{
				return sphere_;
			}


			/*
				Preparing / drawing
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

			//Elapse the total time for this mesh by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;
	};
} //ion::graphics::render

#endif