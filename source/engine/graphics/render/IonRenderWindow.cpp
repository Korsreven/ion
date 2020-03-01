/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonRenderWindow.cpp
-------------------------------------------
*/

#include "IonRenderWindow.h"

namespace ion::graphics::render
{

using namespace render_window;

namespace render_window::detail
{

} //render_window::detail


//Private

/*
	Notifying
*/

void RenderWindow::NotifyWindowActionReceived(events::listeners::WindowAction action) noexcept
{
	for (auto &listener : Events().Listeners())
		WindowEventsBase::Notify(&events::listeners::WindowListener::WindowActionReceived, listener, action);
}

void RenderWindow::NotifyWindowMoved(const Vector2 &position) noexcept
{
	for (auto &listener : Events().Listeners())
		WindowEventsBase::Notify(&events::listeners::WindowListener::WindowMoved, listener, position);
}

void RenderWindow::NotifyWindowResized(const Vector2 &size) noexcept
{
	for (auto &listener : Events().Listeners())
		WindowEventsBase::Notify(&events::listeners::WindowListener::WindowResized, listener, size);
}


/*
	Window events
*/

void RenderWindow::Opened() noexcept
{
	NotifyWindowActionReceived(events::listeners::WindowAction::Open);
}

void RenderWindow::Closed() noexcept
{
	NotifyWindowActionReceived(events::listeners::WindowAction::Close);
}


void RenderWindow::Activated() noexcept
{
	NotifyWindowActionReceived(events::listeners::WindowAction::Activate);
}

void RenderWindow::Deactivated() noexcept
{
	NotifyWindowActionReceived(events::listeners::WindowAction::Deactivate);
}


void RenderWindow::Maximized() noexcept
{
	NotifyWindowActionReceived(events::listeners::WindowAction::Maximize);
}

void RenderWindow::Minimized() noexcept
{
	NotifyWindowActionReceived(events::listeners::WindowAction::Minimize);
}

void RenderWindow::Restored() noexcept
{
	NotifyWindowActionReceived(events::listeners::WindowAction::Restore);
}


void RenderWindow::Moved(const Vector2 &position) noexcept
{
	//Track position only in windowed mode
	if (display_mode_ == WindowDisplayMode::Windowed)
	{
		//Keep position nullopt when window is centered
		if (position_ || !IsCentered())
			position_ = position;
	}

	NotifyWindowActionReceived(events::listeners::WindowAction::Move);
	NotifyWindowMoved(position);
}

void RenderWindow::Resized(const Vector2 &size) noexcept
{
	//Track position only in windowed mode
	if (display_mode_ == WindowDisplayMode::Windowed)
		size_ = size;

	NotifyWindowActionReceived(events::listeners::WindowAction::Resize);
	NotifyWindowResized(size);
	NotifyRenderTargetResized(size);
}

void RenderWindow::DisplayModeChanged() noexcept
{
	switch (display_mode_)
	{
		case WindowDisplayMode::Windowed:
		display_mode_ = pending_display_mode_ = WindowDisplayMode::FullScreen;
		break;

		case WindowDisplayMode::FullScreen:
		default:
		display_mode_ = pending_display_mode_ = WindowDisplayMode::Windowed;
		break;
	}
}


std::optional<Vector2> RenderWindow::GetFullScreenSize() const noexcept
{
	return full_screen_size_;
}

std::pair<std::optional<Vector2>, std::optional<Vector2>> RenderWindow::GetSizeConstraints() const noexcept
{
	return {min_size_, {}};
}


//Public

RenderWindow::RenderWindow(std::string_view title,
	const Vector2 &size, const std::optional<Vector2> &min_size,
	const std::optional<Vector2> &full_screen_size, const std::optional<Vector2> &position,
	WindowDisplayMode display_mode, WindowBorderStyle border_style,
	render_window::WindowCursor cursor, int color_depth) :

	title_{title},
	size_{size},
	min_size_{min_size},
	full_screen_size_{full_screen_size},
	position_{position},
	color_depth_{color_depth},

	display_mode_{display_mode},
	pending_display_mode_{display_mode},
	border_style_{border_style},
	cursor_{cursor}
{
	//Empty
}


/*
	Static window conversions
*/

RenderWindow RenderWindow::Borderless(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position)
{
	return {title, size, {}, {}, position, WindowDisplayMode::Windowed, WindowBorderStyle::None, WindowCursor::Default};
}

RenderWindow RenderWindow::Dialog(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position)
{
	return {title, size, {}, {}, position, WindowDisplayMode::Windowed, WindowBorderStyle::Dialog, WindowCursor::Default};
}

RenderWindow RenderWindow::NonResizable(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position)
{
	return {title, size, {}, {}, position, WindowDisplayMode::Windowed, WindowBorderStyle::Single, WindowCursor::Default};
}

RenderWindow RenderWindow::Resizable(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position)
{
	return {title, size, {}, {}, position, WindowDisplayMode::Windowed, WindowBorderStyle::Sizeable, WindowCursor::Default};
}


/*
	Events
*/

void RenderWindow::DoSwapBuffers() noexcept
{
	SwapBuffers();
}

Vector2 RenderWindow::GetRenderTargetSize() const noexcept
{
	if (auto inner_size = InnerSize(); inner_size)
		return *inner_size;
	else
		return size_;
}


/*
	Create/destroy
*/

bool RenderWindow::Create() noexcept
{
	switch (border_style_)
	{
		case WindowBorderStyle::None:
		return system::Window::CreateBorderless(title_, size_, position_, color_depth_);

		case WindowBorderStyle::Dialog:
		return system::Window::CreateDialog(title_, size_, position_, color_depth_);

		case WindowBorderStyle::Single:
		return system::Window::CreateNonResizable(title_, size_, position_, color_depth_);

		case WindowBorderStyle::Sizeable:
		return system::Window::CreateResizable(title_, size_, position_, color_depth_);
	}

	return false;
}

void RenderWindow::Destroy() noexcept
{
	system::Window::Destroy();
}


/*
	Show/hide
*/

bool RenderWindow::Show() noexcept
{
	auto visible = system::Window::Show();

	if (visible && pending_display_mode_ == WindowDisplayMode::FullScreen)
		EnterFullScreen(full_screen_size_);

	return visible;
}

bool RenderWindow::Hide() noexcept
{
	return system::Window::Hide();
}


/*
	Buffers
*/

void RenderWindow::SwapBuffers() noexcept
{
	return system::Window::SwapBuffers();
}

} //ion::graphics::render