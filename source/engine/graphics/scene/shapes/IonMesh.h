/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonMesh.h
-------------------------------------------
*/

#ifndef ION_MESH_H
#define ION_MESH_H

#include <optional>
#include <string>
#include <vector>

#include "graphics/render/IonRenderPrimitive.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/render/vertex/IonVertexDeclaration.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector3.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
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
}

namespace ion::graphics::scene::shapes
{
	using namespace render;
	using namespace types::type_literals;
	using namespace utilities;	

	namespace mesh
	{
		enum class MeshTexCoordMode : bool
		{
			Manual,
			Auto
		};

		struct Vertex final
		{
			Vector3 Position;
			Vector3 Normal;
			Color BaseColor;
			Vector3 TexCoord;


			///@brief Constructs a new vertex with the given position and base color
			explicit Vertex(const Vector3 &position, const Color &base_color = color::White) noexcept;

			///@brief Constructs a new vertex with the given position, normal and base color (default white)
			Vertex(const Vector3 &position, const Vector3 &normal, const Color &base_color = color::White) noexcept;

			///@brief Constructs a new vertex with the given position, normal, tex coord and base color (default white)
			Vertex(const Vector3 &position, const Vector3 &normal, const Vector2 &tex_coord, const Color &base_color = color::White) noexcept;
		};

		using Vertices = std::vector<Vertex>;


		namespace detail
		{
			constexpr auto position_components = 3; //x,y,z
			constexpr auto normal_components = 3; //x,y,z
			constexpr auto color_components = 4; //r,g,b,a
			constexpr auto tex_coord_components = 3; //s,t,u

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
								vertex::vertex_declaration::VertexElementType::Float3,
								tex_coord_offset * sizeof(real), vertex_components * sizeof(real)}
						}
					};
			}


			render_primitive::VertexContainer vertices_to_vertex_data(const Vertices &vertices);

			void generate_tex_coords(render_primitive::VertexContainer &vertex_data, const Aabb &aabb) noexcept;
			void normalize_tex_coords(render_primitive::VertexContainer &vertex_data, const materials::Material *material) noexcept;
		} //detail
	} //mesh


	///@brief A class representing a mesh that supports any complex shape
	///@details This base class must support inheritance (open set of shapes)
	class Mesh :
		public managed::ManagedObject<scene::Model>,
		public render::RenderPrimitive
	{
		private:

			mesh::MeshTexCoordMode tex_coord_mode_ = mesh::MeshTexCoordMode::Auto;
			bool include_bounding_volumes_ = true;
			bool update_tex_coords_ = true;

		protected:

			/**
				@name Events
				@{
			*/

			virtual void VertexDataChanged() noexcept override;
			virtual void RenderPassesChanged() noexcept override;
			virtual void MaterialChanged() noexcept override;

		public:

			///@brief Constructs a new mesh with the given name, vertices and visibility
			Mesh(std::optional<std::string> name, const mesh::Vertices &vertices, bool visible = true);

			///@brief Constructs a new mesh with the given name, vertices, material, tex coord mode and visibility
			Mesh(std::optional<std::string> name, const mesh::Vertices &vertices,
				NonOwningPtr<materials::Material> material, mesh::MeshTexCoordMode tex_coord_mode = mesh::MeshTexCoordMode::Auto, bool visible = true);

			///@brief Constructs a new mesh with the given name, draw mode, vertices and visibility
			Mesh(std::optional<std::string> name, vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices, bool visible = true);

			///@brief Constructs a new mesh with the given name, draw mode, vertices, material, tex coord mode and visibility
			Mesh(std::optional<std::string> name, vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices,
				NonOwningPtr<materials::Material> material, mesh::MeshTexCoordMode tex_coord_mode = mesh::MeshTexCoordMode::Auto, bool visible = true);


			///@brief Constructs a new mesh with the given name, raw vertex data and visibility
			Mesh(std::optional<std::string> name, render_primitive::VertexContainer vertex_data, bool visible = true) noexcept;

			///@brief Constructs a new mesh with the given name, raw vertex data, material, tex coord mode and visibility
			Mesh(std::optional<std::string> name, render_primitive::VertexContainer vertex_data,
				NonOwningPtr<materials::Material> material, mesh::MeshTexCoordMode tex_coord_mode = mesh::MeshTexCoordMode::Auto, bool visible = true) noexcept;

			///@brief Constructs a new mesh with the given name, draw mode, raw vertex data and visibility
			Mesh(std::optional<std::string> name, vertex::vertex_batch::VertexDrawMode draw_mode, render_primitive::VertexContainer vertex_data, bool visible = true) noexcept;

			///@brief Constructs a new mesh with the given name, draw mode, raw vertex data, material, tex coord mode and visibility
			Mesh(std::optional<std::string> name, vertex::vertex_batch::VertexDrawMode draw_mode, render_primitive::VertexContainer vertex_data,
				NonOwningPtr<materials::Material> material, mesh::MeshTexCoordMode tex_coord_mode = mesh::MeshTexCoordMode::Auto, bool visible = true) noexcept;


			///@brief Default virtual destructor
			virtual ~Mesh() = default;


			/**
				@name Modifiers
				@{
			*/

			using RenderPrimitive::VertexData; //Mitigate name hiding

			///@brief Sets the vertex data of this mesh to the given vertices
			inline void VertexData(const mesh::Vertices &vertices)
			{
				VertexData(mesh::detail::vertices_to_vertex_data(vertices));
			}


			///@brief Sets the surface material used by this mesh to the given material
			inline void SurfaceMaterial(NonOwningPtr<materials::Material> material) noexcept
			{
				RenderMaterial(material);
			}

			///@brief Sets the tex coord mode of this mesh to the given mode
			inline void TexCoordMode(mesh::MeshTexCoordMode tex_coord_mode) noexcept
			{
				if (tex_coord_mode_ != tex_coord_mode)
				{
					tex_coord_mode_ = tex_coord_mode;
					update_tex_coords_ = true;
				}
			}

			///@brief Sets if the bounding volumes from this mesh should be included in the model or not
			inline void IncludeBoundingVolumes(bool include) noexcept
			{
				if (include_bounding_volumes_ != include)
				{
					include_bounding_volumes_ = include;
					Mesh::VertexDataChanged(); //To force bounding volume update
				}
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the tex coord mode of this mesh
			[[nodiscard]] inline auto TexCoordMode() const noexcept
			{
				return tex_coord_mode_;
			}

			///@brief Returns true if the bounding volumes from this mesh should be included in the model
			[[nodiscard]] inline auto IncludeBoundingVolumes() const noexcept
			{
				return include_bounding_volumes_;
			}

			///@}

			/**
				@name Preparing
				@{
			*/

			///@brief Prepares this mesh such that it is ready to be drawn
			///@details This function is typically called each frame
			virtual void Prepare() override;

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses the total time for this mesh by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			virtual void Elapse(duration time) noexcept;

			///@}
	};
} //ion::graphics::scene::shapes

#endif