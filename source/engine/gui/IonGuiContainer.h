/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiContainer.h
-------------------------------------------
*/

#ifndef ION_GUI_CONTAINER_H
#define ION_GUI_CONTAINER_H

#include <string>
#include <string_view>
#include <type_traits>

#include "IonGuiComponent.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::gui
{
	namespace gui_container::detail
	{
	} //gui_container::detail


	///@brief A class representing an owning GUI container that can create and store multiple GUI components
	///@details A container is itself a component, and is therefore both the owner and the parent of a newly create component
	class GuiContainer :
		public GuiComponent,
		public managed::ObjectManager<GuiComponent, GuiContainer>
	{
		protected:

			/**
				@name Events
				@{
			*/

			///@brief See ObjectManager::Created for more details
			virtual void Created(GuiComponent &component) noexcept override;

			///@brief See ObjectManager::Removed for more details
			virtual void Removed(GuiComponent &component) noexcept override;

			///@}

		public:

			using GuiComponent::GuiComponent;

			///@brief Deleted copy constructor
			GuiContainer(const GuiContainer&) = delete;

			///@brief Default move constructor
			GuiContainer(GuiContainer&&) = default;


			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all components in this container
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Components() noexcept
			{
				return Objects();
			}

			///@brief Returns an immutable range of all components in this container
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Components() const noexcept
			{
				return Objects();
			}

			///@}

			/**
				@name Components - Creating
				@{
			*/

			///@brief Creates a component of type T with the given name and arguments
			template <typename T, typename... Args>
			auto CreateComponent(std::string name, Args &&...args)
			{
				static_assert(std::is_base_of_v<GuiComponent, T>);

				auto ptr = Create<T>(std::move(name), std::forward<Args>(args)...);
				return static_pointer_cast<T>(ptr);
			}


			///@brief Creates a component of type T as a copy of the given component
			template <typename T>
			auto CreateComponent(const T &component_t)
			{
				static_assert(std::is_base_of_v<GuiComponent, T>);

				auto ptr = Create(component_t);
				return static_pointer_cast<T>(ptr);
			}

			///@brief Creates a component of type T by moving the given component
			template <typename T>
			auto CreateComponent(T &&component_t)
			{
				static_assert(std::is_base_of_v<GuiComponent, T>);

				auto ptr = Create(std::move(component_t));
				return static_pointer_cast<T>(ptr);
			}

			///@}

			/**
				@name Components - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable component with the given name
			///@details Returns nullptr if component could not be found
			[[nodiscard]] NonOwningPtr<GuiComponent> GetComponent(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable component with the given name
			///@details Returns nullptr if component could not be found
			[[nodiscard]] NonOwningPtr<const GuiComponent> GetComponent(std::string_view name) const noexcept;


			///@brief Gets a pointer to a mutable component of type T with the given name
			///@details Returns nullptr if a component of type T could not be found
			template <typename T>
			[[nodiscard]] auto GetComponentAs(std::string_view name) noexcept
			{
				static_assert(std::is_base_of_v<GuiComponent, T>);
				return dynamic_pointer_cast<T>(GetComponent(name));
			}

			///@brief Gets a pointer to an immutable component of type T with the given name
			///@details Returns nullptr if a component of type T could not be found
			template <typename T>
			[[nodiscard]] auto GetComponentAs(std::string_view name) const noexcept
			{
				static_assert(std::is_base_of_v<GuiComponent, T>);
				return dynamic_pointer_cast<const T>(GetComponent(name));
			}

			///@}

			/**
				@name Components - Removing
				@{
			*/

			///@brief Clears all removable components from this container
			void ClearComponents() noexcept;

			///@brief Removes a removable component from this container
			bool RemoveComponent(GuiComponent &component) noexcept;

			///@brief Removes a removable component with the given name from this container
			bool RemoveComponent(std::string_view name) noexcept;

			///@}
	};
} //ion::gui

#endif