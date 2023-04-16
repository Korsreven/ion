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
#include "graphics/utilities/IonVector2.h"
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
	using namespace graphics::utilities;

	namespace gui_component::detail
	{
		bool is_descendant_of(const GuiContainer &owner, const GuiComponent &component) noexcept;
	} //gui_component::detail


	///@brief A class representing a general GUI component, the base class of all GUI objects
	///@details The owner and parent of a component is usually the same, but they can differ.
	///A component is always relative to its parent component and can be enabled/disabled and shown/hidden
	class GuiComponent : public managed::ManagedObject<GuiContainer>
	{
		friend GuiContainer;

		private:

			/**
				@name Helper functions
				@{
			*/

			void Detach() noexcept;

			///@}

		protected:
			
			bool enabled_ = true;
			bool visible_ = true;
			types::SuppressMove<GuiComponent*> parent_ = nullptr;
			NonOwningPtr<SceneNode> node_;

			std::optional<events::Callback<void, GuiComponent&>> on_enable_;
			std::optional<events::Callback<void, GuiComponent&>> on_disable_;
			std::optional<events::Callback<void, GuiComponent&>> on_show_;
			std::optional<events::Callback<void, GuiComponent&>> on_hide_;


			/**
				@name Events
				@{
			*/

			///@brief Called right after a component has been created or adopted
			virtual void Created() noexcept;

			///@brief Called right after a component has been removed or orphaned
			virtual void Removed() noexcept;


			///@brief Called right after a component has been enabled
			virtual void Enabled() noexcept;

			///@brief Called right after a component has been disabled
			virtual void Disabled() noexcept;


			///@brief Called right after a component has been shown
			virtual void Shown() noexcept;

			///@brief Called right after a component has been hidden
			virtual void Hidden() noexcept;

			///@}

		public:

			using managed::ManagedObject<GuiContainer>::ManagedObject;

			///@brief Default copy constructor
			GuiComponent(const GuiComponent&) = default;

			///@brief Default move constructor
			GuiComponent(GuiComponent&&) = default;

			///@brief Virtual destructor
			virtual ~GuiComponent() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Default copy assignment
			inline GuiComponent& operator=(const GuiComponent&) = default;

			///@brief Default move assignment
			inline GuiComponent& operator=(GuiComponent&&) = default;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Enables this component
			void Enable() noexcept;

			///@brief Disables this component
			void Disable() noexcept;


			///@brief Shows this component
			void Show() noexcept;

			///@brief Hides this component
			void Hide() noexcept;


			///@brief Sets whether or not this component is enabled
			inline void Enabled(bool enabled) noexcept
			{
				if (enabled)
					Enable();
				else
					Disable();
			}

			///@brief Sets whether or not this component is visible
			inline void Visible(bool visible) noexcept
			{
				if (visible)
					Show();
				else
					Hide();
			}


			///@brief Sets the parent of this component
			void Parent(GuiComponent &parent) noexcept;


			///@brief Sets the on enable callback
			inline void OnEnable(events::Callback<void, GuiComponent&> on_enable) noexcept
			{
				on_enable_ = on_enable;
			}

			///@brief Sets the on enable callback
			inline void OnEnable(std::nullopt_t) noexcept
			{
				on_enable_ = {};
			}


			///@brief Sets the on disable callback
			inline void OnDisable(events::Callback<void, GuiComponent&> on_disable) noexcept
			{
				on_disable_ = on_disable;
			}

			///@brief Sets the on disable callback
			inline void OnDisable(std::nullopt_t) noexcept
			{
				on_disable_ = {};
			}


			///@brief Sets the on show callback
			inline void OnShow(events::Callback<void, GuiComponent&> on_show) noexcept
			{
				on_show_ = on_show;
			}

			///@brief Sets the on show callback
			inline void OnShow(std::nullopt_t) noexcept
			{
				on_show_ = {};
			}


			///@brief Sets the on hide callback
			inline void OnHide(events::Callback<void, GuiComponent&> on_hide) noexcept
			{
				on_hide_ = on_hide;
			}

			///@brief Sets the on hide callback
			inline void OnHide(std::nullopt_t) noexcept
			{
				on_hide_ = {};
			}


			///@brief Sets ownership of this component
			void Owner(GuiContainer &owner) noexcept;

			///@brief Releases ownership for this component
			void Owner(std::nullptr_t) noexcept;

			///@brief Sets the local position of this component
			void Position(const Vector2 &position) noexcept;

			///@brief Sets the global position of this component
			void GlobalPosition(const Vector2 &position) noexcept;

			///@brief Sets the local z-order of this component
			void ZOrder(real z) noexcept;

			///@brief Sets the global z-order of this component
			void GlobalZOrder(real z) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns true if this component is enabled
			[[nodiscard]] inline auto IsEnabled() const noexcept
			{
				return enabled_;
			}

			///@brief Returns true if this component is visible
			[[nodiscard]] inline auto IsVisible() const noexcept
			{
				return visible_;
			}


			///@brief Returns a pointer to the parent of this component
			[[nodiscard]] inline auto Parent() const noexcept
			{
				return parent_.Get();
			}

			///@brief Returns a pointer to the node for this component
			[[nodiscard]] inline auto Node() const noexcept
			{
				return node_;
			}


			///@brief Returns the on enable callback
			[[nodiscard]] inline auto OnEnable() const noexcept
			{
				return on_enable_;
			}

			///@brief Returns the on disable callback
			[[nodiscard]] inline auto OnDisable() const noexcept
			{
				return on_disable_;
			}


			///@brief Returns the on show callback
			[[nodiscard]] inline auto OnShow() const noexcept
			{
				return on_show_;
			}

			///@brief Returns the on hide callback
			[[nodiscard]] inline auto OnHide() const noexcept
			{
				return on_hide_;
			}


			///@brief Returns a pointer to the owner of this component
			[[nodiscard]] inline auto Owner() const noexcept
			{
				return owner_;
			}

			///@brief Returns the local position of this component
			[[nodiscard]] Vector2 Position() const noexcept;

			///@brief Returns the global position of this component
			[[nodiscard]] Vector2 GlobalPosition() const noexcept;

			///@brief Returns the local z-order of this component
			[[nodiscard]] real ZOrder() const noexcept;

			///@brief Returns the global z-order of this component
			[[nodiscard]] real GlobalZOrder() const noexcept;

			///@brief Returns true if this component is a descendant of the given owner
			[[nodiscard]] bool IsDescendantOf(const GuiContainer &owner) const noexcept;

			///@}
	};
} //ion::gui

#endif