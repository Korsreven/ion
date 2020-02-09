/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system
File:	IonSystemWindow.h
-------------------------------------------
*/

#ifndef ION_SYSTEM_WINDOW_H
#define ION_SYSTEM_WINDOW_H

#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "IonSystemAPI.h"
#include "events/listeners/IonListenerInterface.h"
#include "events/listeners/IonWindowListener.h"
#include "graphics/utilities/IonVector2.h"
#include "system/events/listeners/IonSystemMessageListener.h"

namespace ion::system
{
	using graphics::utilities::Vector2;

	namespace window
	{
		enum class WindowMode : bool
		{
			FullScreen,
			Windowed
		};

		enum class WindowBorderStyle
		{
			None,
			Dialog,
			Single,
			Sizeable
		};

		enum class WindowCursor : bool
		{
			None,
			Default
		};

		namespace detail
		{
			using namespace std::string_view_literals;
			constexpr auto class_name = "OpenGL"sv;

			inline auto clamp_size(const Vector2 &size, const Vector2 &min_size) noexcept
			{
				auto [width, height] = size.XY();
				auto [min_width, min_height] = min_size.XY();
				return Vector2{width < min_width ? min_width : width,
							   height < min_height ? min_height : height};
			}


			#ifdef ION_WIN32

			LRESULT CALLBACK window_procedure(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) noexcept;

			WNDCLASS make_window_class(std::string_view name, HINSTANCE instance) noexcept;
			PIXELFORMATDESCRIPTOR make_pixel_format_descriptor(int color_depth) noexcept;
			DEVMODE make_device_mode(const Vector2 &full_screen_size, int color_depth) noexcept;

			DWORD make_window_style(WindowBorderStyle border_style) noexcept;	
			DWORD make_extended_window_style(WindowBorderStyle border_style) noexcept;

			RECT get_desktop_rectangle() noexcept;
			RECT get_adjusted_window_rectangle(RECT rectangle, DWORD style, DWORD extended_style) noexcept;
			RECT get_non_client_window_rectangle(DWORD style, DWORD extended_style) noexcept;
			RECT get_total_window_rectangle(RECT client_rectangle, DWORD style, DWORD extended_style) noexcept;
			RECT get_client_window_rectangle(RECT total_rectangle, DWORD style, DWORD extended_style) noexcept;
			RECT get_centered_window_rectangle(RECT total_rectangle) noexcept;

			RECT make_window_rectangle(const Vector2 &size, const std::optional<Vector2> &position, WindowBorderStyle border_style) noexcept;


			template <typename T>
			class handle_base
			{
				protected:

					T handle_ = nullptr;

				public:

					handle_base() = default;
					handle_base(const handle_base&) = delete;

					handle_base(handle_base &&rhs) noexcept :
						handle_{std::exchange(rhs.handle_, nullptr)}
					{
						//Empty
					}

					virtual ~handle_base() = default;


					handle_base& operator=(const handle_base&) = delete;

					inline auto& operator=(handle_base &&rhs) noexcept
					{
						if (this != &rhs)
						{
							this->~handle_base();
							handle_ = std::exchange(rhs.handle_, nullptr);
						}

						return *this;
					}


					inline auto operator*() noexcept
					{
						return handle_;
					}

					inline const auto operator*() const noexcept
					{
						return handle_;
					}

					inline auto operator->() noexcept
					{
						return handle_;
					}

					inline const auto operator->() const noexcept
					{
						return handle_;
					}

					inline operator bool() const noexcept
					{
						return !!handle_;
					}
			};

			struct window_class : handle_base<HINSTANCE>
			{
				window_class() = default;
				window_class(HINSTANCE instance) noexcept;
				window_class(window_class &&rhs) noexcept;
				~window_class();

				window_class& operator=(window_class &&rhs) noexcept;
				void reset(window_class &&rhs) noexcept;
			};

			struct window_handle : handle_base<HWND>
			{
				window_handle() = default;
				window_handle(window_class &win_class,
					std::string_view title, const Vector2 &size, const std::optional<Vector2> &position,
					WindowBorderStyle border_style, LPVOID parameter) noexcept;
				window_handle(window_handle &&rhs) noexcept;
				~window_handle();

				window_handle& operator=(window_handle &&rhs) noexcept;
				void reset(window_handle &&rhs) noexcept;
			};

			class device_context : public handle_base<HDC>
			{
				private:

					window_handle *win_handle_ = nullptr;

				public:

					device_context() = default;
					device_context(window_handle &win_handle, int color_depth) noexcept;
					device_context(device_context &&rhs) noexcept;
					~device_context();

					device_context& operator=(device_context &&rhs) noexcept;
					void reset(device_context &&rhs) noexcept;
			};

			struct rendering_context : handle_base<HGLRC>
			{
				rendering_context() = default;
				rendering_context(device_context &dev_context) noexcept;
				rendering_context(rendering_context &&rhs) noexcept;
				~rendering_context();

				rendering_context& operator=(rendering_context &&rhs) noexcept;
				void reset(rendering_context &&rhs) noexcept;
			};


			bool enter_full_screen_mode(const std::optional<Vector2> &full_screen_size, int color_depth, HWND handle) noexcept;
			bool exit_full_screen_mode(const Vector2 &size, const std::optional<Vector2> &position, WindowBorderStyle border_style, HWND handle) noexcept;

			bool change_title(std::string_view title, HWND handle) noexcept;
			bool change_client_size(const Vector2 &size, HWND handle) noexcept;
			bool change_full_screen_size(WindowMode mode, const std::optional<Vector2> &full_screen_size, int color_depth, HWND handle) noexcept;		
			bool change_position(const Vector2 &position, HWND handle) noexcept;

			bool change_border_style(WindowBorderStyle border_style, HWND handle) noexcept;
			bool change_cursor(WindowCursor cursor) noexcept;

			bool show_window(int cmd_show, HWND handle) noexcept;
			bool hide_window(HWND handle) noexcept;
			bool center_window(HWND handle) noexcept;
			bool focus_window(HWND handle) noexcept;

			DWORD get_window_style(HWND handle) noexcept;
			DWORD get_extended_window_style(HWND handle) noexcept;
			RECT get_window_rectangle(HWND handle) noexcept;
			RECT get_client_window_rectangle(HWND handle) noexcept;
			RECT get_centered_window_rectangle(HWND handle) noexcept;

			Vector2 get_size(HWND handle) noexcept;
			Vector2 get_client_size(HWND handle) noexcept;
			Vector2 get_position(HWND handle) noexcept;
			bool is_active(HWND handle) noexcept;

			#endif


			void change_viewport(const Vector2 &size) noexcept;
		} //detail
	} //window


	class Window final :
		protected ion::events::listeners::ListenerInterface<events::listeners::MessageListener>,
		protected ion::events::listeners::ListenerInterface<ion::events::listeners::WindowListener>
	{
		private:

			std::string title_;
			Vector2 size_;
			std::optional<Vector2> min_size_;
			std::optional<Vector2> full_screen_size_;
			std::optional<Vector2> position_;
			int color_depth_ = 32;
			
			window::WindowMode mode_ = window::WindowMode::Windowed;
			window::WindowBorderStyle border_style_ = window::WindowBorderStyle::Single;
			window::WindowCursor cursor_ = window::WindowCursor::Default;
			
			#ifdef ION_WIN32
			window::detail::window_class class_;
			window::detail::window_handle handle_;
			window::detail::device_context dev_context_;
			window::detail::rendering_context gl_context_;
			#endif


			#ifdef ION_WIN32
			//Processes a message that is sent to the window
			bool ProcessMessage(HWND handle, UINT message, WPARAM w_param, LPARAM l_param,
				std::optional<ion::events::listeners::WindowAction> &action) noexcept;
			#endif

		protected:

			using MessageEventsBase = ion::events::listeners::ListenerInterface<events::listeners::MessageListener>;
			using WindowEventsBase = ion::events::listeners::ListenerInterface<ion::events::listeners::WindowListener>;

		public:

			//Default constructor
			Window() = default;

			//Construct a new window with the given arguments
			Window(std::string_view title, const Vector2 &size, const std::optional<Vector2> &min_size,
				const std::optional<Vector2> &full_screen_size, const std::optional<Vector2> &position,
				window::WindowMode mode, window::WindowBorderStyle border_style);

			//Deleted copy constructor
			Window(const Window&) = delete;

			//Default move constructor
			Window(Window &&rhs) = default;


			/*
				Static window conversions
			*/

			//Returns a new borderless window from the given title, size and position
			[[nodiscard]] static Window Borderless(std::string_view title, const Vector2 &size,
				const std::optional<Vector2> &position = std::nullopt);

			//Returns a new dialog window from the given title, size and position
			[[nodiscard]] static Window Dialog(std::string_view title, const Vector2 &size,
				const std::optional<Vector2> &position = std::nullopt);

			//Returns a new non resizable window from the given title, size and position
			[[nodiscard]] static Window NonResizable(std::string_view title, const Vector2 &size,
				const std::optional<Vector2> &position = std::nullopt);

			//Returns a new resizable window from the given title, size and position
			[[nodiscard]] static Window Resizable(std::string_view title, const Vector2 &size,
				const std::optional<Vector2> &position = std::nullopt);


			/*
				Operators
			*/

			//Deleted copy assignment
			Window& operator=(const Window&) = delete;

			//Default move assignment
			Window& operator=(Window&&) = default;


			/*
				Events
			*/

			//Return a mutable reference to the message events of this input listener
			[[nodiscard]] inline auto& MessageEvents() noexcept
			{
				return static_cast<MessageEventsBase&>(*this);
			}

			//Return a immutable reference to the message events of this input listener
			[[nodiscard]] inline const auto& MessageEvents() const noexcept
			{
				return static_cast<const MessageEventsBase&>(*this);
			}


			//Return a mutable reference to the window events of this input listener
			[[nodiscard]] inline auto& Events() noexcept
			{
				return static_cast<WindowEventsBase&>(*this);
			}

			//Return a immutable reference to the window events of this input listener
			[[nodiscard]] inline auto& Events() const noexcept
			{
				return static_cast<const WindowEventsBase&>(*this);
			}


			/*
				Modifiers
			*/

			//Sets the window position to the center of the main desktop
			inline void Center() noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::center_window(*handle_);
				#endif

				position_.reset();
			}

			//Focuses the window by bringing it to the front
			inline void Focus() noexcept
			{
				#ifdef ION_WIN32
				if (handle_)
					window::detail::focus_window(*handle_);
				#endif
			}


			//Sets the window title to the given title
			inline void Title(std::string_view title) noexcept
			{
				if (title_ != title)
				{
					title_ = title;

					#ifdef ION_WIN32
					if (handle_)
						window::detail::change_title(title, *handle_);
					#endif
				}
			}

			//Sets the window size to the given size
			inline void Size(const Vector2 &size) noexcept
			{
				if (size_ != size)
				{
					size_ = min_size_ ? window::detail::clamp_size(size, *min_size_) : size;

					#ifdef ION_WIN32
					if (handle_)
						window::detail::change_client_size(size_, *handle_);
					#endif

					if (!position_)
						Center();
				}
			}

			//Sets the window min size constraint to the given min size
			inline void MinSize(const std::optional<Vector2> &min_size) noexcept
			{
				if (min_size_ != min_size)
				{
					if ((min_size_ = min_size))
					{
						if (auto size = window::detail::clamp_size(size_, *min_size); size != size_)
						{
							size_ = size;

							#ifdef ION_WIN32
							if (handle_)
								window::detail::change_client_size(size_, *handle_);
							#endif

							if (!position_)
								Center();
						}
					}
				}
			}

			//Sets a custom full screen size to the given size
			inline void FullScreenSize(const std::optional<Vector2> &full_screen_size) noexcept
			{
				if (full_screen_size_ != full_screen_size)
				{
					full_screen_size_ = full_screen_size;

					#ifdef ION_WIN32
					if (handle_)
						window::detail::change_full_screen_size(mode_, full_screen_size, color_depth_, *handle_);
					#endif
				}
			}

			//Sets a custom window position to the given size
			inline void Position(const std::optional<Vector2> &position) noexcept
			{
				if (position_ != position)
				{
					if ((position_ = position))
					{
						#ifdef ION_WIN32
						if (handle_)
							window::detail::change_position(*position, *handle_);
						#endif
					}
					else
						Center();
				}
			}

			//Sets the window color depth to the given value
			//Changes will apply the next time the window is recreated
			inline void ColorDepth(int color_depth) noexcept
			{
				color_depth_ = color_depth;
			}


			//Sets the window mode to the given mode
			inline void Mode(window::WindowMode mode) noexcept
			{		
				if (mode_ != mode)
				{
					if (mode == window::WindowMode::FullScreen)
						mode_ = mode;

					#ifdef ION_WIN32
					if (handle_)
					{
						switch (mode)
						{
							case window::WindowMode::FullScreen:
							window::detail::enter_full_screen_mode(full_screen_size_, color_depth_, *handle_);
							break;

							case window::WindowMode::Windowed:
							default:
							window::detail::exit_full_screen_mode(size_, position_, border_style_, *handle_);
							break;
						}
					}
					#endif

					if (mode == window::WindowMode::Windowed)
						mode_ = mode;
				}	
			}

			//Sets the window border style to the given style
			inline void BorderStyle(window::WindowBorderStyle border_style) noexcept
			{
				if (border_style_ != border_style)
				{
					border_style_ = border_style;

					#ifdef ION_WIN32
					if (handle_)
						window::detail::change_border_style(border_style, *handle_);
					#endif
				}
			}

			//Sets the window cursore to the given cursor
			inline void Cursor(window::WindowCursor cursor) noexcept
			{
				if (cursor_ != cursor)
				{
					cursor_ = cursor;

					#ifdef ION_WIN32
					window::detail::change_cursor(cursor);
					#endif
				}
			}


			/*
				Observers
			*/

			//Returns the window title
			[[nodiscard]] inline const auto& Title() const noexcept
			{
				return title_;
			}

			//Returns the size of the window
			[[nodiscard]] inline const auto& Size() const noexcept
			{
				return size_;
			}

			//Returns the min allowed client size of the window
			//Returns nullopt if there is not a constraint on min size
			[[nodiscard]] inline const auto& MinSize() const noexcept
			{
				return min_size_;
			}

			//Returns the full screen size of the window
			//Returns nullopt if no custom full screen size specified
			[[nodiscard]] inline const auto& FullScreenSize() const noexcept
			{
				return full_screen_size_;
			}

			//Returns the position of the window
			//Returns nullopt if the window does not have a custom position (centered)
			[[nodiscard]] inline const auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the window color depth
			[[nodiscard]] inline auto ColorDepth() const noexcept
			{
				return color_depth_;
			}


			//Returns the window mode
			[[nodiscard]] inline auto Mode() const noexcept
			{
				return mode_;
			}

			//Returns the window border style
			[[nodiscard]] inline auto BorderStyle() const noexcept
			{
				return border_style_;
			}

			//Returns the window cursor
			[[nodiscard]] inline auto Cursor() const noexcept
			{
				return cursor_;
			}


			/*
				Window
			*/

			//Returns true if the window is active (in focus)
			[[nodiscard]] inline auto IsActive() const noexcept
			{
				#ifdef ION_WIN32
				return handle_ && window::detail::is_active(*handle_);
				#else
				return false;
				#endif
			}

			//Returns the client size of the window
			[[nodiscard]] inline auto ClientSize() const noexcept
			{
				if (mode_ == window::WindowMode::FullScreen)
				{
					#ifdef ION_WIN32
					if (handle_)
						return window::detail::get_client_size(*handle_);
					#endif
				}

				return size_;
			}

			//Returns the aspect ratio of the window
			[[nodiscard]] inline auto AspectRatio() const noexcept
			{
				auto [width, height] = ClientSize().XY();
				return width / height;
			}


			/*
				Handles
			*/

			//Returns the window device context
			[[nodiscard]] inline auto DeviceContext() const noexcept
			{
				return *dev_context_;
			}

			//Returns the window handle
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return *handle_;
			}


			/*
				System
			*/

			#ifdef ION_WIN32
			//An application-defined function that processes messages sent to a window
			LRESULT CALLBACK Procedure(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) noexcept;
			#endif


			/*
				Create/destroy
			*/

			//Creates an actual system specific window
			#ifdef ION_WIN32
			bool Create(HINSTANCE instance = nullptr) noexcept;
			#else
			bool Create() noexcept
			#endif

			//Destroyes the actual system specific window (if existing)
			void Destroy() noexcept;


			/*
				Show/hide
			*/

			//Shows and focus the actual system specific window
			//Makes the window visible
			#ifdef ION_WIN32
			bool Show(int cmd_show = SW_SHOW) noexcept;
			#else
			bool Show() noexcept;
			#endif

			//Hides the actual system specific window
			//Makes the window not visible
			bool Hide() noexcept;
	};

} //ion::system

#endif