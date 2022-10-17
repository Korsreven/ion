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
#include "types/IonSuppressMove.h"
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


	//A class representing a general GUI component, the base class of all GUI objects
	//The owner and parent of a component is usually the same, but they can differ
	//A component is always relative to its parent component and can be enabled/disabled and shown/hidden
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
			bool visible_ = true;
			types::SuppressMove<GuiComponent*> parent_ = nullptr;
			NonOwningPtr<SceneNode> node_;

			std::optional<events::Callback<void, GuiComponent&>> on_enable_;
			std::optional<events::Callback<void, GuiComponent&>> on_disable_;
			std::optional<events::Callback<void, GuiComponent&>> on_show_;
			std::optional<events::Callback<void, GuiComponent&>> on_hide_;


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


			//Called right after a component has been shown
			virtual void Shown() noexcept;

			//Called right after a component has been hidden
			virtual void Hidden() noexcept;

		public:

			using managed::ManagedObject<GuiContainer>::ManagedObject;

			//Default copy constructor
			GuiComponent(const GuiComponent&) = default;

			//Default move constructor
			GuiComponent(GuiComponent&&) = default;

			//Virtual destructor
			virtual ~GuiComponent() noexcept;


			/*
				Operators
			*/

			//Default copy assignment
			inline GuiComponent& operator=(const GuiComponent&) = default;

			//Default move assignment
			inline GuiComponent& operator=(GuiComponent&&) = default;


			/*
				Modifiers
			*/

			//Enable this component
			void Enable() noexcept;

			//Disable this component
			void Disable() noexcept;


			//Show this component
			void Show() noexcept;

			//Hide this component
			void Hide() noexcept;


			//Sets whether or not this component is enabled
			inline void Enabled(bool enabled) noexcept
			{
				if (enabled)
					Enable();
				else
					Disable();
			}

			//Sets whether or not this component is visible
			inline void Visible(bool visible) noexcept
			{
				if (visible)
					Show();
				else
					Hide();
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


			//Sets the on show callback
			inline void OnShow(events::Callback<void, GuiComponent&> on_show) noexcept
			{
				on_show_ = on_show;
			}

			//Sets the on show callback
			inline void OnShow(std::nullopt_t) noexcept
			{
				on_show_ = {};
			}


			//Sets the on hide callback
			inline void OnHide(events::Callback<void, GuiComponent&> on_hide) noexcept
			{
				on_hide_ = on_hide;
			}

			//Sets the on hide callback
			inline void OnHide(std::nullopt_t) noexcept
			{
				on_hide_ = {};
			}


			//Set ownership of this component
			void Owner(GuiContainer &owner) noexcept;

			//Release ownership for this component
			void Owner(std::nullptr_t) noexcept;

			//Sets the local z-order of this component
			void ZOrder(real z) noexcept;

			//Sets the global z-order of this component
			void GlobalZOrder(real z) noexcept;


			/*
				Observers
			*/

			//Returns true if this component is enabled
			[[nodiscard]] inline auto IsEnabled() const noexcept
			{
				return enabled_;
			}

			//Returns true if this component is visible
			[[nodiscard]] inline auto IsVisible() const noexcept
			{
				return visible_;
			}


			//Returns a pointer to the parent of this component
			[[nodiscard]] inline auto Parent() const noexcept
			{
				return parent_.Get();
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


			//Returns the on show callback
			[[nodiscard]] inline auto OnShow() const noexcept
			{
				return on_show_;
			}

			//Returns the on hide callback
			[[nodiscard]] inline auto OnHide() const noexcept
			{
				return on_hide_;
			}


			//Returns a pointer to the owner of this component
			[[nodiscard]] inline auto Owner() const noexcept
			{
				return owner_;
			}

			//Returns the local z-order of this component
			[[nodiscard]] real ZOrder() const noexcept;

			//Returns the global z-order of this component
			[[nodiscard]] real GlobalZOrder() const noexcept;

			//Returns true if this component is a descendant of the given owner
			[[nodiscard]] bool IsDescendantOf(const GuiContainer &owner) const noexcept;
	};
} //ion::gui

#endif