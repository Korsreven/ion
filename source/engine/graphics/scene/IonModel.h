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
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "IonDrawableObject.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/render/vertex/IonVertexBufferObject.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"
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

	namespace shaders
	{
		class ShaderProgram;
	}
}

namespace ion::graphics::scene
{
	class Light; //Forward declaration

	namespace model::detail
	{
		using mesh_pointers = std::vector<shapes::Mesh*>;
		using emissive_mesh = std::pair<shapes::Mesh*,Light>;
		using emissive_meshes = std::vector<emissive_mesh>;

		struct mesh_vertex_stream final
		{
			shapes::mesh::VertexContainer vertex_data;
			render::vertex::VertexBatch vertex_batch;
			mesh_pointers meshes;
			bool reload_vertex_data = false;

			mesh_vertex_stream(shapes::mesh::VertexContainer vertex_data, render::vertex::vertex_batch::VertexDrawMode draw_mode,
				NonOwningPtr<materials::Material> material = nullptr);
		};

		using mesh_vertex_streams = std::vector<mesh_vertex_stream>;

		struct mesh_group_comparator final
		{
			bool operator()(const shapes::Mesh &mesh, const shapes::Mesh &mesh2) const noexcept;
		};


		int get_mesh_vertex_count(const mesh_vertex_streams &mesh_streams) noexcept;
		mesh_vertex_stream* get_mesh_stream(const shapes::Mesh &mesh, mesh_vertex_streams &mesh_streams) noexcept;

		emissive_mesh* get_emissive_mesh(const shapes::Mesh &mesh, emissive_meshes &meshes) noexcept;
		Light get_emissive_light(const shapes::Mesh &mesh) noexcept;
		void add_emissive_mesh(shapes::Mesh &mesh, emissive_meshes &meshes);
		void remove_emissive_mesh(const shapes::Mesh &mesh, emissive_meshes &meshes) noexcept;
		void update_emissive_mesh(emissive_mesh &em_mesh) noexcept;
		bool is_mesh_emissive(const shapes::Mesh &mesh) noexcept;
	} //model::detail


	class Model final :
		public DrawableObject,
		public managed::ObjectManager<shapes::Mesh, Model>
	{
		private:

			model::detail::mesh_vertex_streams mesh_vertex_streams_;
			model::detail::emissive_meshes emissive_meshes_;
			std::optional<render::vertex::VertexBufferObject> vbo_;

			bool reload_vertex_streams_ = false;
			bool reload_vertex_buffer_ = false;
			bool update_bounding_volumes_ = false;
			mutable bool update_emissive_lights_ = false;


			void ReloadVertexStreams();
			void PrepareVertexStreams();

		protected:

			/*
				Events
			*/

			//See ObjectManager<T>::Created for more details
			void Created(shapes::Mesh &mesh) noexcept override;

			//See ObjectManager<T>::removed for more details
			void Removed(shapes::Mesh &mesh) noexcept override;


			void OpacityChanged() noexcept override;

		public:

			//Construct a new model with the given name and visibility
			explicit Model(std::optional<std::string> name = {}, bool visible = true) noexcept;


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

			//Returns the vertex buffer this model uses
			[[nodiscard]] inline auto& VertexBuffer() const noexcept
			{
				return vbo_;
			}


			/*
				Notifying
			*/

			//Called when there are changes to the given mesh that requires a reload of one stream
			void NotifyMeshReload(const shapes::Mesh &mesh) noexcept;

			//Called when there are changes to the given mesh that requires a reload of all streams
			void NotifyMeshReloadAll(const shapes::Mesh &mesh) noexcept;


			/*
				Rendering
			*/

			//Returns all emissive lights in this model
			[[nodiscard]] const movable_object::Lights& EmissiveLights(bool derive = true) const;


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
				return static_pointer_cast<T>(ptr);
			}


			//Create a mesh of type T as a copy of the given mesh
			template <typename T,
				typename = std::enable_if_t<std::is_base_of_v<shapes::Shape, T>>>
			auto CreateMesh(const T &mesh_t)
			{
				static_assert(std::is_base_of_v<shapes::Mesh, T>);

				auto ptr = Create(mesh_t);
				return static_pointer_cast<T>(ptr);
			}

			//Create a mesh of type T by moving the given mesh
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

			//Clear all meshes from this model
			void ClearMeshes() noexcept;

			//Remove a mesh from this model
			bool RemoveMesh(shapes::Mesh &mesh) noexcept;
	};
} //ion::graphics::scene

#endif