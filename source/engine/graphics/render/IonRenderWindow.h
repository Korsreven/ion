/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonRenderWindow.h
-------------------------------------------
*/

#ifndef ION_RENDER_WINDOW_H
#define ION_RENDER_WINDOW_H

#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "IonRenderTarget.h"
#include "events/IonListenable.h"
#include "events/listeners/IonWindowListener.h"
#include "graphics/utilities/IonVector2.h"
#include "system/IonSystemWindow.h"

namespace ion::graphics::render
{
	using graphics::utilities::Vector2;

	namespace render_window
	{
		enum class WindowDisplayMode : bool
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
			inline auto clamp_size(const Vector2 &size, const Vector2 &min_size) noexcept
			{
				auto [width, height] = size.XY();
				auto [min_width, min_height] = min_size.XY();
				return Vector2{width < min_width ? min_width : width,
								height < min_height ? min_height : height};
			}
		} //detail
	} //render_window


	class RenderWindow final :	
		public RenderTarget,
		public events::Listenable<events::listeners::WindowListener>,
		protected system::Window
	{
		private:

			using WindowEventsBase = events::Listenable<events::listeners::WindowListener>;
			using RenderTargetEventsBase = events::Listenable<events::listeners::RenderTargetListener>;
			using MessageEventsBase = ion::events::Listenable<system::events::listeners::MessageListener>;


			std::string title_;
			Vector2 size_;
			std::optional<Vector2> min_size_;
			std::optional<Vector2> full_screen_size_;
			std::optional<Vector2> position_;
			int color_depth_ = 32;
			
			render_window::WindowDisplayMode display_mode_ = render_window::WindowDisplayMode::Windowed;
			render_window::WindowDisplayMode pending_display_mode_ = render_window::WindowDisplayMode::Windowed;
			render_window::WindowBorderStyle border_style_ = render_window::WindowBorderStyle::Sizeable;
			render_window::WindowCursor cursor_ = render_window::WindowCursor::Default;		


			/*
				Notifying
			*/

			void NotifyWindowActionReceived(events::listeners::WindowAction action) noexcept;
			void NotifyWindowMoved(const Vector2 &position) noexcept;
			void NotifyWindowResized(const Vector2 &size) noexcept;


			/*
				Window events
			*/

			//See Window::Opened for more details
			void Opened() noexcept override final;

			//See Window::Closed for more details
			void Closed() noexcept override final;


			//See Window::Activated for more details
			void Activated() noexcept override final;

			//See Window::Deactivated for more details
			void Deactivated() noexcept override final;


			//See Window::Maximized for more details
			void Maximized() noexcept override final;

			//See Window::Minimized for more details
			void Minimized() noexcept override final;

			//See Window::Restored for more details
			void Restored() noexcept override final;


			//See Window::Moved for more details
			void Moved(const Vector2 &position) noexcept override final;

			//See Window::Resized for more details
			void Resized(const Vector2 &size) noexcept override final;

			//See Window::DisplayModeChanged for more details
			void DisplayModeChanged() noexcept override final;


			//See Window::GetFullScreenSize for more details
			std::optional<Vector2> GetFullScreenSize() const noexcept override final;

			//See Window::GetSizeConstraints for more details
			std::pair<std::optional<Vector2>, std::optional<Vector2>> GetSizeConstraints() const noexcept override final;


			

		public:

			//Default constructor
			RenderWindow() = default;

			//Construct a new window with the given arguments
			RenderWindow(std::string_view title,
				const Vector2 &size, const std::optional<Vector2> &min_size,
				const std::optional<Vector2> &full_screen_size, const std::optional<Vector2> &position,
				render_window::WindowDisplayMode display_mode, render_window::WindowBorderStyle border_style,
				render_window::WindowCursor cursor, int color_depth = 32);

			//Deleted copy constructor
			RenderWindow(const RenderWindow&) = delete;

			//Default move constructor
			RenderWindow(RenderWindow &&rhs) = default;


			/*
				Static window conversions
			*/

			//Returns a new borderless window from the given title, size and position
			[[nodiscard]] static RenderWindow Borderless(std::string_view title, const Vector2 &size,
				const std::optional<Vector2> &position = std::nullopt);

			//Returns a new dialog window from the given title, size and position
			[[nodiscard]] static RenderWindow Dialog(std::string_view title, const Vector2 &size,
				const std::optional<Vector2> &position = std::nullopt);

			//Returns a new non resizable window from the given title, size and position
			[[nodiscard]] static RenderWindow NonResizable(std::string_view title, const Vector2 &size,
				const std::optional<Vector2> &position = std::nullopt);

			//Returns a new resizable window from the given title, size and position
			[[nodiscard]] static RenderWindow Resizable(std::string_view title, const Vector2 &size,
				const std::optional<Vector2> &position = std::nullopt);


			/*
				Operators
			*/

			//Deleted copy assignment
			RenderWindow& operator=(const RenderWindow&) = delete;

			//Default move assignment
			RenderWindow& operator=(RenderWindow&&) = default;


			/*
				Events
			*/

			//Return a mutable reference to the window events of this render window
			[[nodiscard]] inline auto& Events() noexcept
			{
				return static_cast<WindowEventsBase&>(*this);
			}

			//Return a immutable reference to the window events of this render window
			[[nodiscard]] inline auto& Events() const noexcept
			{
				return static_cast<const WindowEventsBase&>(*this);
			}


			//Return a mutable reference to the render target events of this render window
			[[nodiscard]] inline auto& RenderTargetEvents() noexcept
			{
				return static_cast<RenderTargetEventsBase&>(*this);
			}

			//Return a immutable reference to the render target events of this render window
			[[nodiscard]] inline auto& RenderTargetEvents() const noexcept
			{
				return static_cast<const RenderTargetEventsBase&>(*this);
			}


			//Return a mutable reference to the message events of this render window
			[[nodiscard]] inline auto& MessageEvents() noexcept
			{
				return static_cast<MessageEventsBase&>(*this);
			}

			//Return a immutable reference to the message events of this render window
			[[nodiscard]] inline auto& MessageEvents() const noexcept
			{
				return static_cast<const MessageEventsBase&>(*this);
			}
			

			//See RenderTarget::DoSwapBuffers for more details
			void DoSwapBuffers() noexcept override final;

			//See RenderTarget::GetRenderTargetSize for more details
			Vector2 GetRenderTargetSize() const noexcept override final;


			/*
				Modifiers
			*/

			//Sets the window title to the given title
			inline void Title(std::string_view title) noexcept
			{
				if (title_ != title)
				{
					title_ = title;
					system::Window::Title(title);
				}
			}

			//Sets the window size to the given size
			inline void Size(const Vector2 &size) noexcept
			{
				if (size_ != size)
				{
					size_ = min_size_ ? render_window::detail::clamp_size(size, *min_size_) : size;
					system::Window::InnerSize(size_);

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
						if (auto size = render_window::detail::clamp_size(size_, *min_size); size != size_)
						{
							size_ = size;
							system::Window::InnerSize(size);

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
					full_screen_size_ = full_screen_size;
			}

			//Sets a custom window position to the given size
			inline void Position(const std::optional<Vector2> &position) noexcept
			{
				if (position_ != position)
				{
					if ((position_ = position))
						system::Window::OuterPosition(*position);
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


			//Sets the window display mode to the given mode
			inline void DisplayMode(render_window::WindowDisplayMode display_mode) noexcept
			{
				if (display_mode_ != display_mode)
				{
					pending_display_mode_ = display_mode;

					switch (display_mode)
					{
						case render_window::WindowDisplayMode::FullScreen:
						EnterFullScreen(full_screen_size_);
						break;

						case render_window::WindowDisplayMode::Windowed:
						default:
						ExitFullScreen();
						break;
					}
				}
			}

			//Sets the window border style to the given style
			inline void BorderStyle(render_window::WindowBorderStyle border_style) noexcept
			{
				if (border_style_ != border_style)
				{
					border_style_ = border_style;
					
					switch (border_style)
					{
						case render_window::WindowBorderStyle::None:
						BorderlessStyle();
						break;

						case render_window::WindowBorderStyle::Dialog:
						DialogStyle();
						break;

						case render_window::WindowBorderStyle::Single:
						SingleBorderStyle();
						break;

						case render_window::WindowBorderStyle::Sizeable:
						default:
						SizeableBorderStyle();			
						break;
					}
				}
			}

			//Sets the window cursor to the given cursor
			inline void Cursor(render_window::WindowCursor cursor) noexcept
			{
				if (cursor_ != cursor)
				{
					cursor_ = cursor;
					
					switch (cursor)
					{
						case render_window::WindowCursor::None:
						HideCursor();
						break;

						case render_window::WindowCursor::Default:
						default:
						ShowCursor();
						break;
					}
				}
			}


			/*
				Observers
			*/

			//Returns the window title
			[[nodiscard]] inline auto& Title() const noexcept
			{
				return title_;
			}

			//Returns the size of the window
			[[nodiscard]] inline auto& Size() const noexcept
			{
				return size_;
			}

			//Returns the min allowed client size of the window
			//Returns nullopt if there is not a constraint on min size
			[[nodiscard]] inline auto& MinSize() const noexcept
			{
				return min_size_;
			}

			//Returns the full screen size of the window
			//Returns nullopt if no custom full screen size specified
			[[nodiscard]] inline auto& FullScreenSize() const noexcept
			{
				return full_screen_size_;
			}

			//Returns the position of the window
			//Returns nullopt if the window does not have a custom position (centered)
			[[nodiscard]] inline auto& Position() const noexcept
			{
				return position_;
			}

			//Returns the window color depth
			[[nodiscard]] inline auto ColorDepth() const noexcept
			{
				return color_depth_;
			}


			//Returns the window display mode
			[[nodiscard]] inline auto DisplayMode() const noexcept
			{
				return display_mode_;
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
				Buffers
			*/

			//Exchanges the front and back buffers
			void SwapBuffers() noexcept;


			/*
				Create/destroy
			*/

			//Creates a window
			bool Create() noexcept;

			//Destroyes a window (if existing)
			void Destroy() noexcept;


			/*
				Show/hide
			*/

			//Shows and focus the window
			//Makes the window visible
			bool Show() noexcept;

			//Hides the window
			//Makes the window not visible
			bool Hide() noexcept;


			/*
				Messages
			*/

			//Process all messages in the message queue
			//Returns false if a quit message has been received
			bool ProcessMessages() noexcept;


			/*
				System window (shadowing)
				Modifiers
			*/

			//Focuses the window by bringing it to the front
			inline void Focus() noexcept
			{
				system::Window::Focus();
			}

			//Sets the window position to the center of the main desktop
			inline void Center() noexcept
			{
				position_.reset();
				system::Window::Center();
			}


			/*
				System window (shadowing)
				Observers
			*/

			//Returns the inner (client/screen) size of the window
			//Returns nullopt if no window has been created
			[[nodiscard]] inline auto InnerSize() const noexcept
			{
				return system::Window::InnerSize();
			}

			//Returns the outer size of the window
			//Returns nullopt if no window has been created
			[[nodiscard]] inline auto OuterSize() const noexcept
			{
				return system::Window::OuterSize();
			}

			//Returns the inner (client/screen) position of the window
			//Returns nullopt if no window has been created
			[[nodiscard]] inline auto InnerPosition() const noexcept
			{
				return system::Window::InnerPosition();
			}

			//Returns the outer position of the window
			//Returns nullopt if no window has been created
			[[nodiscard]] inline auto OuterPosition() const noexcept
			{
				return system::Window::OuterPosition();
			}


			//Returns true if the window is created
			[[nodiscard]] inline auto Created() const noexcept
			{
				return system::Window::Created();
			}

			//Returns true if the window is active (in focus)
			[[nodiscard]] inline auto IsActive() const noexcept
			{
				return system::Window::IsActive();
			}


			//Returns the native window handle
			//Returns nullptr if no window has been created
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return system::Window::Handle();
			}
	};
} //ion::graphics::render

#endif