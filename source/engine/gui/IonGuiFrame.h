/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiFrame.h
-------------------------------------------
*/

#ifndef ION_GUI_FRAME_H
#define ION_GUI_FRAME_H

#include <string>
#include <string_view>
#include <type_traits>

#include "IonGuiContainer.h"
#include "IonGuiPanel.h"
#include "managed/IonManagedObject.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::gui
{
	class GuiController; //Forward declaration

	namespace gui_frame::detail
	{
	} //gui_frame::detail


	class GuiFrame :
		public GuiContainer,
		public managed::ManagedObject<GuiController>
	{
		private:



		public:

			//Construct a frame with the given name
			explicit GuiFrame(std::string name);

			//Construct a frame with the given owner and name
			GuiFrame(GuiController &owner, std::string name);


			/*
				Ranges
			*/

			//Returns a mutable range of all containers in this frame
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Containers() noexcept
			{
				return Objects();
			}

			//Returns an immutable range of all containers in this frame
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Containers() const noexcept
			{
				return Objects();
			}


			/*
				Modifiers
			*/

			//Set ownership of this frame to the given owner
			inline void Owner(GuiController &owner) noexcept
			{
				managed::ManagedObject<GuiController>::Owner(owner);
			}

			//Release ownership for this frame
			inline void Release() noexcept
			{
				managed::ManagedObject<GuiController>::Release();
			}


			/*
				Observers
			*/

			//Returns the name of this frame
			[[nodiscard]] inline auto& Name() const noexcept
			{
				return managed::ManagedObject<GuiController>::name_;
			}

			//Returns a pointer to the owner of this frame
			[[nodiscard]] inline auto Owner() const noexcept
			{
				return managed::ManagedObject<GuiController>::owner_;
			}


			/*
				Container (derived)
				Creating
			*/

			//Create a container of type T with the given arguments
			template <typename T, typename... Args,
				typename = std::enable_if_t<std::is_base_of_v<GuiContainer, T> && !std::is_same_v<GuiFrame, T>>>
			auto CreateContainer(Args &&...args)
			{
				static_assert(std::is_base_of_v<GuiContainer, T> && !std::is_same_v<GuiFrame, T>);

				auto ptr = Create<T>(std::forward<Args>(args)...);
				return static_pointer_cast<T>(ptr);
			}

			//Create a container of type T by moving the given container
			template <typename T,
				typename = std::enable_if_t<std::is_base_of_v<GuiContainer, T> && !std::is_same_v<GuiFrame, T>>>
			auto CreateMesh(T &&container_t)
			{
				static_assert(std::is_base_of_v<GuiContainer, T> && !std::is_same_v<GuiFrame, T>);

				auto ptr = Create(std::move(container_t));
				return static_pointer_cast<T>(ptr);
			}


			/*
				Containers
				Retrieving
			*/

			//Gets a pointer to a mutable container with the given name
			//Returns nullptr if container could not be found
			[[nodiscard]] NonOwningPtr<GuiContainer> GetContainer(std::string_view name) noexcept;

			//Gets a pointer to an immutable container with the given name
			//Returns nullptr if container could not be found
			[[nodiscard]] NonOwningPtr<const GuiContainer> GetContainer(std::string_view name) const noexcept;


			/*
				Containers
				Removing
			*/

			//Clear all containers from this frame
			void ClearContainers() noexcept;

			//Remove a container from this frame
			bool RemoveContainer(GuiContainer &container) noexcept;

			//Remove a removable container with the given name from this frame
			bool RemoveContainer(std::string_view name) noexcept;
	};
} //ion::gui

#endif