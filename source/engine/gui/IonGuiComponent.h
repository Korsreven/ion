/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui
File:	IonGuiComponent.h
-------------------------------------------
*/

#ifndef ION_GUI_COMPONENT_H
#define ION_GUI_COMPONENT_H

#include <optional>
#include <string>
#include <utility>

#include "events/IonCallback.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics::scene::graph
{
	class SceneNode;
}

namespace ion::gui
{
	//Forward declarations
	class GuiComponent;
	class GuiContainer;

	using namespace graphics::scene::graph;

	namespace gui_component::detail
	{
		bool is_descendant_of(const GuiContainer &owner, const GuiComponent &component) noexcept;
	} //gui_component::detail


	class GuiComponent : public managed::ManagedObject<GuiContainer>
	{
		friend GuiContainer;

		private:

			/*
				Helper functions
			*/

			void Detach() noexcept;

		protected:
			
			bool enabled_ = true;
			GuiComponent *parent_ = nullptr;
			NonOwningPtr<SceneNode> node_;

			std::optional<events::Callback<void, GuiComponent&>> on_enable_;
			std::optional<events::Callback<void, GuiComponent&>> on_disable_;


			/*
				Events
			*/

			//Called right after a component has been created or adopted
			virtual void Created() noexcept;

			//Called right after a component has been removed or orphaned
			virtual void Removed() noexcept;


			//Called right after a component has been enabled
			virtual void Enabled() noexcept;

			//Called right after a component has been disabled
			virtual void Disabled() noexcept;

		public:

			using managed::ManagedObject<GuiContainer>::ManagedObject;

			//Copy constructor
			GuiComponent(const GuiComponent &rhs);

			//Virtual destructor
			virtual ~GuiComponent() noexcept;


			/*
				Operators
			*/

			//Copy assignment
			inline auto& operator=(const GuiComponent &rhs)
			{
				managed::ManagedObject<GuiContainer>::operator=(rhs);

				if (this != &rhs)
					Detach();

				return *this;
			}


			/*
				Modifiers
			*/

			//Enable this component
			void Enable() noexcept;

			//Disable this component
			void Disable() noexcept;


			//Sets whether or not this component is enabled
			inline void Enabled(bool enabled) noexcept
			{
				if (enabled)
					Enable();
				else
					Disable();
			}

			//Set the parent of this component
			void Parent(GuiComponent &parent) noexcept;


			//Sets the on enable callback
			inline void OnEnable(events::Callback<void, GuiComponent&> on_enable) noexcept
			{
				on_enable_ = on_enable;
			}

			//Sets the on enable callback
			inline void OnEnable(std::nullopt_t) noexcept
			{
				on_enable_ = {};
			}


			//Sets the on disable callback
			inline void OnDisable(events::Callback<void, GuiComponent&> on_disable) noexcept
			{
				on_disable_ = on_disable;
			}

			//Sets the on disable callback
			inline void OnDisable(std::nullopt_t) noexcept
			{
				on_disable_ = {};
			}


			//Set ownership of this component
			void Owner(GuiContainer &owner) noexcept;

			//Release ownership for this component
			void Owner(std::nullptr_t) noexcept;

			//Sets the relative z-order of this component
			void ZOrder(real z) noexcept;


			/*
				Observers
			*/

			//Returns true if this component is enabled
			[[nodiscard]] inline auto IsEnabled() const noexcept
			{
				return enabled_;
			}

			//Returns a pointer to the parent of this component
			[[nodiscard]] inline auto Parent() const noexcept
			{
				return parent_;
			}

			//Returns a pointer to the node for this component
			[[nodiscard]] inline auto Node() const noexcept
			{
				return node_;
			}


			//Returns the on enable callback
			[[nodiscard]] inline auto OnEnable() const noexcept
			{
				return on_enable_;
			}

			//Returns the on disable callback
			[[nodiscard]] inline auto OnDisable() const noexcept
			{
				return on_disable_;
			}


			//Returns a pointer to the owner of this component
			[[nodiscard]] inline auto Owner() const noexcept
			{
				return owner_;
			}

			//Returns the relative z-order of this component
			[[nodiscard]] real ZOrder() const noexcept;

			//Returns true if this component is a descendant of the given owner
			[[nodiscard]] bool IsDescendantOf(const GuiContainer &owner) const noexcept;
	};
} //ion::gui

#endif