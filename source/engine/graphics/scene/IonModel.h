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
#include <vector>

#include "IonMovableObject.h"
#include "graphics/render/IonMesh.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/render/vertex/IonVertexBufferObject.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "memory/IonNonOwningPtr.h"
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
	using utilities::Aabb;
	using utilities::Obb;
	using utilities::Sphere;

	namespace model::detail
	{
	} //model


	class Model final :
		public MovableObject,
		protected unmanaged::ObjectFactory<render::Mesh>
	{
		private:

			Aabb aabb_;
			Obb obb_;
			Sphere sphere_;

			std::optional<render::vertex::VertexBufferObject> vbo_;

			bool reload_vertex_buffer_ = false;
			bool update_bounding_volumes_ = false;


			void Created(render::Mesh &mesh);
			render::mesh::VertexContainer MeshVertexData() const;

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

			//Returns the local axis-aligned bounding box (AABB) for this model
			[[nodiscard]] inline auto& AxisAlignedBoundingBox() const noexcept
			{
				return aabb_;
			}

			//Returns the local oriented bounding box (OBB) for this model
			[[nodiscard]] inline auto& OrientedBoundingBox() const noexcept
			{
				return obb_;
			}

			//Returns the local bounding sphere for this model
			[[nodiscard]] inline auto& BoundingSphere() const noexcept
			{
				return sphere_;
			}


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
			void Prepare() noexcept;

			//Draw this model with the given shader program (optional)
			//This can be called multiple times if more than one pass
			void Draw(shaders::ShaderProgram *shader_program = nullptr) noexcept;


			/*
				Elapse time
			*/

			//Elapse the total time for each mesh in this model by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;


			/*
				Meshes
				Creating
			*/

			//Create a mesh with the given vertices and visibility
			NonOwningPtr<render::Mesh> CreateMesh(const render::mesh::Vertices &vertices, bool visible = true);

			//Create a mesh with the given vertices, material, tex coord mode and visibility
			NonOwningPtr<render::Mesh> CreateMesh(const render::mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
				render::mesh::MeshTexCoordMode tex_coord_mode = render::mesh::MeshTexCoordMode::Auto, bool visible = true);

			//Create a mesh with the given draw mode, vertices and visibility
			NonOwningPtr<render::Mesh> CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, const render::mesh::Vertices &vertices, bool visible = true);

			//Create a mesh with the given draw mode, vertices, material, tex coord mode and visibility
			NonOwningPtr<render::Mesh> CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, const render::mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
				render::mesh::MeshTexCoordMode tex_coord_mode = render::mesh::MeshTexCoordMode::Auto, bool visible = true);


			//Create a mesh with the given raw vertex data and visibility
			NonOwningPtr<render::Mesh> CreateMesh(render::mesh::VertexContainer vertex_data, bool visible = true);

			//Create a mesh with the given raw vertex data, material, tex coord mode and visibility
			NonOwningPtr<render::Mesh> CreateMesh(render::mesh::VertexContainer vertex_data, NonOwningPtr<materials::Material>material,
				render::mesh::MeshTexCoordMode tex_coord_mode = render::mesh::MeshTexCoordMode::Auto, bool visible = true);

			//Create a mesh with the given draw mode, raw vertex data and visibility
			NonOwningPtr<render::Mesh> CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, render::mesh::VertexContainer vertex_data, bool visible = true);

			//Create a mesh with the given draw mode, raw vertex data, material, tex coord mode and visibility
			NonOwningPtr<render::Mesh> CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, render::mesh::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
				render::mesh::MeshTexCoordMode tex_coord_mode = render::mesh::MeshTexCoordMode::Auto, bool visible = true);


			//Create a mesh as a copy of the given mesh
			NonOwningPtr<render::Mesh> CreateMesh(const render::Mesh &mesh);

			//Create a mesh by moving the given mesh
			NonOwningPtr<render::Mesh> CreateMesh(render::Mesh &&mesh);


			/*
				Meshes
				Removing
			*/

			//Clear all meshes from this model
			void ClearMeshes() noexcept;

			//Remove a mesh from this model
			bool RemoveMesh(render::Mesh &mesh) noexcept;
	};
} //ion::graphics::scene

#endif