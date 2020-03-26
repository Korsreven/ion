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


RECT make_window_rectangle(const Vector2 &size, const std::optional<Vector2> &position, DWORD style, DWORD extended_style) noexcept
{
	auto [width, height] = size.XY();
	auto rectangle =
		get_window_rectangle(
			{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)},
			style,
			extended_style);

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


//Window class

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


//Window handle

window_handle::window_handle(window_class &win_class,
	std::string_view title, const Vector2 &size, const std::optional<Vector2> &position,
	DWORD style, DWORD extended_style, LPVOID parameter) noexcept
{
	if (win_class)
	{
		auto rectangle =
			make_window_rectangle(size, position, style, extended_style);

		handle_ =
			CreateWindowEx(
				extended_style,
				std::data(class_name), std::data(title),
				style | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
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


//Device context

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
	handle_base{std::move(rhs)},
	win_handle_{std::exchange(rhs.win_handle_, nullptr)}
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


//Rendering context

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


//Full screen

full_screen::full_screen(
	window_handle &win_handle, device_context &dev_context,
	const std::optional<Vector2> &full_screen_size) noexcept
{
	if (win_handle && dev_context)
	{
		auto size = full_screen_size ?
			*full_screen_size :
			get_desktop_size();
		auto color_depth =
			GetDeviceCaps(*dev_context, BITSPIXEL) *
			GetDeviceCaps(*dev_context, PLANES);

		if (auto dev_mode = make_device_mode(size, color_depth);
			ChangeDisplaySettings(&dev_mode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
		{
			handle_ = *win_handle;
			size_ = get_client_size(handle_);
			position_ = get_position(handle_);
			style_ = get_window_style(handle_);
			extended_style_ = get_extended_window_style(handle_);

			auto [style, extended_style] = get_borderless_style();
			set_extents_and_border_style(size, graphics::utilities::vector2::Zero, style, extended_style, handle_);
		}
	}
}

full_screen::full_screen(full_screen &&rhs) noexcept :
	handle_base{std::move(rhs)},
	size_{rhs.size_},
	position_{rhs.position_},
	style_{rhs.style_},
	extended_style_{rhs.extended_style_}
{
	//Empty
}

full_screen::~full_screen()
{
	if (handle_)
	{
		if (ChangeDisplaySettings(nullptr, 0) == DISP_CHANGE_SUCCESSFUL)
			set_extents_and_border_style(size_, position_, style_, extended_style_, handle_);
	}
}

full_screen& full_screen::operator=(full_screen &&rhs) noexcept
{
	handle_base::operator=(std::move(rhs));

	if (this != &rhs)
	{
		size_ = rhs.size_;
		position_ = rhs.position_;
		style_ = rhs.style_;
		extended_style_ = rhs.extended_style_;
	}

	return *this;
}

void full_screen::reset(full_screen &&rhs) noexcept
{
	*this = std::move(rhs);
}


bool set_title(std::string_view title, HWND handle) noexcept
{
	return SetWindowText(handle, std::data(title)) != 0;
}

bool set_size(const Vector2 &size, HWND handle) noexcept
{
	auto [width, height] = size.XY();
	auto [x, y] = get_position(handle).XY();
	return MoveWindow(
		handle,
		static_cast<LONG>(x),
		static_cast<LONG>(y),
		static_cast<LONG>(width),
		static_cast<LONG>(height), 1) != 0;
}

bool set_client_size(const Vector2 &size, HWND handle) noexcept
{
	auto [width, height] = size.XY();
	auto rectangle =
		get_window_rectangle(
			{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)},
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

bool set_position(const Vector2 &position, HWND handle) noexcept
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

bool set_client_position(const Vector2 &position, HWND handle) noexcept
{
	auto [x, y] = position.XY();
	auto rectangle =
		get_window_rectangle(
			{static_cast<LONG>(x), static_cast<LONG>(y), 0, 0},
			get_window_style(handle),
			get_extended_window_style(handle));

	auto [width, height] = get_size(handle).XY();
	return MoveWindow(
		handle,
		static_cast<LONG>(rectangle.left),
		static_cast<LONG>(rectangle.top),
		static_cast<LONG>(width),
		static_cast<LONG>(height), 1) != 0;
}

bool set_extents_and_border_style(const Vector2 &size, const Vector2 &position,
	DWORD style, DWORD extended_style, HWND handle) noexcept
{
	SetWindowLong(handle, GWL_STYLE, style);
	SetWindowLong(handle, GWL_EXSTYLE, extended_style);

	auto [width, height] = size.XY();
	auto rectangle =
		get_window_rectangle(
			{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)},
			get_window_style(handle),
			get_extended_window_style(handle));

	auto [x, y] = position.XY();
	return SetWindowPos(
		handle, nullptr,
		static_cast<LONG>(x),
		static_cast<LONG>(y),
		rectangle.right,
		rectangle.bottom,
		SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW) != 0;
}


bool set_border_style(DWORD style, DWORD extended_style, HWND handle) noexcept
{
	SetWindowLong(handle, GWL_STYLE, style);
	SetWindowLong(handle, GWL_EXSTYLE, extended_style);

	auto rectangle = get_window_rectangle(handle);
	return SetWindowPos(
		handle, nullptr,
		0, 0,
		rectangle.right - rectangle.left,
		rectangle.bottom - rectangle.top,	
		SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW) != 0;
}

bool set_borderless_style(HWND handle) noexcept
{
	auto [style, extended_style] = get_borderless_style();
	return set_border_style(style, extended_style, handle);
}

bool set_dialog_border_style(HWND handle) noexcept
{
	auto [style, extended_style] = get_dialog_style();
	return set_border_style(style, extended_style, handle);
}

bool set_single_border_style(HWND handle) noexcept
{
	auto [style, extended_style] = get_single_border_style();
	return set_border_style(style, extended_style, handle);
}

bool set_sizeable_border_style(HWND handle) noexcept
{
	auto [style, extended_style] = get_sizeable_border_style();
	return set_border_style(style, extended_style, handle);
}


bool show_cursor() noexcept
{
	for (auto counter = ShowCursor(1); counter < 0;)
	{
		if (auto prev_counter = counter;
			(counter = ShowCursor(1)) == prev_counter)
			return false;
	}

	return true;
}

bool hide_cursor() noexcept
{
	for (auto counter = ShowCursor(0); counter >= 0;)
	{
		if (auto prev_counter = counter;
			(counter = ShowCursor(0)) == prev_counter)
			return false;
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
	return get_centered_window_rectangle({0, 0, static_cast<LONG>(width), static_cast<LONG>(height)});
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

Vector2 get_desktop_size() noexcept
{
	auto rectangle = get_desktop_rectangle();
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

bool is_centered(HWND handle) noexcept
{
	auto [x, y] = get_position(handle).XY();
	auto centered_rectangle = get_centered_window_rectangle(handle);
	return static_cast<LONG>(x) == centered_rectangle.left &&
		   static_cast<LONG>(y) == centered_rectangle.top;
}

#endif

} //window::detail


//Private

#ifdef ION_WIN32
bool Window::ProcessMessage(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) noexcept
{
	switch (message)
	{
		case WM_ACTIVATE:
		{
			switch (LOWORD(w_param))
			{
				case WA_ACTIVE:
				Activated();
				break;

				case WA_INACTIVE:
				Deactivated();
				break;
			}

			return true;
		}

		case WM_CREATE:
		Opened();
		return true;

		case WM_CLOSE:
		PostQuitMessage(0);
		Closed();
		return true;

		case WM_SYSCOMMAND:
		{
			switch (w_param)
			{
				case SC_MAXIMIZE:
				Maximized();
				break;

				case SC_MINIMIZE:
				Minimized();
				break;

				case SC_RESTORE:
				Restored();
				break;
			}

			return false; //Do not return true from this
		}

		case WM_SYSKEYDOWN:
		{
			//Don't register repeated key down
			if ((l_param & 0x40000000) == 0)
			{
				//Alt + Enter (change dispaly mode)
				if (w_param == VK_RETURN)
				{
					if (InFullScreen())
						ExitFullScreen();
					else
						EnterFullScreen(GetFullScreenSize());

					return true;
				}
			}

			break;
		}
		
		case WM_MOVE: //Client position
		{
			auto rectangle =
				detail::get_window_rectangle(
					RECT{static_cast<short>(LOWORD(l_param)), static_cast<short>(HIWORD(l_param)), 0, 0},
					detail::get_window_style(handle),
					detail::get_extended_window_style(handle));

			Moved({static_cast<real>(rectangle.left), static_cast<real>(rectangle.top)});
			return true;
		}
		
		case WM_SIZE: //Client size
		{
			PostMessage(handle, events::listeners::WM_GLSIZE, w_param, l_param);
			Resized({static_cast<real>(LOWORD(l_param)), static_cast<real>(HIWORD(l_param))});
			return true;
		}

		case WM_GETMINMAXINFO:
		{
			auto [min_size, max_size] = GetSizeConstraints();

			if (min_size)
			{
				auto rectangle =
					detail::make_window_rectangle(
						*min_size,
						detail::get_position(handle),
						detail::get_window_style(handle),
						detail::get_extended_window_style(handle));

				if (auto min_max_info = reinterpret_cast<MINMAXINFO*>(l_param); min_max_info)
				{
					min_max_info->ptMinTrackSize.x = rectangle.right;
					min_max_info->ptMinTrackSize.y = rectangle.bottom;
				}
			}

			if (max_size)
			{
				auto rectangle =
					detail::make_window_rectangle(
						*max_size,
						detail::get_position(handle),
						detail::get_window_style(handle),
						detail::get_extended_window_style(handle));

				if (auto min_max_info = reinterpret_cast<MINMAXINFO*>(l_param); min_max_info)
				{
					min_max_info->ptMaxTrackSize.x = rectangle.right;
					min_max_info->ptMaxTrackSize.y = rectangle.bottom;
				}
			}

			return true;
		}
	}

	return false;
}
#endif


/*
	Notifying
*/

#ifdef ION_WIN32
bool Window::NotifyMessageReceived(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) noexcept
{
	auto received = false;
	for (auto &listener : Listeners())
		received |= Notify(&system::events::listeners::MessageListener::MessageReceived, listener, handle, message, w_param, l_param).value_or(false);

	return received;
}
#endif

/*
*/

bool Window::DoCreate(std::string_view title, const Vector2 &size,
	const std::optional<Vector2> &position, int color_depth,
	DWORD style, DWORD extended_style) noexcept
{
	#ifdef ION_WIN32
	if (!class_ && !handle_ && !dev_context_ && !gl_context_)
	{
		class_.reset({nullptr});
		handle_.reset({class_, title, size, position, style, extended_style, this});
		dev_context_.reset({handle_, color_depth});
		gl_context_.reset({dev_context_});
	}

	return class_ && handle_ && dev_context_ && gl_context_;
	#else
	return false;
	#endif
}

void Window::DoDestroy() noexcept
{
	ExitFullScreen();

	#ifdef ION_WIN32
	gl_context_.reset({});
	dev_context_.reset({});
	handle_.reset({});
	class_.reset({});
	#endif
}


/*
	Window events
*/

void Window::Opened() noexcept
{
	//Optional to override
}

void Window::Closed() noexcept
{
	//Optional to override
}


void Window::Activated() noexcept
{
	//Optional to override
}

void Window::Deactivated() noexcept
{
	//Optional to override
}


void Window::Maximized() noexcept
{
	//Optional to override
}

void Window::Minimized() noexcept
{
	//Optional to override
}

void Window::Restored() noexcept
{
	//Optional to override
}


void Window::Moved([[maybe_unused]] const Vector2 &position) noexcept
{
	//Optional to override
}

void Window::Resized([[maybe_unused]] const Vector2 &size) noexcept
{
	//Optional to override
}

void Window::DisplayModeChanged() noexcept
{
	//Optional to override
}


std::optional<Vector2> Window::GetFullScreenSize() const noexcept
{
	//Optional to override
	return {};
}

std::pair<std::optional<Vector2>, std::optional<Vector2>> Window::GetSizeConstraints() const noexcept
{
	//Optional to override
	return {};
}


//Public

/*
	Buffers
*/

void Window::SwapBuffers() const noexcept
{
	#ifdef ION_WIN32
	::SwapBuffers(*dev_context_);
	#endif
}


/*
	Create/destroy
*/

bool Window::CreateBorderless(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position, int color_depth) noexcept
{
	#ifdef ION_WIN32
	auto [style, extended_style] = detail::get_borderless_style();
	return DoCreate(title, size, position, color_depth, style, extended_style);
	#else
	return false;
	#endif
}

bool Window::CreateDialog(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position, int color_depth) noexcept
{
	#ifdef ION_WIN32
	auto [style, extended_style] = detail::get_dialog_style();
	return DoCreate(title, size, position, color_depth, style, extended_style);
	#else
	return false;
	#endif
}

bool Window::CreateNonResizable(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position, int color_depth) noexcept
{
	#ifdef ION_WIN32
	auto [style, extended_style] = detail::get_single_border_style();
	return DoCreate(title, size, position, color_depth, style, extended_style);
	#else
	return false;
	#endif
}

bool Window::CreateResizable(std::string_view title, const Vector2 &size, const std::optional<Vector2> &position, int color_depth) noexcept
{
	#ifdef ION_WIN32
	auto [style, extended_style] = detail::get_sizeable_border_style();
	return DoCreate(title, size, position, color_depth, style, extended_style);
	#else
	return false;
	#endif
}


void Window::Destroy() noexcept
{
	DoDestroy();
}


/*
	Show/hide
*/

bool Window::Show() noexcept
{
	#ifdef ION_WIN32
	return handle_ && detail::show_window(SW_SHOW, *handle_);
	#else
	return false;
	#endif
}

bool Window::Hide() noexcept
{
	#ifdef ION_WIN32
	if (handle_)
	{
		ExitFullScreen();
		return detail::hide_window(*handle_);
	}
	#endif
	return false;
}


/*
	Messages
*/

bool Window::ProcessMessages() noexcept
{
	auto quit = false;
	while (ProcessNextMessage(quit));
	return !quit;
}

bool Window::ProcessNextMessage(bool &quit) noexcept
{
	#ifdef ION_WIN32
	if (MSG msg; PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
	{
		if (msg.message == WM_QUIT)
		{
			quit = true;
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		return true;
	}
	#endif

	return false;
}

#ifdef ION_WIN32
LRESULT CALLBACK Window::Procedure(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) noexcept
{
	auto received =
		ProcessMessage(handle, message, w_param, l_param) |
		NotifyMessageReceived(handle, message, w_param, l_param);

	//Call the default system procedure for handling all messages not received
	return received ? 0 : DefWindowProc(handle, message, w_param, l_param);
}
#endif

} //ion::system