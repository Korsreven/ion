/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system
File:	IonSystemWindow.cpp
-------------------------------------------
*/

#include "IonSystemWindow.h"

#include "graphics/IonGraphicsAPI.h"

namespace ion::system
{

using namespace window;

namespace window::detail
{

#ifdef ION_WIN32

LRESULT CALLBACK window_procedure(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) noexcept
{
	auto window =
		[=]() noexcept
		{
			switch (message)
			{
				case WM_NCCREATE:
				{
					auto window = static_cast<Window*>(reinterpret_cast<LPCREATESTRUCT>(l_param)->lpCreateParams);
					SetWindowLongPtr(handle, GWL_USERDATA, reinterpret_cast<LONG_PTR>(window)); //Set user data
					return window;
				}

				default:
				return reinterpret_cast<Window*>(GetWindowLongPtr(handle, GWL_USERDATA)); //Get user data
			}
		}();

	return window ?
		window->Procedure(handle, message, w_param, l_param) :
		DefWindowProc(handle, message, w_param, l_param);
}

WNDCLASS make_window_class(std::string_view name, HINSTANCE instance) noexcept
{
	auto wnd_class = WNDCLASS{};
	wnd_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wnd_class.lpfnWndProc = static_cast<WNDPROC>(window_procedure);
	wnd_class.hInstance = instance;
	wnd_class.hIcon = LoadIcon(0, IDI_WINLOGO);
	wnd_class.hCursor = LoadCursor(0, IDC_ARROW);
	wnd_class.lpszClassName = std::data(name);

	return wnd_class;
}

PIXELFORMATDESCRIPTOR make_pixel_format_descriptor(int color_depth) noexcept
{
	auto pfd = PIXELFORMATDESCRIPTOR{};
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.cColorBits = static_cast<BYTE>(color_depth);
	pfd.cAlphaBits = 8; //Enable alpha in frame buffer
	pfd.cDepthBits = 16;

	return pfd;
}

DEVMODE make_device_mode(const Vector2 &full_screen_size, int color_depth) noexcept
{
	auto dev_mode = DEVMODE{};
	dev_mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	dev_mode.dmBitsPerPel = color_depth;

	auto [width, height] = full_screen_size.XY();
	dev_mode.dmPelsWidth = static_cast<DWORD>(width);
	dev_mode.dmPelsHeight = static_cast<DWORD>(height);

	return dev_mode;
}


DWORD make_window_style(WindowBorderStyle border_style) noexcept
{
	switch (border_style)
	{
		case WindowBorderStyle::None:
		return WS_POPUP;

		case WindowBorderStyle::Dialog:
		return WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

		case WindowBorderStyle::Single:
		return WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

		case WindowBorderStyle::Sizeable:
		default:
		return WS_OVERLAPPEDWINDOW;
	}
}

DWORD make_extended_window_style(WindowBorderStyle border_style) noexcept
{
	switch (border_style)
	{
		case WindowBorderStyle::None:
		return WS_EX_APPWINDOW & ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);

		case WindowBorderStyle::Dialog:
		case WindowBorderStyle::Single:
		case WindowBorderStyle::Sizeable:
		default:
		return WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	}
}


RECT get_desktop_rectangle() noexcept
{
	RECT rectangle;
	GetWindowRect(GetDesktopWindow(), &rectangle);
	return rectangle;
}

RECT get_adjusted_window_rectangle(RECT rectangle, DWORD style, DWORD extended_style) noexcept
{
	AdjustWindowRectEx(&rectangle, style, 0, extended_style);
	return rectangle;
}

RECT get_non_client_window_rectangle(DWORD style, DWORD extended_style) noexcept
{
	return get_adjusted_window_rectangle({}, style, extended_style);
}

RECT get_window_rectangle(RECT client_rectangle, DWORD style, DWORD extended_style) noexcept
{
	auto non_client_rectangle = get_non_client_window_rectangle(style, extended_style);
	return {
		client_rectangle.left + non_client_rectangle.left,
		client_rectangle.top + non_client_rectangle.top,
		client_rectangle.right - (non_client_rectangle.left - non_client_rectangle.right),
		client_rectangle.bottom - (non_client_rectangle.top - non_client_rectangle.bottom)
	};
}

RECT get_client_window_rectangle(RECT rectangle, DWORD style, DWORD extended_style) noexcept
{
	auto non_client_rectangle = get_non_client_window_rectangle(style, extended_style);
	return {
		rectangle.left - non_client_rectangle.left,
		rectangle.top - non_client_rectangle.top,
		rectangle.right + (non_client_rectangle.left - non_client_rectangle.right),
		rectangle.bottom + (non_client_rectangle.top - non_client_rectangle.bottom)
	};
}

RECT get_centered_window_rectangle(RECT rectangle) noexcept
{
	auto desktop_rectangle = get_desktop_rectangle();
	return {
		(desktop_rectangle.right - desktop_rectangle.left) / 2 -
		(rectangle.right - rectangle.left) / 2 + desktop_rectangle.left,

		(desktop_rectangle.bottom - desktop_rectangle.top) / 2 -
		(rectangle.bottom - rectangle.top) / 2 + desktop_rectangle.top,

		rectangle.right,
		rectangle.bottom
	};
}


RECT make_window_rectangle(const Vector2 &size, const std::optional<Vector2> &position, WindowBorderStyle border_style) noexcept
{
	auto rectangle = RECT{};

	auto [width, height] = size.XY();
	rectangle.right = static_cast<LONG>(width);
	rectangle.bottom = static_cast<LONG>(height);

	rectangle =
		get_window_rectangle(
			rectangle,
			make_window_style(border_style),
			make_extended_window_style(border_style));

	if (position)
	{
		auto [x, y] = position->XY();
		rectangle.left = static_cast<LONG>(x);
		rectangle.top = static_cast<LONG>(y);		
	}
	//Center on desktop
	else
	{
		rectangle.left = 0;
		rectangle.top = 0;
		rectangle = get_centered_window_rectangle(rectangle);
	}

	return rectangle;
}


window_class::window_class(HINSTANCE instance) noexcept
{
	if (!instance)
		instance = GetModuleHandle(nullptr);

	if (instance)
	{
		if (auto wnd_class = make_window_class(class_name, instance);
			RegisterClass(&wnd_class) != 0)

			handle_ = instance;
	}
}

window_class::window_class(window_class &&rhs) noexcept :
	handle_base{std::move(rhs)}
{
	//Empty
}

window_class::~window_class()
{
	if (handle_)
		UnregisterClass(std::data(class_name), handle_);
}

window_class& window_class::operator=(window_class &&rhs) noexcept
{
	handle_base::operator=(std::move(rhs));
	return *this;
}

void window_class::reset(window_class &&rhs) noexcept
{
	*this = std::move(rhs);
}


window_handle::window_handle(window_class &win_class,
	std::string_view title, const Vector2 &size, const std::optional<Vector2> &position,
	WindowBorderStyle border_style, LPVOID parameter) noexcept
{
	if (win_class)
	{
		auto rectangle =
			make_window_rectangle(size, position, border_style);

		handle_ =
			CreateWindowEx(
				make_extended_window_style(border_style),
				std::data(class_name), std::data(title),
				make_window_style(border_style) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
				rectangle.left, rectangle.top,
				rectangle.right, rectangle.bottom,
				nullptr /*parent*/, nullptr /*menu*/,
				*win_class, parameter);
	}
}

window_handle::window_handle(window_handle &&rhs) noexcept :
	handle_base{std::move(rhs)}
{
	//Empty
}

window_handle::~window_handle()
{
	if (handle_)
		DestroyWindow(handle_);
}

window_handle& window_handle::operator=(window_handle &&rhs) noexcept
{
	handle_base::operator=(std::move(rhs));
	return *this;
}

void window_handle::reset(window_handle &&rhs) noexcept
{
	*this = std::move(rhs);
}


device_context::device_context(window_handle &win_handle, int color_depth) noexcept :
	win_handle_{&win_handle}
{
	if (win_handle)
	{
		if (handle_ = GetDC(*win_handle); handle_)
		{
			auto pfd = make_pixel_format_descriptor(color_depth);

			if (auto pixel_format = ChoosePixelFormat(handle_, &pfd); pixel_format != 0)
				SetPixelFormat(handle_, pixel_format, &pfd);
		}
	}
}

device_context::device_context(device_context &&rhs) noexcept :
	handle_base{std::move(rhs)}
{
	//Empty
}

device_context::~device_context()
{
	if (handle_)
		ReleaseDC(**win_handle_, handle_);
}

device_context& device_context::operator=(device_context &&rhs) noexcept
{
	handle_base::operator=(std::move(rhs));

	if (this != &rhs)
		win_handle_ = std::exchange(rhs.win_handle_, nullptr);

	return *this;
}

void device_context::reset(device_context &&rhs) noexcept
{
	*this = std::move(rhs);
}


rendering_context::rendering_context(device_context &dev_context) noexcept
{
	if (dev_context)
	{
		if (handle_ = wglCreateContext(*dev_context); handle_)
			wglMakeCurrent(*dev_context, handle_);
	}
}

rendering_context::rendering_context(rendering_context &&rhs) noexcept :
	handle_base{std::move(rhs)}
{
	//Empty
}

rendering_context::~rendering_context()
{
	if (handle_)
	{
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(handle_);
	}
}

rendering_context& rendering_context::operator=(rendering_context &&rhs) noexcept
{
	handle_base::operator=(std::move(rhs));
	return *this;
}

void rendering_context::reset(rendering_context &&rhs) noexcept
{
	*this = std::move(rhs);
}


bool enter_full_screen_mode(const std::optional<Vector2> &full_screen_size, int color_depth, HWND handle) noexcept
{
	auto size = full_screen_size ?
		*full_screen_size :
		[]() noexcept
		{
			auto desktop_rectangle = get_desktop_rectangle();
			return Vector2{static_cast<real>(desktop_rectangle.right - desktop_rectangle.left),
						   static_cast<real>(desktop_rectangle.bottom - desktop_rectangle.top)};
		}();

	if (auto dev_mode = make_device_mode(size, color_depth);
		ChangeDisplaySettings(&dev_mode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
	{
		change_border_style(WindowBorderStyle::None, handle);	
		change_position(graphics::utilities::vector2::Zero, handle);
		change_client_size(size, handle);
		return true;
	}
	else
		return false;
}

bool exit_full_screen_mode(const Vector2 &size, const std::optional<Vector2> &position, WindowBorderStyle border_style, HWND handle) noexcept
{
	if (ChangeDisplaySettings(nullptr, 0) == DISP_CHANGE_SUCCESSFUL)
	{
		change_border_style(border_style, handle);
		change_client_size(size, handle);

		if (position)
			change_position(*position, handle);
		else
			center_window(handle);
		
		return true;
	}
	else
		return false;
}


bool change_title(std::string_view title, HWND handle) noexcept
{
	return SetWindowText(handle, std::data(title)) != 0;
}

bool change_client_size(const Vector2 &size, HWND handle) noexcept
{
	auto rectangle = RECT{};

	auto [width, height] = size.XY();
	rectangle.right = static_cast<LONG>(width);
	rectangle.bottom = static_cast<LONG>(height);

	rectangle =
		get_window_rectangle(
		rectangle,
		get_window_style(handle),
		get_extended_window_style(handle));

	auto [x, y] = get_position(handle).XY();
	return MoveWindow(
		handle,
		static_cast<LONG>(x),
		static_cast<LONG>(y),
		rectangle.right,
		rectangle.bottom, 1) != 0;
}

bool change_full_screen_size(WindowMode mode, const std::optional<Vector2> &full_screen_size, int color_depth, HWND handle) noexcept
{
	if (mode == WindowMode::FullScreen)
		return enter_full_screen_mode(full_screen_size, color_depth, handle);
	else
		return true;
}

bool change_position(const Vector2 &position, HWND handle) noexcept
{
	auto [x, y] = position.XY();
	auto [width, height] = get_size(handle).XY();
	return MoveWindow(
		handle,
		static_cast<LONG>(x),
		static_cast<LONG>(y),
		static_cast<LONG>(width),
		static_cast<LONG>(height), 1) != 0;
}


bool change_border_style(WindowBorderStyle border_style, HWND handle) noexcept
{
	SetWindowLong(handle, GWL_STYLE, make_window_style(border_style));
	SetWindowLong(handle, GWL_EXSTYLE, make_extended_window_style(border_style));

	auto rectangle = get_window_rectangle(handle);
	return SetWindowPos(
		handle, nullptr,
		0, 0,
		rectangle.right - rectangle.left,
		rectangle.bottom - rectangle.top,	
		SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW) != 0;
}

bool change_cursor(WindowCursor cursor) noexcept
{
	if (cursor == WindowCursor::Default)
	{
		for (auto counter = ShowCursor(1); counter < 0;)
		{
			if (auto prev_counter = counter;
				(counter = ShowCursor(1)) == prev_counter)
				return false;
		}
	}
	else
	{
		for (auto counter = ShowCursor(0); counter >= 0;)
		{
			if (auto prev_counter = counter;
				(counter = ShowCursor(0)) == prev_counter)
				return false;
		}
	}

	return true;
}


bool show_window(int cmd_show, HWND handle) noexcept
{
	if (ShowWindow(handle, cmd_show) == 0)
	{
		focus_window(handle);
		return true;
	}
	else
		return false;
}

bool hide_window(HWND handle) noexcept
{
	return ShowWindow(handle, SW_HIDE) != 0;
}

bool center_window(HWND handle) noexcept
{
	auto rectangle = get_centered_window_rectangle(handle);
	return MoveWindow(handle,
		rectangle.left,
		rectangle.top,
		rectangle.right,
		rectangle.bottom, 1) != 0;
}

bool focus_window(HWND handle) noexcept
{
	return SetForegroundWindow(handle) == 1 &&
		   SetFocus(handle) == handle;
}


DWORD get_window_style(HWND handle) noexcept
{
	return GetWindowLong(handle, GWL_STYLE);
}

DWORD get_extended_window_style(HWND handle) noexcept
{
	return GetWindowLong(handle, GWL_EXSTYLE);
}

RECT get_window_rectangle(HWND handle) noexcept
{
	RECT rectangle;
	GetWindowRect(handle, &rectangle);
	return rectangle;
}

RECT get_client_window_rectangle(HWND handle) noexcept
{
	RECT rectangle;
	GetClientRect(handle, &rectangle);
	return rectangle;
}

RECT get_centered_window_rectangle(HWND handle) noexcept
{
	auto [width, height] = get_size(handle).XY();

	auto rectangle = RECT{};
	rectangle.right = static_cast<LONG>(width);
	rectangle.bottom = static_cast<LONG>(height);

	return get_centered_window_rectangle(rectangle);
}


Vector2 get_size(HWND handle) noexcept
{
	auto rectangle = get_window_rectangle(handle);
	return {static_cast<real>(rectangle.right - rectangle.left),
			static_cast<real>(rectangle.bottom - rectangle.top)};
}

Vector2 get_client_size(HWND handle) noexcept
{
	auto rectangle = get_client_window_rectangle(handle);
	return {static_cast<real>(rectangle.right - rectangle.left),
			static_cast<real>(rectangle.bottom - rectangle.top)};
}

Vector2 get_position(HWND handle) noexcept
{
	auto rectangle = get_window_rectangle(handle);
	return {static_cast<real>(rectangle.left),
			static_cast<real>(rectangle.top)};
}

Vector2 get_client_position(HWND handle) noexcept
{
	auto rectangle = get_client_window_rectangle(handle);
	return {static_cast<real>(rectangle.left),
			static_cast<real>(rectangle.top)};
}

bool is_active(HWND handle) noexcept
{
	return GetActiveWindow() == handle;
}

#endif


void change_viewport(const Vector2 &size) noexcept
{
	auto [width, height] = size.XY();
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}

} //window::detail


//Private

#ifdef ION_WIN32
bool Window::ProcessMessage(HWND handle, UINT message, WPARAM w_param, LPARAM l_param,
	std::optional<ion::events::listeners::WindowAction> &action) noexcept
{
	switch (message)
	{
		case WM_ACTIVATE:
		action = LOWORD(w_param) == WA_INACTIVE ?
			ion::events::listeners::WindowAction::Deactivate :
			ion::events::listeners::WindowAction::Activate;
		return true;

		case WM_CREATE:
		action = ion::events::listeners::WindowAction::Open;
		return true;

		case WM_CLOSE:
		PostQuitMessage(0);
		action = ion::events::listeners::WindowAction::Close;
		return true;

		case WM_SYSCOMMAND:
		{
			switch (w_param)
			{
				case SC_MAXIMIZE:
				action = ion::events::listeners::WindowAction::Maximize;
				break;

				case SC_MINIMIZE:
				action = ion::events::listeners::WindowAction::Minimize;
				break;

				case SC_RESTORE:
				action = ion::events::listeners::WindowAction::Restore;
				break;
			}

			return false; //Do not return true from this
		}

		case WM_SYSKEYDOWN:
		{
			//Don't register repeated key down
			if ((l_param & 0x40000000) == 0)
			{
				//Alt + Enter
				if (w_param == VK_RETURN)
				{
					Mode(mode_ == WindowMode::Windowed ?
						WindowMode::FullScreen :
						WindowMode::Windowed);
					return true;
				}
			}

			break;
		}
		
		case WM_MOVE: //Client position
		{
			if (mode_ != WindowMode::FullScreen)
			{
				auto rectangle = RECT{};
				rectangle.left = static_cast<short>(LOWORD(l_param));
				rectangle.top = static_cast<short>(HIWORD(l_param));

				rectangle =
					detail::get_window_rectangle(
						rectangle,
						detail::get_window_style(handle),
						detail::get_extended_window_style(handle));

				if (!position_)
				{
					if (auto centered_rectangle = detail::get_centered_window_rectangle(handle);
						rectangle.left == centered_rectangle.left &&
						rectangle.top == centered_rectangle.top)					
							return true; //Do not set position if centered
				}

				position_ = Vector2{static_cast<real>(rectangle.left), static_cast<real>(rectangle.top)};
			}

			action = ion::events::listeners::WindowAction::Move;
			return true;
		}
		
		case WM_SIZE: //Client size
		{
			auto size = Vector2{static_cast<real>(LOWORD(l_param)), static_cast<real>(HIWORD(l_param))};

			if (mode_ != WindowMode::FullScreen)
				size_ = size;

			window::detail::change_viewport(size);
			action = ion::events::listeners::WindowAction::Resize;
			return true;
		}

		case WM_GETMINMAXINFO:
		{
			if (mode_ != WindowMode::FullScreen && min_size_)
			{
				auto rectangle = detail::make_window_rectangle(*min_size_, position_, border_style_);
				auto minmax_info = reinterpret_cast<MINMAXINFO*>(l_param);
				minmax_info->ptMinTrackSize.x = rectangle.right;
				minmax_info->ptMinTrackSize.y = rectangle.bottom;
			}

			return true;
		}
	}

	return false;
}
#endif

//Public

Window::Window(std::string_view title, const Vector2 &size, const std::optional<Vector2> &min_size,
	const std::optional<Vector2> &full_screen_size, const std::optional<Vector2> &position,
	window::WindowMode mode, WindowBorderStyle border_style) :

	title_{title},
	size_{size},
	min_size_{min_size},
	full_screen_size_{full_screen_size},
	position_{position},

	mode_{mode},
	border_style_{border_style}
{
	//Empty
}


/*
	Static window conversions
*/

Window Window::Borderless(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position)
{
	return {title, size, {}, {}, position, WindowMode::Windowed, WindowBorderStyle::None};
}

Window Window::Dialog(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position)
{
	return {title, size, {}, {}, position, WindowMode::Windowed, WindowBorderStyle::Dialog};
}

Window Window::NonResizable(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position)
{
	return {title, size, {}, {}, position, WindowMode::Windowed, WindowBorderStyle::Single};
}

Window Window::Resizable(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position)
{
	return {title, size, {}, {}, position, WindowMode::Windowed, WindowBorderStyle::Sizeable};
}


/*
	System
*/

#ifdef ION_WIN32
LRESULT CALLBACK Window::Procedure(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) noexcept
{
	std::optional<ion::events::listeners::WindowAction> action;
	auto received = ProcessMessage(handle, message, w_param, l_param, action);

	for (auto &listener : MessageEvents().Listeners())
		received |= listener.MessageReceived(handle, message, w_param, l_param);

	if (action)
	{
		for (auto &listener : Events().Listeners())
			listener.WindowActionReceived(*action);
	}

	//Call the default system procedure for handling all messages not received
	return received ? 0 : DefWindowProc(handle, message, w_param, l_param);
}
#endif


/*
	Create/destroy
*/

#ifdef ION_WIN32
bool Window::Create(HINSTANCE instance) noexcept
#else
bool Window::Create() noexcept
#endif
{
	#ifdef ION_WIN32
	if (!class_ && !handle_ && !dev_context_ && !gl_context_)
	{
		class_.reset({instance});
		handle_.reset({class_, title_, size_, position_, border_style_, this});
		dev_context_.reset({handle_, color_depth_});
		gl_context_.reset({dev_context_});
	}

	return class_ && handle_ && dev_context_ && gl_context_;
	#else
	return false;
	#endif
}

void Window::Destroy() noexcept
{
	#ifdef ION_WIN32
	gl_context_.reset({});
	dev_context_.reset({});
	handle_.reset({});
	class_.reset({});
	#endif
}


/*
	Show/hide
*/

#ifdef ION_WIN32
bool Window::Show(int cmd_show) noexcept
#else
bool Window::Show() noexcept
#endif
{
	#ifdef ION_WIN32
	if (handle_ && detail::show_window(cmd_show, *handle_))
	{
		if (cursor_ == WindowCursor::None)
			detail::change_cursor(cursor_);

		if (mode_ == WindowMode::FullScreen &&
			!detail::enter_full_screen_mode(full_screen_size_, color_depth_, *handle_))
			mode_ = WindowMode::Windowed;

		return true;
	}
	else
		return false;
	#else
	return false;
	#endif
}

bool Window::Hide() noexcept
{
	#ifdef ION_WIN32
	if (handle_)
	{
		if (mode_ == WindowMode::FullScreen)
			detail::exit_full_screen_mode(size_, position_, border_style_, *handle_);

		return detail::hide_window(*handle_);
	}
	#endif
	return false;
}

} //ion::system