/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiLabel.h
-------------------------------------------
*/

#ifndef ION_GUI_LABEL_H
#define ION_GUI_LABEL_H

#include <optional>
#include <string>
#include <utility>

#include "IonGuiControl.h"
#include "graphics/utilities/IonVector2.h"
#include "memory/IonOwningPtr.h"

namespace ion::gui::controls
{
	using namespace graphics::utilities;

	namespace gui_label
	{
		struct LabelSkin : gui_control::ControlSkin
		{
			//Empty
		};


		namespace detail
		{
		} //detail
	} //gui_label


	///@brief A class representing a GUI label with a caption
	class GuiLabel : public GuiControl
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			bool auto_size_ = false;
			std::optional<Vector2> min_size_;
			std::optional<Vector2> max_size_;


			/**
				@name Skins
				@{
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;

			virtual void UpdateCaption() noexcept override;

			///@}

		public:

			///@brief Constructs a label with the given name, size, caption and hit boxes
			GuiLabel(std::string name, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes = {}) noexcept;

			///@brief Constructs a label with the given name, skin, size, caption and hit boxes
			GuiLabel(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes = {});


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets whether or not this label should automatically adjust its size
			inline void AutoSize(bool auto_size) noexcept
			{
				if (auto_size_ != auto_size)
				{
					auto_size_ = auto_size;

					if (Caption())
						UpdateCaption();
				}
			}

			///@brief Sets the auto size constraints for this label to the given min and max size
			inline void AutoSizeConstraints(std::optional<Vector2> min_size, std::optional<Vector2> max_size) noexcept
			{
				if (min_size && max_size)
				{
					auto [min, max] = std::minmax(*min_size, *max_size);
					min_size = min;
					max_size = max;
				}

				if (min_size_ != min_size || max_size_ != max_size)
				{
					min_size_ = min_size;
					max_size_ = max_size;

					if (Caption())
						UpdateCaption();
				}
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns whether or not this label should automatically adjust its size
			[[nodiscard]] inline auto AutoSize() const noexcept
			{
				return auto_size_;
			}

			///@brief Returns the auto size constraints for this label
			[[nodiscard]] inline auto AutoSizeConstraints() const noexcept
			{
				return std::pair{min_size_, max_size_};
			}

			///@}
	};

} //ion::gui::controls

#endif