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
#include <span>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "IonDrawableObject.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "shapes/IonMesh.h"
#include "shapes/IonShape.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics
{
	namespace materials
	{
		class Material;
	}
}

namespace ion::graphics::scene
{
	class Light; //Forward declaration

	namespace model
	{
		using Lights = std::vector<Light*>;


		namespace detail
		{
			using emissive_meshes = std::vector<std::pair<shapes::Mesh*, OwningPtr<Light>>>;

			void add_emissive_mesh(shapes::Mesh &mesh, emissive_meshes &meshes);
			bool remove_emissive_mesh(const shapes::Mesh &mesh, emissive_meshes &meshes) noexcept;
			bool is_mesh_emissive(const shapes::Mesh &mesh) noexcept;

			OwningPtr<Light> create_emissive_light(const shapes::Mesh &mesh);
		} //detail
	} //model


	//A class representing a model that can be attached to a scene node
	//A model can contain multiple meshes where each mesh is relative to the parent model
	class Model final :
		public DrawableObject,
		public managed::ObjectManager<shapes::Mesh, Model>
	{
		private:

			model::detail::emissive_meshes emissive_meshes_;
			bool update_bounding_volumes_ = false;
			bool update_emissive_lights_ = false;

		protected:

			model::Lights emissive_lights_;


			/*
				Events
			*/

			//See ObjectManager<T>::Created for more details
			void Created(shapes::Mesh &mesh) noexcept override;

			//See ObjectManager<T>::removed for more details
			void Removed(shapes::Mesh &mesh) noexcept override;

		public:

			//Constructs a new model with the given name and visibility
			explicit Model(std::optional<std::string> name = {}, bool visible = true) noexcept;

			//Virtual destructor
			virtual ~Model() noexcept;


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
			
			//Returns all emissive lights in this model
			[[nodiscard]] movable_object::LightRange AllEmissiveLights() noexcept override;


			/*
				Notifying
			*/

			//Called when vertex data has changed on the given mesh
			void NotifyVertexDataChanged(shapes::Mesh &mesh) noexcept;

			//Called when render passes has changed on the given mesh
			void NotifyRenderPassesChanged(shapes::Mesh &mesh) noexcept;

			//Called when material has changed on the given mesh
			void NotifyMaterialChanged(shapes::Mesh &mesh) noexcept;


			/*
				Preparing
			*/

			//Prepares this model such that it is ready to be drawn
			//This function is typically called each frame
			void Prepare() override;


			/*
				Elapse time
			*/

			//Elapses the total time for each mesh in this model by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept override;


			/*
				Meshes
				Creating
			*/

			//Creates a mesh with the given vertices and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(const shapes::mesh::Vertices &vertices, bool visible = true);

			//Creates a mesh with the given vertices, material, tex coord mode and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(const shapes::mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
				shapes::mesh::MeshTexCoordMode tex_coord_mode = shapes::mesh::MeshTexCoordMode::Auto, bool visible = true);

			//Creates a mesh with the given draw mode, vertices and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, const shapes::mesh::Vertices &vertices, bool visible = true);

			//Creates a mesh with the given draw mode, vertices, material, tex coord mode and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, const shapes::mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
				shapes::mesh::MeshTexCoordMode tex_coord_mode = shapes::mesh::MeshTexCoordMode::Auto, bool visible = true);


			//Creates a mesh with the given raw vertex data and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(render::render_primitive::VertexContainer vertex_data, bool visible = true);

			//Creates a mesh with the given raw vertex data, material, tex coord mode and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(render::render_primitive::VertexContainer vertex_data, NonOwningPtr<materials::Material>material,
				shapes::mesh::MeshTexCoordMode tex_coord_mode = shapes::mesh::MeshTexCoordMode::Auto, bool visible = true);

			//Creates a mesh with the given draw mode, raw vertex data and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, render::render_primitive::VertexContainer vertex_data, bool visible = true);

			//Creates a mesh with the given draw mode, raw vertex data, material, tex coord mode and visibility
			NonOwningPtr<shapes::Mesh> CreateMesh(render::vertex::vertex_batch::VertexDrawMode draw_mode, render::render_primitive::VertexContainer vertex_data, NonOwningPtr<materials::Material> material,
				shapes::mesh::MeshTexCoordMode tex_coord_mode = shapes::mesh::MeshTexCoordMode::Auto, bool visible = true);


			//Creates a mesh as a copy of the given mesh
			NonOwningPtr<shapes::Mesh> CreateMesh(const shapes::Mesh &mesh);

			//Creates a mesh by moving the given mesh
			NonOwningPtr<shapes::Mesh> CreateMesh(shapes::Mesh &&mesh);


			/*
				Meshes (derived)
				Creating
			*/

			//Creates a mesh of type T with the given arguments
			template <typename T, typename... Args,
				typename = std::enable_if_t<std::is_base_of_v<shapes::Shape, T>>>
			auto CreateMesh(Args &&...args)
			{
				static_assert(std::is_base_of_v<shapes::Mesh, T>);

				auto ptr = Create<T>(std::forward<Args>(args)...);
				return static_pointer_cast<T>(ptr);
			}


			//Creates a mesh of type T as a copy of the given mesh
			template <typename T,
				typename = std::enable_if_t<std::is_base_of_v<shapes::Shape, T>>>
			auto CreateMesh(const T &mesh_t)
			{
				static_assert(std::is_base_of_v<shapes::Mesh, T>);

				auto ptr = Create(mesh_t);
				return static_pointer_cast<T>(ptr);
			}

			//Creates a mesh of type T by moving the given mesh
			template <typename T,
				typename = std::enable_if_t<std::is_base_of_v<shapes::Shape, T>>>
			auto CreateMesh(T &&mesh_t)
			{
				static_assert(std::is_base_of_v<shapes::Mesh, T>);

				auto ptr = Create(std::move(mesh_t));
				return static_pointer_cast<T>(ptr);
			}


			/*
				Meshes
				Removing
			*/

			//Clears all meshes from this model
			void ClearMeshes() noexcept;

			//Removes a mesh from this model
			bool RemoveMesh(shapes::Mesh &mesh) noexcept;
	};
} //ion::graphics::scene

#endif