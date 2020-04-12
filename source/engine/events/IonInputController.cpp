/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events
File:	IonInputController.cpp
-------------------------------------------
*/

#include "IonInputController.h"

#include <cctype>
#include <utility>

#include "graphics/render/IonRenderWindow.h"

namespace ion::events
{

using namespace listeners;

//Private

bool InputController::Unsubscribable(Listenable<WindowListener>&) noexcept
{
	//Cancel all unsubscribe atempts
	return false;
}


/*
	Window listener events
*/

void InputController::WindowActionReceived(WindowAction action) noexcept
{
	switch (action)
	{
		case listeners::WindowAction::Close:
		case WindowAction::Deactivate:
		case WindowAction::Minimize:
		ReleaseButtons();
		break;
	}
}


/*
	Key listener events
*/

void InputController::KeyPressed(KeyButton button) noexcept
{
	if (!IsButtonPressed(button))
	{
		key_buttons_.push_back(button);
		InputListener::KeyPressed(button);
	}
}

void InputController::KeyReleased(KeyButton button) noexcept
{
	if (IsButtonPressed(button))
	{
		key_buttons_.erase(std::find(std::begin(key_buttons_), std::end(key_buttons_), button));
		InputListener::KeyReleased(button);
	}
}

void InputController::CharacterPressed(char character) noexcept
{
	//Accept only non control characters
	if (!std::iscntrl(static_cast<unsigned char>(character)))
		InputListener::CharacterPressed(character);
}


/*
	Mouse listener events
*/

void InputController::MousePressed(MouseButton button, Vector2 position) noexcept
{
	if (!IsButtonPressed(button))
	{
		mouse_buttons_.push_back(button);
		InputListener::MousePressed(button, position);
	}
}

void InputController::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	if (IsButtonPressed(button))
	{
		mouse_buttons_.erase(std::find(std::begin(mouse_buttons_), std::end(mouse_buttons_), button));
		InputListener::MouseReleased(button, position);
	}
}

void InputController::MouseMoved(Vector2 position) noexcept
{
	mouse_position_ = position;
	InputListener::MouseMoved(position);
}

void InputController::MouseWheelRolled(int delta, Vector2 position) noexcept
{
	InputListener::MouseWheelRolled(delta, position);
}


//Public

InputController::InputController(graphics::render::RenderWindow &render_window) noexcept :
	InputListener{render_window}
{
	render_window_.Events().Subscribe(*this);
}

//Public

InputController::~InputController()
{
	ReleaseButtons();

	WindowListener::Listening(false);
	render_window_.Events().Unsubscribe(*this);
}


/*
	Button pressed
*/

bool InputController::IsButtonPressed(KeyButton button) const noexcept
{
	return std::find(std::begin(key_buttons_), std::end(key_buttons_), button) != std::end(key_buttons_);
}

bool InputController::IsButtonPressed(MouseButton button) const noexcept
{
	return std::find(std::begin(mouse_buttons_), std::end(mouse_buttons_), button) != std::end(mouse_buttons_);
}


/*
	Release pressed buttons
*/

void InputController::ReleaseButtons() noexcept
{
	ReleaseKeyButtons();
	ReleaseMouseButtons();
}

void InputController::ReleaseKeyButtons() noexcept
{
	for (auto key_button : key_buttons_)
		KeyReleased(key_button);

	key_buttons_.clear();
}

void InputController::ReleaseMouseButtons() noexcept
{
	for (auto mouse_button : mouse_buttons_)
		MouseReleased(mouse_button, mouse_position_); //Release on last registered position

	mouse_buttons_.clear();
}

} //ion::events