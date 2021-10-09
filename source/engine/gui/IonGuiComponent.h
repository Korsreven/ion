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

#include <string>
#include <utility>

#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"

//Forward declarations
namespace ion::graphics::scene::graph
{
	class SceneNode;
}

namespace ion::gui
{
	using namespace graphics::scene::graph;
	struct GuiContainer; //Forward declaration

	namespace gui_component::detail
	{
	} //gui_component::detail


	class GuiComponent : public managed::ManagedObject<GuiContainer>
	{
		private:

			/*
				Helper functions
			*/

			void Detach() noexcept;

		protected:
			
			bool enabled_ = true;
			GuiComponent *parent_ = nullptr;
			NonOwningPtr<SceneNode> node_;		


			/*
				Events
			*/

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

			//Sets whether or not this component is enabled
			inline void Enabled(bool enabled) noexcept
			{
				if (enabled_ != enabled)
				{
					if ((enabled_ = enabled) == true) //Suppress W4706
						Enabled();
					else
						Disabled();
				}
			}

			//Set the parent of this component
			void Parent(GuiComponent &parent) noexcept;


			//Set ownership of this component
			void Owner(GuiContainer &owner) noexcept;

			//Release ownership for this component
			void Release() noexcept;


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


			//Returns a pointer to the owner of this component
			[[nodiscard]] inline auto Owner() const noexcept
			{
				return owner_;
			}
	};
} //ion::gui

#endif