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

#include "graphics/render/IonMesh.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "graphics/utilities/IonVector2.h"
#include "types/IonTypes.h"
#include "unmanaged/IonObjectFactory.h"

namespace ion::graphics::scene
{
	using utilities::Aabb;
	using utilities::Obb;
	using utilities::Sphere;
	using utilities::Vector2;

	namespace model
	{
		enum class ModelBufferUsage
		{
			Static,		//Load/modify vertex data once
			Dynamic,	//Load/modify vertex data occasionally
			Stream		//Load/modify vertex data often (every frame)
		};

		namespace detail
		{
			int model_buffer_usage_to_gl_buffer_usage(ModelBufferUsage buffer_usage) noexcept;
			ModelBufferUsage gl_buffer_usage_to_model_buffer_usage(int buffer_usage) noexcept;


			/*
				Graphics API
			*/

			std::optional<int> create_vertex_buffer_object() noexcept;
			void delete_vertex_buffer_object(int vbo_handle) noexcept;

			void set_vertex_buffer_data(int vbo_handle, ModelBufferUsage buffer_usage,
				const render::mesh::detail::vertex_storage_type &vertex_buffer) noexcept;
			int get_vertex_buffer_size(int vbo_handle) noexcept;
			ModelBufferUsage get_vertex_buffer_usage(int vbo_handle) noexcept;
		} //detail
	} //model


	class Model final : protected unmanaged::ObjectFactory<render::Mesh>
	{
		private:

			model::ModelBufferUsage buffer_usage_ = model::ModelBufferUsage::Static;
			bool visible_ = true;

			std::optional<int> vbo_handle_;

			std::optional<Aabb> aabb_;
			std::optional<Obb> obb_;
			std::optional<Sphere> sphere_;

			render::mesh::detail::vertex_storage_type vertex_buffer_;
			bool reload_vertex_buffer_ = false;


			void FetchVertexData();

		public:

			//Default constructor
			Model() = default;

			//Construct a new model with the given vertex buffer usage pattern and visibility
			explicit Model(model::ModelBufferUsage buffer_usage, bool visible = true) noexcept;

			//Destructor
			~Model() noexcept;


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
			[[nodiscard]] inline const auto Meshes() const noexcept
			{
				return Objects();
			}
			

			/*
				Modifiers
			*/

			//Sets the vertex buffer usage pattern for this model to the given value
			inline void BufferUsage(model::ModelBufferUsage buffer_usage) noexcept
			{
				if (buffer_usage_ != buffer_usage)
				{
					buffer_usage_ = buffer_usage;
					reload_vertex_buffer_ = !!vbo_handle_;
				}
			}

			//Sets the visibility of this model to the given value
			inline void Visible(bool visible) noexcept
			{
				visible_ = visible;
			}


			/*
				Observers
			*/

			//Returns the vertex buffer usage pattern for this model
			[[nodiscard]] inline auto BufferUsage() const noexcept
			{
				return buffer_usage_;
			}

			//Returns true if this model is visible
			[[nodiscard]] inline auto Visible() const noexcept
			{
				return visible_;
			}


			//Return the VBO handle this model uses
			[[nodiscard]] inline auto VboHandle() const noexcept
			{
				return vbo_handle_;
			}


			/*
				Drawing
			*/

			//Prepare this model such that it is ready to be drawn
			//This is called once regardless of passes
			void Prepare() noexcept;

			//Draw this model with the given shader program (optional)
			//This can be called multiple times if more than one pass
			void Draw(shaders::ShaderProgram *shader_program = nullptr) noexcept;


			/*
				Meshes
				Creating
			*/

			//Create a mesh with the given vertices and whether or not tex coords should automatically be generated
			//If auto generate tex coords is true, user specified vertex tex coords will be ignored
			render::Mesh& CreateMesh(const render::mesh::Vertices &vertices, bool auto_generate_tex_coords = false);

			//Create a mesh with the given vertices, lower left and upper right tex coords
			//Tex coords are auto generated in range [lower_left_tex_coords, upper_right_tex_coords]
			//User specified vertex tex coords will be ignored
			render::Mesh& CreateMesh(const render::mesh::Vertices &vertices,
				const Vector2 &lower_left_tex_coords, const Vector2 &upper_right_tex_coords);

			//Create a mesh with the given raw vertex data and whether or not tex coords should automatically be generated
			//If auto generate tex coords is true, user specified vertex tex coords will be ignored
			render::Mesh& CreateMesh(render::mesh::detail::vertex_storage_type vertex_data, bool auto_generate_tex_coords = false);

			//Create a mesh with the given raw vertex data
			//Tex coords are auto generated in range [lower_left_tex_coords, upper_right_tex_coords]
			//User specified vertex tex coords will be ignored
			render::Mesh& CreateMesh(render::mesh::detail::vertex_storage_type vertex_data,
				const Vector2 &lower_left_tex_coords, const Vector2 &upper_right_tex_coords);


			//Create a mesh as a copy of the given mesh
			render::Mesh& CreateMesh(const render::Mesh &mesh);

			//Create a mesh by moving the given mesh
			render::Mesh& CreateMesh(render::Mesh &&mesh);


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