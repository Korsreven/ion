/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiImage.cpp
-------------------------------------------
*/

#include "IonGuiImage.h"

#include <typeinfo>

#include "IonEngine.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "gui/IonGuiPanelContainer.h"
#include "gui/skins/IonGuiSkin.h"
#include "gui/skins/IonGuiTheme.h"

namespace ion::gui::controls
{

using namespace gui_image;

namespace gui_image
{

/*
	ImageSkin
*/

void ImageSkin::GetParts(gui_control::SkinPartPointers &parts, std::string_view name) const
{
	ControlSkin::GetParts(parts, name);
	auto all = name == "";

	//Image
	if (all || name == "image")
		parts.push_back(&Image);
}

namespace detail
{

/*
	Skins
*/

void resize_image(gui_control::ControlSkinPart &image, const Vector2 &delta_size) noexcept
{
	if (image)
	{
		auto &center = image->Position();
		gui_control::detail::resize_part(image, delta_size, vector2::Zero, center);
	}
}

void resize_skin(ImageSkin &skin, const Vector2 &from_size, const Vector2 &to_size, ImageMode mode) noexcept
{
	auto delta_size = to_size - from_size;

	if (skin.Image && mode == ImageMode::Fit)
	{
		auto fit_size = graphics::scene::shapes::rectangle::detail::scale_to_fit(skin.Image->Size(), to_size);
		delta_size = fit_size - from_size;
	}

	resize_image(skin.Image, delta_size);
}


bool has_center_area(const gui_control::ControlSkin &skin) noexcept
{
	return skin.Parts &&
		(skin.Parts.Center ||
		(skin.Parts.Top && skin.Parts.Bottom) ||
		(skin.Parts.Left && skin.Parts.Right) ||
		(skin.Parts.TopLeft && skin.Parts.BottomRight) ||
		(skin.Parts.BottomLeft && skin.Parts.TopRight));
}

} //detail
} //gui_image


//Private

void GuiImage::DefaultSetup() noexcept
{
	Focusable(false);
	CaptionLayout(gui_control::ControlCaptionLayout::OutsideTopCenter);
}


//Protected

/*
	Events
*/

void GuiImage::Resized(Vector2 from_size, Vector2 to_size) noexcept
{
	if (skin_)
		detail::resize_skin(static_cast<ImageSkin&>(*skin_), from_size, to_size, mode_);

	GuiControl::Resized(from_size, to_size); //Use base functionality
}


/*
	States
*/

void GuiImage::SetSkinState(gui_control::ControlState state, ImageSkin &skin) noexcept
{
	if (skin.Image)
		SetPartState(state, skin.Image);
}

void GuiImage::SetState(gui_control::ControlState state) noexcept
{
	GuiControl::SetState(state); //Use base functionality

	if (visible_ && skin_)
		SetSkinState(state, static_cast<ImageSkin&>(*skin_));
}


/*
	Skins
*/

OwningPtr<gui_control::ControlSkin> GuiImage::AttuneSkin(OwningPtr<gui_control::ControlSkin> skin) const
{
	//Not fully compatible
	if (skin && !dynamic_cast<ImageSkin*>(skin.get()))
	{
		auto image_skin = make_owning<ImageSkin>();
		image_skin->Assign(*skin);
		return image_skin;
	}
	else
		return skin;
}

void GuiImage::UpdateImage() noexcept
{
	if (skin_)
	{
		if (auto &skin = static_cast<ImageSkin&>(*skin_); skin.Image)
		{
			if (auto size = ContentSize(); size)
				detail::resize_skin(skin, skin.Image->Size(), *size, mode_);

			skin_color_ = skin.Image->FillColor();
		}
	}
}


/*
	Images
*/

NonOwningPtr<graphics::scene::shapes::Sprite> GuiImage::CreateImage(NonOwningPtr<graphics::materials::Material> material)
{
	if (skin_)
	{
		auto sprite = skin_->Parts->CreateMesh<graphics::scene::shapes::Sprite>(material);

		//Position
		auto [x, y, z] = sprite->Position().XYZ();
		sprite->Position({x, y, z + Engine::ZEpsilon()});

		if (auto size = ContentSize(); size)
		{
			switch (mode_)
			{
				case ImageMode::Fill:
				sprite->Size(*size);
				break;

				case ImageMode::Fit:
				sprite->ResizeToFit(*size);
				break;
			}
		}
		else
			Size(sprite->Size());

		skin_color_ = sprite->FillColor();

		if (color_)
			sprite->FillColor(*color_);

		return sprite;
	}
	else
		return nullptr;
}


//Public

GuiImage::GuiImage(std::string name, const std::optional<Vector2> &size,
	std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes) noexcept :
	GuiControl{std::move(name), size, std::move(caption), {}, std::move(hit_boxes)}
{
	DefaultSetup();
	UpdateImage();
}

GuiImage::GuiImage(std::string name, const skins::GuiSkin &skin, const std::optional<Vector2> &size,
	std::optional<std::string> caption, gui_control::BoundingBoxes hit_boxes) :
	GuiControl{std::move(name), skin, size, std::move(caption), {}, std::move(hit_boxes)}
{
	DefaultSetup();
	UpdateImage();
}


/*
	Modifiers
*/

void GuiImage::Source(NonOwningPtr<graphics::materials::Material> image)
{
	Source(image, nullptr, nullptr, nullptr, nullptr);
}

void GuiImage::Source(NonOwningPtr<graphics::materials::Material> image_enabled,
					  NonOwningPtr<graphics::materials::Material> image_disabled,
					  NonOwningPtr<graphics::materials::Material> image_focused,
					  NonOwningPtr<graphics::materials::Material> image_pressed,
					  NonOwningPtr<graphics::materials::Material> image_hovered)
{
	//No skin, create temp
	if (!skin_)
	{
		skins::GuiSkin skin{"", typeid(GuiImage)};

		if (auto owner = Owner(); owner)
		{
			if (auto theme = owner->CurrentTheme(); theme)
				skin.Owner(*theme);
		}

		if (skin.Owner())
			Skin(skin);
		else
			return;
	}
	
	auto &skin = static_cast<ImageSkin&>(*skin_);

	//No image, create sprite
	if (!skin.Image)
		skin.Image.Object = CreateImage(image_enabled);

	skin.Image.Enabled = image_enabled;
	skin.Image.Disabled = image_disabled;
	skin.Image.Focused = image_focused;
	skin.Image.Pressed = image_pressed;
	skin.Image.Hovered = image_hovered;

	SetState(state_);

	//Update caption based on image
	if (!detail::has_center_area(skin))
		UpdateCaption();
}


void GuiImage::FillColor(const std::optional<Color> &color) noexcept
{
	if (color_ != color)
	{
		color_ = color;

		if (skin_)
		{
			if (auto &skin = static_cast<ImageSkin&>(*skin_); skin.Image)
				skin.Image->FillColor(color_.value_or(skin_color_));
		}
	}
}

void GuiImage::FillOpacity(real opacity) noexcept
{
	auto [r, g, b, a] = color_.value_or(skin_color_).RGBA();
	FillColor(Color{r, g, b, opacity});
}


void GuiImage::Mode(ImageMode mode) noexcept
{
	if (mode_ != mode)
	{
		mode_ = mode;

		if (auto size = ContentSize(); size && skin_)
		{
			if (auto &skin = static_cast<ImageSkin&>(*skin_); skin.Image)
			{
				switch (mode_)
				{
					case ImageMode::Fill:
					skin.Image->Size(*size);
					break;

					case ImageMode::Fit:
					skin.Image->AutoSize(true);
					skin.Image->ResizeToFit(*size);
					break;
				}

				//Update caption based on image
				if (!detail::has_center_area(skin))
					UpdateCaption();
			}
		}
	}
}

} //ion::gui::controls