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
#include "graphics/render/IonViewport.h"
#include "graphics/utilities/IonAabb.h"
#include "types/IonTypes.h"
#include "utilities/IonMath.h"

namespace ion::system::events::listeners
{

using namespace ion::events::listeners;
using namespace graphics::utilities;
using namespace types::type_literals;

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

	//Prepare mouse coordinates
	switch (message)
	{
		case WM_MOUSEWHEEL:
		{
			//Mouse wheel rolled coordinates are not in client space
			//Convert from screen to client coordinates
			auto point = POINT{static_cast<short>(LOWORD(l_param)), static_cast<short>(HIWORD(l_param))};
			ScreenToClient(render_window_.Handle(), &point);
			l_param = MAKELPARAM(point.x, point.y);

			//Cancel this message if outside window
			if (auto x = static_cast<real>(point.x),
					 y = static_cast<real>(point.y); !IsInsideWindow({x, y}))
				return true;

			[[fallthrough]];
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		case WM_MOUSEMOVE:
		{
			//Adjust coordinates from window client space to view space
			if (auto x = static_cast<real>(LOWORD(l_param)),
					 y = static_cast<real>(HIWORD(l_param)); !IsInsideViewport({x, y}))
				return true;
		}
	}

	//Process input messages
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
					ViewAdjusted({static_cast<real>(LOWORD(l_param)), //x
								  static_cast<real>(HIWORD(l_param))})); //y
			return true;
		}
		
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			if (auto mouse_button = events::GetMappedMouseButton(w_param); mouse_button)
				MouseReleased(*mouse_button,
					ViewAdjusted({static_cast<real>(LOWORD(l_param)), //x
								  static_cast<real>(HIWORD(l_param))})); //y
			return true;
		}

		case WM_MOUSEMOVE:
		{
			MouseMoved(ViewAdjusted({static_cast<real>(LOWORD(l_param)), //x
									 static_cast<real>(HIWORD(l_param))})); //y
			return true;
		}

		case WM_MOUSEWHEEL:
		{
			MouseWheelRolled(static_cast<short>(HIWORD(w_param)), //delta
				ViewAdjusted({static_cast<real>(LOWORD(l_param)), //x
							  static_cast<real>(HIWORD(l_param))})); //y
			return true;
		}
	}

	return false;
}
#endif

bool InputListener::IsInsideWindow(Vector2 position) const noexcept
{
	//Has render window
	if (auto inner_size = render_window_.InnerSize(); inner_size)
	{
		//Invert y-axis
		position.Y(inner_size->Y() - position.Y());
		return Aabb{vector2::Zero, *inner_size}.Contains(position);
	}
	else
		return false;
}

bool InputListener::IsInsideViewport(Vector2 position) const noexcept
{
	//Has render window
	if (auto inner_size = render_window_.InnerSize(); inner_size)
	{
		//Invert y-axis
		position.Y(inner_size->Y() - position.Y());

		//Has viewport at given position
		if (render_window_.GetViewport(position))
			return true;
	}
	
	return false;
}

Vector2 InputListener::ViewAdjusted(Vector2 position) const noexcept
{
	//Has render window
	if (auto inner_size = render_window_.InnerSize(); inner_size)
	{
		//Invert y-axis
		position.Y(inner_size->Y() - position.Y());

		//Has viewport at given position
		if (auto viewport = render_window_.GetViewport(position); viewport)
		{
			//Adjust coordinates from client to viewport coordinates
			position -= viewport->Bounds().Min();

			//Adjust coordinates from viewport to camera coordinates
			position = viewport->ViewportToCameraPoint(position);
		}
	}

	return position;
}


//Protected

/*
	Key listener events
*/

void InputListener::KeyPressed(KeyButton button) noexcept
{
	KeyEventsBase::NotifyAll(KeyEventsBase::Listeners(), &KeyListener::KeyPressed, button);
}

void InputListener::KeyReleased(KeyButton button) noexcept
{
	KeyEventsBase::NotifyAll(KeyEventsBase::Listeners(), &KeyListener::KeyReleased, button);
}

void InputListener::CharacterPressed(char character) noexcept
{
	KeyEventsBase::NotifyAll(KeyEventsBase::Listeners(), &KeyListener::CharacterPressed, character);
}


/*
	Mouse listener events
*/

void InputListener::MousePressed(MouseButton button, Vector2 position) noexcept
{
	MouseEventsBase::NotifyAll(MouseEventsBase::Listeners(), &MouseListener::MousePressed, button, position);
}

void InputListener::MouseReleased(MouseButton button, Vector2 position) noexcept
{
	MouseEventsBase::NotifyAll(MouseEventsBase::Listeners(), &MouseListener::MouseReleased, button, position);
}

void InputListener::MouseMoved(Vector2 position) noexcept
{
	MouseEventsBase::NotifyAll(MouseEventsBase::Listeners(), &MouseListener::MouseMoved, position);
}

void InputListener::MouseWheelRolled(int delta, Vector2 position) noexcept
{
	MouseEventsBase::NotifyAll(MouseEventsBase::Listeners(), &MouseListener::MouseWheelRolled, delta, position);
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