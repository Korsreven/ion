/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	gui/controls
File:	IonGuiControl.cpp
-------------------------------------------
*/

#include "IonGuiControl.h"

#include "graphics/materials/IonMaterial.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "graphics/utilities/IonMatrix3.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonVector3.h"
#include "graphics/utilities/IonObb.h"
#include "gui/IonGuiFrame.h"
#include "gui/IonGuiPanelContainer.h"
#include "types/IonTypeTraits.h"

namespace ion::gui::controls
{

using namespace gui_control;

namespace gui_control::detail
{


void resize_part(ControlVisualPart &part, const Vector2 &delta_size, const Vector2 &delta_position, const Vector2 &center) noexcept
{
	if (part)
	{
		auto position = Vector2{part->Position()};
		auto direction = (position - center).NormalizeCopy();
		part->Position(part->Position() + direction * delta_position);
		part->Size(part->Size() + delta_size);
	}
}

void resize_parts(ControlVisualParts &parts, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto delta_size = to_size - from_size;
	auto delta_position = delta_size * 0.5_r;
	auto &center = parts.Center->Position();
	auto [delta_width, delta_height] = delta_size.XY();

	//Center
	resize_part(parts.Center, delta_size, vector2::Zero, center);

	//Sides
	resize_part(parts.Top, {delta_width, 0.0_r}, delta_position, center);
	resize_part(parts.Left, {0.0_r, delta_height}, delta_position, center);
	resize_part(parts.Bottom, {delta_width, 0.0_r}, delta_position, center);
	resize_part(parts.Right, {0.0_r, delta_height}, delta_position, center);

	//Corners
	resize_part(parts.TopLeft, vector2::Zero, delta_position, center);
	resize_part(parts.TopRight, vector2::Zero, delta_position, center);
	resize_part(parts.BottomLeft, vector2::Zero, delta_position, center);
	resize_part(parts.BottomRight, vector2::Zero, delta_position, center);
}


void resize_area(Aabb &area, const Vector2 &scaling) noexcept
{
	area.Transform(Matrix3::Transformation(0.0_r, scaling, vector2::Zero));
}

void resize_areas(Areas &areas, const Vector2 &from_size, const Vector2 &to_size) noexcept
{
	auto scaling = to_size / from_size;

	for (auto &area : areas)
		resize_area(area, scaling);
}

} //gui_control::detail

//Protected

/*
	Events
*/

void GuiControl::Enabled() noexcept
{
	SetState(ControlState::Enabled);
	NotifyControlEnabled();
	GuiComponent::Enabled(); //Use base functionality
}

void GuiControl::Disabled() noexcept
{
	Reset();
	SetState(ControlState::Disabled);
	NotifyControlDisabled();
	GuiComponent::Disabled(); //Use base functionality
}


void GuiControl::Focused() noexcept
{
	if (state_ == ControlState::Enabled)
		SetState(ControlState::Focused);

	NotifyControlFocused();
}

void GuiControl::Defocused() noexcept
{
	if (state_ == ControlState::Focused)
		SetState(ControlState::Enabled);

	NotifyControlDefocused();
}


void GuiControl::Pressed() noexcept
{
	Focus();
	SetState(ControlState::Pressed);
	NotifyControlPressed();
}

void GuiControl::Released() noexcept
{
	if (hovered_)
		SetState(ControlState::Hovered);
	else if (focused_)
		SetState(ControlState::Focused);
	else
		SetState(ControlState::Enabled);

	NotifyControlReleased();
}

void GuiControl::Clicked() noexcept
{
	NotifyControlClicked();
}


void GuiControl::Entered() noexcept
{
	if (state_ != ControlState::Pressed)
		SetState(ControlState::Hovered);

	NotifyControlEntered();
}

void GuiControl::Exited() noexcept
{
	if (state_ != ControlState::Pressed)
	{
		if (focused_)
			SetState(ControlState::Focused);
		else
			SetState(ControlState::Enabled);
	}

	NotifyControlExited();
}


void GuiControl::Changed() noexcept
{
	NotifyControlChanged();
}

void GuiControl::Resized() noexcept
{
	NotifyControlResized();
}


void GuiControl::StateChanged() noexcept
{
	NotifyControlStateChanged();
}


/*
	Notifying
*/

void GuiControl::NotifyControlEnabled() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Enabled, std::ref(*this));
	}
}

void GuiControl::NotifyControlDisabled() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Disabled, std::ref(*this));
	}
}


void GuiControl::NotifyControlFocused() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Focused, std::ref(*this));
	}

	//User callback
	if (on_focus_)
		(*on_focus_)(*this);
}

void GuiControl::NotifyControlDefocused() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Defocused, std::ref(*this));
	}

	//User callback
	if (on_defocus_)
		(*on_defocus_)(*this);
}


void GuiControl::NotifyControlPressed() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Pressed, std::ref(*this));
	}

	//User callback
	if (on_press_)
		(*on_press_)(*this);
}

void GuiControl::NotifyControlReleased() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Released, std::ref(*this));
	}

	//User callback
	if (on_release_)
		(*on_release_)(*this);
}

void GuiControl::NotifyControlClicked() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Clicked, std::ref(*this));
	}

	//User callback
	if (on_click_)
		(*on_click_)(*this);
}


void GuiControl::NotifyControlEntered() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Entered, std::ref(*this));
	}

	//User callback
	if (on_enter_)
		(*on_enter_)(*this);
}

void GuiControl::NotifyControlExited() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Exited, std::ref(*this));
	}

	//User callback
	if (on_exit_)
		(*on_exit_)(*this);
}


void GuiControl::NotifyControlChanged() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Changed, std::ref(*this));
	}

	//User callback
	if (on_change_)
		(*on_change_)(*this);
}

void GuiControl::NotifyControlResized() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::Resized, std::ref(*this));
	}

	//User callback
	if (on_resize_)
		(*on_resize_)(*this);
}


void GuiControl::NotifyControlStateChanged() noexcept
{
	if (auto owner = Owner(); owner)
	{
		if (auto frame = owner->ParentFrame(); frame)
			NotifyAll(frame->ControlEvents().Listeners(),
				&events::listeners::GuiControlListener::StateChanged, std::ref(*this));
	}

	//User callback
	if (on_state_change_)
		(*on_state_change_)(*this);
}


/*
	States
*/

NonOwningPtr<graphics::materials::Material> GuiControl::GetStateMaterial(ControlState state, ControlVisualPart &part) noexcept
{
	auto material = detail::control_state_to_material(state, part);

	//Fallback
	if (!material)
	{
		//Check hovered
		if (hovered_ && state != ControlState::Hovered)
		{
			if (part.HoveredMaterial)
				return part.HoveredMaterial; //Display hovered material instead
		}

		//Check focused
		if (focused_ && state != ControlState::Focused)
		{
			if (part.FocusedMaterial)
				return part.FocusedMaterial; //Display focused material instead
		}

		//Check enabled
		if (state != ControlState::Enabled)
		{
			if (part.EnabledMaterial)
				return part.EnabledMaterial; //Display enabled material instead
		}
	}

	return material;
}


void GuiControl::SetPartState(ControlState state, ControlVisualPart &part) noexcept
{
	if (part)
	{
		if (auto material = GetStateMaterial(state, part); material)
			part->SurfaceMaterial(material);
	}
}

void GuiControl::SetSkinState(ControlState state, ControlSkin &skin) noexcept
{
	if (skin.Parts)
	{
		//Center
		SetPartState(state, skin.Parts.Center);

		//Sides
		SetPartState(state, skin.Parts.Top);
		SetPartState(state, skin.Parts.Left);
		SetPartState(state, skin.Parts.Bottom);
		SetPartState(state, skin.Parts.Right);

		//Corners
		SetPartState(state, skin.Parts.TopLeft);
		SetPartState(state, skin.Parts.BottomLeft);
		SetPartState(state, skin.Parts.TopRight);
		SetPartState(state, skin.Parts.BottomRight);
	}
}

void GuiControl::SetState(ControlState state) noexcept
{
	if (visible_)
		SetSkinState(state, skin_);

	if (state_ != state)
	{
		state_ = state;
		StateChanged();
	}
}


/*
	Skins
*/

void GuiControl::AttachSkin(gui_control::ControlSkin skin)
{
	DetachSkin();

	if (skin.Parts)
	{
		if (skin.Parts->ParentNode())
			skin.Parts->ParentNode()->DetachObject(*skin.Parts.Owner);
		
		if (node_) //Create child node for all parts
			skin.Parts->ParentNode(node_->CreateChildNode(node_->Visible()).get());
	}

	if (skin.Caption)
	{
		if (skin.Caption->ParentNode())
			skin.Caption->ParentNode()->DetachObject(*skin.Caption);
		
		if (node_) //Create child node for caption
			skin.Caption->ParentNode(node_->CreateChildNode(node_->Visible()).get());
	}

	skin_ = std::move(skin);
}

void GuiControl::DetachSkin() noexcept
{
	if (skin_.Parts)
	{
		if (auto node = skin_.Parts->ParentNode(); node_ && node)
			node_->RemoveChildNode(*node); //Remove parts node

		skin_.Parts->Owner()->RemoveModel(*skin_.Parts.Owner); //Remove all parts
	}

	if (skin_.Caption)
	{
		if (auto node = skin_.Caption->ParentNode(); node_ && node)
			node_->RemoveChildNode(*node); //Remove caption node

		skin_.Caption->Owner()->RemoveText(*skin_.Caption); //Remove caption
	}

	skin_ = {};
}


//Public

GuiControl::GuiControl(std::string name) :
	GuiComponent{std::move(name)}
{
	//Empty
}

GuiControl::GuiControl(std::string name, const Vector2 &size) :

	GuiComponent{std::move(name)},
	clickable_areas_{Aabb::Size(size)}
{
	//Empty
}

GuiControl::GuiControl(std::string name, Areas areas) :

	GuiComponent{std::move(name)},
	clickable_areas_{std::move(areas)}
{
	//Empty
}

GuiControl::GuiControl(std::string name, gui_control::ControlSkin skin) :
	GuiComponent{std::move(name)}
{
	AttachSkin(std::move(skin));
}

GuiControl::GuiControl(std::string name, gui_control::ControlSkin skin, const Vector2 &size) :
	GuiComponent{std::move(name)}
{
	AttachSkin(std::move(skin));
	Size(size); //Resize skin to the given size
}

GuiControl::~GuiControl() noexcept
{
	DetachSkin();
}


/*
	Modifiers
*/

void GuiControl::Focus() noexcept
{
	if (!focused_ &&
		enabled_ && focusable_ && visible_)
	{
		if (auto owner = Owner(); !owner || owner->IsFocusable())
		{
			focused_ = true;
			Focused();
		}
	}
}

void GuiControl::Defocus() noexcept
{
	if (focused_)
	{
		focused_ = false;
		Defocused();
	}
}


void GuiControl::Press() noexcept
{
	if (!pressed_ && enabled_)
	{
		if (auto owner = Owner(); !owner || owner->IsFocusable())
		{
			pressed_ = true;
			Pressed();
		}
	}
}

void GuiControl::Release() noexcept
{
	if (pressed_)
	{
		pressed_ = false;
		Released();
	}
}

void GuiControl::Click() noexcept
{
	if (enabled_)
		Clicked();
}


void GuiControl::Enter() noexcept
{
	if (!hovered_ && enabled_)
	{
		if (auto owner = Owner(); !owner || owner->IsFocusable())
		{
			hovered_ = true;
			Entered();
		}
	}
}

void GuiControl::Exit() noexcept
{
	if (hovered_)
	{
		hovered_ = false;
		Exited();
	}
}


void GuiControl::Reset() noexcept
{
	Release();
	Defocus();
	Exit();
}


void GuiControl::Visible(bool visible) noexcept
{
	if (visible_ != visible)
	{
		visible_ = visible;

		if (!visible && focused_)
			Defocus();

		if (node_)
			node_->Visible(visible, !visible);

		SetState(state_);
	}
}

void GuiControl::Skin(gui_control::ControlSkin skin) noexcept
{
	if (skin_.Parts.Owner != skin.Parts.Owner)
	{
		if (skin.Parts)
		{
			//A skin is already attached, inherit size
			if (skin_.Parts)
				gui_control::detail::resize_parts(skin.Parts,
					skin_.Parts->AxisAlignedBoundingBox().ToSize(),
					skin.Parts->AxisAlignedBoundingBox().ToSize());

			else //No skin attached
				gui_control::detail::resize_parts(skin.Parts,
					Size(),
					skin.Parts->AxisAlignedBoundingBox().ToSize());
		}

		AttachSkin(std::move(skin));
	}
}


void GuiControl::Size(const Vector2 &size) noexcept
{
	if (auto current_size = Size(); current_size != size)
	{
		auto resized = false;

		if (skin_.Parts)
		{
			detail::resize_parts(skin_.Parts, current_size, size);
			resized = true;
		}

		if (std::empty(clickable_areas_))
		{
			if (!skin_.Parts)
			{
				clickable_areas_.push_back(Aabb::Size(size));
				resized = true;
			}
		}
		else if (std::size(clickable_areas_) == 1)
		{
			clickable_areas_.back() = Aabb::Size(size);
			resized = true;
		}

		//Multiple areas
		else if (current_size.X() != 0.0_r && current_size.Y() != 0.0_r)
		{
			detail::resize_areas(clickable_areas_, current_size, size);
			resized = true;
		}

		if (resized)
			Resized();
	}
}


/*
	Observers
*/

Vector2 GuiControl::Size() const noexcept
{
	if (std::empty(clickable_areas_))
		return skin_.Parts ?
			skin_.Parts->AxisAlignedBoundingBox().ToSize() :
			vector2::Zero;
	else if (std::size(clickable_areas_) == 1)
		return clickable_areas_.back().ToSize();
	else //Multiple areas
		return Aabb::Enclose(clickable_areas_).ToSize();
}


GuiPanelContainer* GuiControl::Owner() const noexcept
{
	return static_cast<GuiPanelContainer*>(owner_);
}


/*
	Intersecting
*/

bool GuiControl::Intersects(const Vector2 &point) const noexcept
{
	if (node_ && visible_)
	{
		if (std::empty(clickable_areas_))
		{
			if (skin_.Parts &&
				skin_.Parts->WorldOrientedBoundingBox().Intersects(point))
				return true;
		}
		else
		{
			for (auto &area : clickable_areas_)
			{
				if (Obb{area}.Transform(node_->FullTransformation()).Intersects(point))
					return true;
			}
		}
	}

	return false;
}


/*
	Tabulating
*/

void GuiControl::TabOrder(int order) noexcept
{
	if (auto owner = Owner(); owner)
		owner->TabOrder(*this, order);
}

std::optional<int> GuiControl::TabOrder() const noexcept
{
	if (auto owner = Owner(); owner)
		return owner->TabOrder(*this);
	else
		return std::nullopt;
}


/*
	Frame events
*/

void GuiControl::FrameStarted([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}

void GuiControl::FrameEnded([[maybe_unused]] duration time) noexcept
{
	//Optional to override
}


/*
	Key events
*/

bool GuiControl::KeyPressed([[maybe_unused]] KeyButton button) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::KeyReleased([[maybe_unused]] KeyButton button) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::CharacterPressed([[maybe_unused]] char character) noexcept
{
	//Optional to override
	return false;
}


/*
	Mouse events
*/

bool GuiControl::MousePressed([[maybe_unused]] MouseButton button, [[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::MouseReleased([[maybe_unused]] MouseButton button, [[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::MouseMoved([[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
	return false;
}

bool GuiControl::MouseWheelRolled([[maybe_unused]] int delta, [[maybe_unused]] Vector2 position) noexcept
{
	//Optional to override
	return false;
}

} //ion::gui::controls