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
#include "types/IonTypes.h"

namespace ion::graphics::scene
{
	using utilities::Aabb;
	using utilities::Obb;
	using utilities::Sphere;

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
			using mesh_container_type = std::vector<render::Mesh>;
		} //detail
	} //model


	class Model final
	{
		private:

			model::ModelBufferUsage buffer_usage_ = model::ModelBufferUsage::Static;
			bool visible_ = true;

			std::optional<int> vbo_handle_;

			std::optional<Aabb> aabb_;
			std::optional<Obb> obb_;
			std::optional<Sphere> sphere_;

			model::detail::mesh_container_type meshes_;
			render::mesh::detail::vertex_storage_type vertex_buffer_;
			bool reload_vertex_buffer_ = false;

		public:
	};
} //ion::graphics::scene

#endif