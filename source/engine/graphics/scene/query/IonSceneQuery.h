/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/query
File:	IonSceneQuery.h
-------------------------------------------
*/

#ifndef ION_SCENE_QUERY
#define ION_SCENE_QUERY

#include <optional>
#include <vector>

#include "events/IonListenable.h"
#include "graphics/scene/graph/IonSceneGraph.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/render/IonFog.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::query
{
	using namespace graphics::scene::graph;

	namespace scene_query
	{
		struct QueryType
		{
			static constexpr uint32 Movable = 1 << 0;
			static constexpr uint32 Drawable = 1 << 1;

			static constexpr uint32 Animation = 1 << 2;
			static constexpr uint32 Camera = 1 << 3;
			static constexpr uint32 Light = 1 << 4;
			static constexpr uint32 Model = 1 << 5;
			static constexpr uint32 ParticleSystem = 1 << 6;
			static constexpr uint32 Sound = 1 << 7;
			static constexpr uint32 SoundListener = 1 << 8;
			static constexpr uint32 Text = 1 << 9;
		};


		namespace detail
		{
		} //detail
	} //scene_query


	class SceneQuery
	{
		protected:
		
			std::optional<uint32> query_mask_;
			std::optional<uint32> query_type_mask_ = scene_query::QueryType::Model;
			NonOwningPtr<SceneGraph> scene_graph_;

		public:

			//Default constructor
			SceneQuery() = default;

			//Construct a new scene query with the given scene graph
			SceneQuery(NonOwningPtr<SceneGraph> scene_graph) noexcept;


			/*
				Modifiers
			*/

			//Sets the query mask for this scene query to the given mask
			//This scene query will only query objects if a bitwise AND operation between the query mask and the object query flags is non-zero
			//The meaning of the bits is user-specific
			inline void QueryMask(std::optional<uint32> mask) noexcept
			{
				query_mask_ = mask;
			}

			//Adds the given mask to the already existing query mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query mask and the object query flags is non-zero
			//The meaning of the bits is user-specific
			inline void AddQueryMask(uint32 mask) noexcept
			{
				if (query_mask_)
					*query_mask_ |= mask;
				else
					query_mask_ = mask;
			}

			//Removes the given mask to the already existing query mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query mask and the object query flags is non-zero
			//The meaning of the bits is user-specific
			inline void RemoveQueryMask(uint32 mask) noexcept
			{
				if (query_mask_)
					*query_mask_ &= ~mask;
			}


			//Sets the query type mask for this scene query to the given mask
			//This scene query will only query objects if a bitwise AND operation between the query type mask and the object type is non-zero
			inline void QueryTypeMask(std::optional<uint32> mask) noexcept
			{
				query_type_mask_ = mask;
			}

			//Adds the given mask to the already existing query type mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query type mask and the object type is non-zero
			inline void AddQueryTypeMask(uint32 mask) noexcept
			{
				if (query_type_mask_)
					*query_type_mask_ |= mask;
				else
					query_type_mask_ = mask;
			}

			//Removes the given mask to the already existing query type mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query type mask and the object type is non-zero
			inline void RemoveQueryTypeMask(uint32 mask) noexcept
			{
				if (query_type_mask_)
					*query_type_mask_ &= ~mask;
			}


			//Sets the scene graph this scene query is querying
			inline void Scene(NonOwningPtr<SceneGraph> scene_graph) noexcept
			{
				scene_graph_ = scene_graph;
			}


			/*
				Observers
			*/

			//Returns the query mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query mask and the query flags is non-zero
			//The meaning of the bits is user-specific
			[[nodiscard]] inline auto QueryMask() const noexcept
			{
				return query_mask_;
			}

			//Returns the query type mask for this scene query
			//This scene query will only query objects if a bitwise AND operation between the query type mask and the object type is non-zero
			[[nodiscard]] inline auto QueryTypeMask() const noexcept
			{
				return query_type_mask_;
			}


			//Returns the scene graph this scene query is querying
			[[nodiscard]] inline auto Scene() const noexcept
			{
				return scene_graph_;
			}
	};
} //ion::graphics::scene::query

#endif