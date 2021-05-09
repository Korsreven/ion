/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonModel.h
-------------------------------------------
*/

#ifndef ION_MODEL_H
#define ION_MODEL_H

#include <optional>
#include <type_traits>
#include <vector>

#include "IonDrawableObject.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/render/vertex/IonVertexBufferObject.h"
#include "memory/IonNonOwningPtr.h"
#include "shapes/IonMesh.h"
#include "shapes/IonShape.h"
#include "types/IonTypes.h"
#include "unmanaged/IonObjectFactory.h"

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

namespace ion::graphics::scene
{
	namespace model::detail
	{
	} //model


	class Model final :
		public DrawableObject,
		protected unmanaged::ObjectFactory<shapes::Mesh>
	{
		private:

			std::optional<render::vertex::VertexBufferObject> vbo_;
			bool reload_vertex_buffer_ = false;


			void Created(shapes::Mesh &mesh);

		public:

			//Construct a new model with the given visibility
			explicit Model(bool visible = true) noexcept;


			/*
				Ranges
			*/

			//Returns a mutable range of all meshes in this model
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Meshes() noexcept
			{
				return Objects();
			}

			//Returns an immutable range of all meshes in this model
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Meshes() const noexcept
			{
				return Objects();
			}


			/*
				Observers
			*/

			//Return the vertex buffer this model uses
			[[nodiscard]] inline auto& VertexBuffer() const noexcept
			{
				return vbo_;
			}


			/*
				Preparing / drawing
			*/

			//Prepare this model such that it is ready to be drawn
			//This is called once regardless of passes
			void Prepare() noexcept override;

			//Draw this model with the given shader program (optional)
			//This can be called multiple times if more than one pass
			void Draw(shaders::ShaderProgram *shader_program = nullptr) noexcept override;


			/*
				Elapse time
			*/

			//Elapse the total time for each mesh in this model by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept override;


			/*
				Meshes
				Creating
			*/

			//Create a mesh with the given vertices and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(const shapes::mesh::Vertices &vertices, bool visible = true);

			//Create a mesh with the given vertices, material, tex coord mode and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(const shapes::mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
				shapes::mesh::MeshTexCoordMode tex_coord_mode = shapes::mesh::MeshTexCoordMode::Auto, bool visible = true);

			//Create a mesh with the given draw mode, vertices and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, const shapes::mesh::Vertices &vertices, bool visible = true);

			//Create a mesh with the given draw mode, vertices, material, tex coord mode and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, const shapes::mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
				shapes::mesh::MeshTexCoordMode tex_coord_mode = shapes::mesh::MeshTexCoordMode::Auto, bool visible = true);


			//Create a mesh with the given raw vertex data and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(shapes::mesh::VertexContainer vertex_data, bool visible = true);

			//Create a mesh with the given raw vertex data, material, tex coord mode and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(shapes::mesh::VertexContainer vertex_data, NonOwningPtr<materials::Material>material,
				shapes::mesh::MeshTexCoordMode tex_coord_mode = shapes::mesh::MeshTexCoordMode::Auto, bool visible = true);

			//Create a mesh with the given draw mode, raw vertex data and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, shapes::mesh::VertexContainer vertex_data, bool visible = true);

			//Create a mesh with the given draw mode, raw vertex data, material, tex coord mode and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, shapes::mesh::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
				shapes::mesh::MeshTexCoordMode tex_coord_mode = shapes::mesh::MeshTexCoordMode::Auto, bool visible = true);


			//Create a mesh as a copy of the given mesh
			NonOwningPtr<shapes::Mesh> CreateMesh(const shapes::Mesh &mesh);

			//Create a mesh by moving the given mesh
			NonOwningPtr<shapes::Mesh> CreateMesh(shapes::Mesh &&mesh);


			/*
				Meshes (derived)
				Creating
			*/

			//Create a mesh of type T with the given arguments
			template <typename T, typename... Args,
				typename = std::enable_if_t<std::is_base_of_v<shapes::Shape, T>>>
			auto CreateMesh(Args &&...args)
			{
				static_assert(std::is_base_of_v<shapes::Mesh, T>);

				auto ptr = Create<T>(std::forward<Args>(args)...);
				Created(*ptr);
				return static_pointer_cast<T>(ptr);
			}


			//Create a mesh of type T as a copy of the given mesh
			template <typename T,
				typename = std::enable_if_t<std::is_base_of_v<shapes::Shape, T>>>
			auto CreateMesh(const T &mesh_t)
			{
				static_assert(std::is_base_of_v<shapes::Mesh, T>);

				auto ptr = Create(mesh_t);
				Created(*ptr);
				return static_pointer_cast<T>(ptr);
			}

			//Create a mesh of type T by moving the given mesh
			template <typename T,
				typename = std::enable_if_t<std::is_base_of_v<shapes::Shape, T>>>
			auto CreateMesh(T &&mesh_t)
			{
				static_assert(std::is_base_of_v<shapes::Mesh, T>);

				auto ptr = Create(std::move(mesh_t));
				Created(*ptr);
				return static_pointer_cast<T>(ptr);
			}


			/*
				Meshes
				Removing
			*/

			//Clear all meshes from this model
			void ClearMeshes() noexcept;

			//Remove a mesh from this model
			bool RemoveMesh(shapes::Mesh &mesh) noexcept;
	};
} //ion::graphics::scene

#endif