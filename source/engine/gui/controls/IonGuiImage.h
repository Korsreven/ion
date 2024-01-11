/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiImage.h
-------------------------------------------
*/

#ifndef ION_GUI_IMAGE_H
#define ION_GUI_IMAGE_H

#include <optional>
#include <string>
#include <tuple>

#include "IonGuiControl.h"
#include "graphics/materials/IonMaterial.h"
#include "graphics/utilities/IonColor.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::gui::controls
{
	namespace gui_image
	{
		enum class ImageMode : bool
		{
			Fill,	
			Fit
		};

		struct ImageSkin : gui_control::ControlSkin
		{
			gui_control::ControlSkinPart Image;

			//Copy from most derived
			virtual void Assign(const ControlSkin &control_skin) noexcept
			{
				if (auto skin = dynamic_cast<const ImageSkin*>(&control_skin); skin)
					*this = *skin;
				else //Try less derived
					return ControlSkin::Assign(control_skin);
			}

			virtual void GetParts(gui_control::SkinPartPointers &parts, std::string_view name) const override;
		};


		namespace detail
		{
			/**
				@name Skins
				@{
			*/

			void resize_image(gui_control::ControlSkinPart &image, const Vector2 &delta_size) noexcept;
			void resize_skin(ImageSkin &skin, const Vector2 &from_size, const Vector2 &to_size, ImageMode mode) noexcept;

			bool has_center_area(const gui_control::ControlSkin &skin) noexcept;

			///@}
		} //detail
	} //gui_image


	///@brief A class representing a GUI image
	class GuiImage : public GuiControl
	{
		private:

			void DefaultSetup() noexcept;

		protected:

			std::optional<graphics::utilities::Color> color_;
			graphics::utilities::Color skin_color_;
			gui_image::ImageMode mode_ = gui_image::ImageMode::Fill;


			/**
				@name Events
				@{
			*/

			///@brief See GuiControl::Resized for more details
			virtual void Resized(Vector2 from_size, Vector2 to_size) noexcept override;

			///@}

			/**
				@name States
				@{
			*/

			void SetSkinState(gui_control::ControlState state, gui_image::ImageSkin &skin) noexcept;
			virtual void SetState(gui_control::ControlState state) noexcept override;

			///@}

			/**
				@name Skins
				@{
			*/

			virtual OwningPtr<gui_control::ControlSkin> AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const override;
			void UpdateImage() noexcept;

			///@}

			/**
				@name Images
				@{
			*/

			NonOwningPtr<graphics::scene::shapes::Sprite> CreateImage(NonOwningPtr<graphics::materials::Material> material);

			///@}

		public:

			///@brief Constructs an image with the given name, size, caption and hit boxes
			GuiImage(std::string name, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes = {}) noexcept;

			///@brief Constructs an image with the given name, skin, size, caption and hit boxes
			GuiImage(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
				std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes = {});


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the image source to the given image
			void Source(NonOwningPtr<graphics::materials::Material> image);

			///@brief Sets the image source to the given images
			///@details One for each of the corresponding states
			void Source(NonOwningPtr<graphics::materials::Material> image_enabled,
						NonOwningPtr<graphics::materials::Material> image_disabled,
						NonOwningPtr<graphics::materials::Material> image_focused,
						NonOwningPtr<graphics::materials::Material> image_pressed,
						NonOwningPtr<graphics::materials::Material> image_hovered);
			

			///@brief Sets the image color to the given color
			void FillColor(const std::optional<Color> &color) noexcept;

			///@brief Sets the image opacity to the given opacity
			void FillOpacity(real opacity) noexcept;

			///@brief Sets the image mode to the given mode
			void Mode(gui_image::ImageMode mode) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the image source
			[[nodiscard]] inline auto Source() const noexcept
			{
				return skin_ ?
					static_cast<gui_image::ImageSkin&>(*skin_).Image.Enabled :
					nullptr;
			}

			///@brief Returns the image sources
			///@details One for each of the corresponding states
			[[nodiscard]] inline auto Sources() const noexcept
			{
				return skin_ ?
					std::tuple{
						static_cast<gui_image::ImageSkin&>(*skin_).Image.Enabled,
						static_cast<gui_image::ImageSkin&>(*skin_).Image.Disabled,
						static_cast<gui_image::ImageSkin&>(*skin_).Image.Focused,
						static_cast<gui_image::ImageSkin&>(*skin_).Image.Pressed,
						static_cast<gui_image::ImageSkin&>(*skin_).Image.Hovered
					} :
					std::tuple{nullptr, nullptr, nullptr, nullptr, nullptr};
			}


			///@brief Returns the image color
			[[nodiscard]] inline auto FillColor() const noexcept
			{
				return color_.value_or(skin_color_);
			}

			///@brief Returns the image opacity
			[[nodiscard]] inline auto FillOpacity() const noexcept
			{
				return color_.value_or(skin_color_).A();
			}

			///@brief Returns the image mode
			[[nodiscard]] inline auto Mode() const noexcept
			{
				return mode_;
			}

			///@}
	};

} //ion::gui::controls

#endif