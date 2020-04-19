/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system/events/listeners
File:	IonSystemInputListener.cpp
-------------------------------------------
*/

#include "IonSystemInputListener.h"

#include <utility>

#include "graphics/render/IonRenderWindow.h"
#include "graphics/utilities/IonAabb.h"
#include "types/IonTypes.h"

namespace ion::system::events::listeners
{

using namespace ion::events::listeners;
using namespace types::type_literals;
using graphics::utilities::Aabb;

//Private

bool InputListener::Unsubscribable(Listenable<MessageListener>&) noexcept
{
	//Cancel all unsubscribe atempts
	return false;
}


/*
	Message listener events
*/

#ifdef ION_WIN32
bool InputListener::MessageReceived(HWND, UINT message, WPARAM w_param, LPARAM l_param) noexcept
{
	//Prepare mouse buttons
	switch (message)
	{
		//Left mouse button
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		w_param = MK_LBUTTON;
		break;

		//Right mouse button
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		w_param = MK_RBUTTON;
		break;

		//Middle mouse button
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		w_param = MK_MBUTTON;
		break;

		//X mouse button
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		{
			switch (HIWORD(w_param))
			{
				case XBUTTON1:
				w_param = MK_XBUTTON1;
				break;

				case XBUTTON2:
				w_param = MK_XBUTTON2;
				break;
			}

			break;
		}
	}

	switch (message)
	{
		//Key messages

		case WM_KEYDOWN:
		{
			//Don't register repetition
			if ((l_param & 0x40000000) == 0)
			{
				if (auto key_button = events::GetMappedKeyButton(w_param); key_button)
					KeyPressed(*key_button);
			}

			return true;
		}

		case WM_KEYUP:
		{
			if (auto key_button = events::GetMappedKeyButton(w_param); key_button)
				KeyReleased(*key_button);

			return true;
		}

		case WM_CHAR:
		//Repetition is okay
		CharacterPressed(static_cast<char>(w_param));
		return true;


		//Mouse messages

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		{
			if (auto mouse_button = events::GetMappedMouseButton(w_param); mouse_button)
				MousePressed(*mouse_button,
					ViewportAdjusted(static_cast<real>(LOWORD(l_param)), //x
									 static_cast<real>(HIWORD(l_param)))); //y
			return true;
		}
		
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			if (auto mouse_button = events::GetMappedMouseButton(w_param); mouse_button)
				MouseReleased(*mouse_button,
					ViewportAdjusted(static_cast<real>(LOWORD(l_param)), //x
									 static_cast<real>(HIWORD(l_param)))); //y
			return true;
		}

		case WM_MOUSEMOVE:
		{
			MouseMoved(ViewportAdjusted(static_cast<real>(LOWORD(l_param)), //x
										static_cast<real>(HIWORD(l_param)))); //y
			return true;
		}

		case WM_MOUSEWHEEL:
		{
			//Mouse wheel rolled coordinates are not in client space
			if (auto x = static_cast<real>(LOWORD(l_param)),
					 y = static_cast<real>(HIWORD(l_param)); IsInsideViewport(x, y))
			{
				//TODO, missing system::Window
				//Emulate for now
				auto [left, top] = Vector2{}.XY();
				MouseWheelRolled(static_cast<int>(HIWORD(w_param)), //delta
					ViewportAdjusted(x - left, y - top));
			}

			return true;
		}
	}

	return false;
}
#endif


bool InputListener::IsInsideViewport(real x, real y) const noexcept
{
	//TODO, missing system::Window
	//Emulate for now
	auto window_position = Vector2{};
	auto window_size = Vector2{1.0_r};
	auto window_area = Aabb{window_position, window_position + window_size};

	return window_area.Contains(Vector2{x, y});
}

Vector2 InputListener::ViewportAdjusted(real x, real y) const noexcept
{
	//TODO, missing system::Window
	//Emulate for now
	auto window_size = Vector2{1.0_r};
	auto viewport_size = Vector2{1.0_r};
	auto viewport_position = Vector2{x, viewport_size.Y() - y};

	return viewport_size * (viewport_position / window_size);
}


//Protected

/*
	Key listener events
*/

void InputListener::KeyPressed(KeyButton button) noexcept
{
	this->KeyEventsBase::NotifyAll(this->KeyEventsBase::Listeners(), &KeyListener::KeyPressed, button);
}

void InputListener::KeyReleased(KeyButton button) noexcept
{
	this->KeyEventsBase::NotifyAll(this->KeyEventsBase::Listeners(), &KeyListener::KeyReleased, button);
}

void InputListener::CharacterPressed(char character) noexcept
{
	this->KeyEventsBase::NotifyAll(this->KeyEventsBase::Listeners(), &KeyListener::CharacterPressed, character);
}


/*
	Mouse listener events
*/

void InputListener::MousePressed(MouseButton button, Vector2 position) noexcept
{
	this->MouseEventsBase::NotifyAll(this->MouseEventsBase::Listeners(), &MouseListener::MousePressed, button, position);
}

void InputListener::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	this->MouseEventsBase::NotifyAll(this->MouseEventsBase::Listeners(), &MouseListener::MouseReleased, button, position);
}

void InputListener::MouseMoved(Vector2 position) noexcept
{
	this->MouseEventsBase::NotifyAll(this->MouseEventsBase::Listeners(), &MouseListener::MouseMoved, position);
}

void InputListener::MouseWheelRolled(int delta, Vector2 position) noexcept
{
	this->MouseEventsBase::NotifyAll(this->MouseEventsBase::Listeners(), &MouseListener::MouseWheelRolled, delta, position);
}


//Public

InputListener::InputListener(graphics::render::RenderWindow &render_window) noexcept :
	render_window_{render_window}
{
	render_window_.MessageEvents().Subscribe(*this);
}

InputListener::~InputListener()
{
	MessageListener::Listening(false);
	render_window_.MessageEvents().Unsubscribe(*this);
}

} //ion::system::events::listeners