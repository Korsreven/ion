/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render/vertex
File:	IonVertexBufferView.h
-------------------------------------------
*/

#ifndef ION_VERTEX_BUFFER_VIEW_H
#define ION_VERTEX_BUFFER_VIEW_H

#include <optional>
#include "IonVertexDataView.h"

namespace ion::graphics::render::vertex
{
	class VertexBufferObject; //Forward declaration

	namespace vertex_buffer_view::detail
	{
	} //vertex_buffer_view::detail


	///@brief A class representing a sub range (view) of a vertex buffer object
	class VertexBufferView final
	{
		private:

			std::optional<int> handle_;
			int offset_ = 0;
			int size_ = 0;

		public:

			///@brief Default constructor
			VertexBufferView() = default;

			///@brief Constructs a new vertex buffer view with the given vertex_buffer and offset
			VertexBufferView(const VertexBufferObject &vertex_buffer, int offset = 0) noexcept;

			///@brief Constructs a new vertex buffer view with the given vertex_buffer, offset and size
			VertexBufferView(const VertexBufferObject &vertex_buffer, int offset, int size) noexcept;

			///@brief Constructs a new vertex buffer view with the given handle, offset and size
			VertexBufferView(int handle, int offset, int size) noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Checks if two vertex buffer views are equal (handles, offsets and sizes are equal)
			[[nodiscard]] inline auto operator==(const VertexBufferView &rhs) const noexcept
			{
				return handle_ == rhs.handle_ && offset_ == rhs.offset_ && size_ == rhs.size_;
			}

			///@brief Checks if two vertex buffer objects are different (handles, offsets and sizes are different)
			[[nodiscard]] inline auto operator!=(const VertexBufferView &rhs) const noexcept
			{
				return !(*this == rhs);
			}

			///@brief Returns true if vertex buffer view handle is valid
			[[nodiscard]] inline operator bool() const noexcept
			{
				return handle_.has_value();
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Binds the given vertex buffer view
			void Bind() noexcept;

			///@brief Unbinds the given vertex buffer view
			void Unbind() noexcept;


			///@brief Sets the buffer data for this vertex buffer view to the given vertex data with offset
			void Data(const VertexDataView &vertex_data, int offset = 0) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the handle to this vertex buffer view
			///@details Returns nullopt if this vertex buffer view has no handle
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}

			///@brief Returns the offset of this vertex buffer view
			[[nodiscard]] inline auto Offset() const noexcept
			{
				return offset_;
			}

			///@brief Returns the size of this vertex buffer view
			[[nodiscard]] inline auto Size() const noexcept
			{
				return size_;
			}

			///@}
	};
} //ion::graphics::render::vertex

#endif