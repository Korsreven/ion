/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events/listeners
File:	IonListenerTraits.h
-------------------------------------------
*/

#ifndef ION_LISTENER_TRAITS_H
#define ION_LISTENER_TRAITS_H

#include <type_traits>

#include "IonManagedObjectListener.h"
#include "IonResourceListener.h"
#include "resources/IonResource.h"

//Forward declarations
namespace ion::graphics
{
	namespace render
	{
		class Viewport;
	} //render

	namespace scene
	{
		class Camera;
	} //scene
} //ion::graphics

namespace ion::events::listeners
{
	//Forward declarations
	struct CameraListener;
	struct ViewportListener;

	namespace detail
	{
		template <typename ObjectT, typename OwnerT>
		struct listener_of_impl
		{
			using type = std::conditional_t<std::is_base_of_v<resources::Resource<OwnerT>, ObjectT>,
				ResourceListener<ObjectT, OwnerT>,
				ManagedObjectListener<ObjectT, OwnerT>>;
		};

		template <typename OwnerT>
		struct listener_of_impl<graphics::scene::Camera, OwnerT>
		{
			using type = CameraListener;
		};

		template <typename OwnerT>
		struct listener_of_impl<graphics::render::Viewport, OwnerT>
		{
			using type = ViewportListener;
		};
	} //detail


	template <typename ObjectT, typename OwnerT>
	struct listener_of : detail::listener_of_impl<ObjectT, OwnerT>
	{
	};

	template <typename ObjectT, typename OwnerT>
	using listener_of_t = typename listener_of<ObjectT, OwnerT>::type;
} //ion::events::listeners

#endif