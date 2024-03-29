/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render/vertex
File:	IonVertexBufferObject.h
-------------------------------------------
*/

#ifndef ION_VERTEX_BUFFER_OBJECT_H
#define ION_VERTEX_BUFFER_OBJECT_H

#include <optional>
#include "IonVertexDataView.h"

namespace ion::graphics::render::vertex
{
	class VertexBufferView; //Forward declaration

	namespace vertex_buffer_object
	{
		enum class VertexBufferUsage
		{
			Static,		//Load/modify vertex data once
			Dynamic,	//Load/modify vertex data occasionally
			Stream		//Load/modify vertex data often (every frame)
		};


		namespace detail
		{
			int vertex_buffer_usage_to_gl_buffer_usage(VertexBufferUsage buffer_usage) noexcept;
			VertexBufferUsage gl_buffer_usage_to_vertex_buffer_usage(int buffer_usage) noexcept;


			/**
				@name Graphics API
				@{
			*/

			std::optional<int> create_vertex_buffer_object() noexcept;
			void delete_vertex_buffer_object(int vbo_handle) noexcept;
			void bind_vertex_buffer_object(int vbo_handle) noexcept;

			void set_vertex_buffer_data(int vbo_handle, const void *data, int size, VertexBufferUsage buffer_usage) noexcept;
			void set_vertex_buffer_sub_data(int vbo_handle, const void *data, int offset, int size) noexcept;

			int get_vertex_buffer_size(int vbo_handle) noexcept;
			VertexBufferUsage get_vertex_buffer_usage(int vbo_handle) noexcept;

			///@}
		} //detail
	} //vertex_buffer_object


	///@brief A class representing a vertex buffer object (VBO)
	class VertexBufferObject final
	{
		private:

			std::optional<int> handle_;
			vertex_buffer_object::VertexBufferUsage usage_ = vertex_buffer_object::VertexBufferUsage::Dynamic;	
			int size_ = 0;		

		public:

			///@brief Constructs a new vertex buffer object
			VertexBufferObject() noexcept;

			///@brief Constructs a new vertex buffer object with the given usage
			explicit VertexBufferObject(vertex_buffer_object::VertexBufferUsage usage) noexcept;

			///@brief Constructs a new vertex buffer object with an already existing handle
			explicit VertexBufferObject(int handle) noexcept;

			///@brief Copy constructor
			VertexBufferObject(const VertexBufferObject &rhs);

			///@brief Default move constructor
			VertexBufferObject(VertexBufferObject&&) = default;

			///@brief Destructor
			~VertexBufferObject() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Copy assignment
			inline auto& operator=(const VertexBufferObject &rhs)
			{
				usage_ = rhs.usage_;
				return *this;
			}

			///@brief Default move assignment
			VertexBufferObject& operator=(VertexBufferObject&&) = default;


			///@brief Checks if two vertex buffer objects are equal (handles are equal)
			[[nodiscard]] inline auto operator==(const VertexBufferObject &rhs) const noexcept
			{
				return handle_ == rhs.handle_;
			}

			///@brief Checks if two vertex buffer objects are different (handles are different)
			[[nodiscard]] inline auto operator!=(const VertexBufferObject &rhs) const noexcept
			{
				return !(*this == rhs);
			}

			///@brief Returns true if vertex buffer object handle is valid
			[[nodiscard]] inline operator bool() const noexcept
			{
				return handle_.has_value();
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Binds the given vertex buffer object
			void Bind() noexcept;

			///@brief Unbinds the given vertex buffer object
			void Unbind() noexcept;


			///@brief Sets the buffer data for this vertex buffer object to the given vertex data
			void Data(const VertexDataView &vertex_data) noexcept;

			///@brief Sets the buffer data for this vertex buffer object to the given vertex data with a new usage pattern
			void Data(const VertexDataView &vertex_data, vertex_buffer_object::VertexBufferUsage usage) noexcept;

			///@brief Sets the buffer data for this vertex buffer object to the given vertex data with offset
			void Data(const VertexDataView &vertex_data, int offset) noexcept;


			///@brief Reserves the buffer data for this vertex buffer object to the size
			void Reserve(int size) noexcept;

			///@brief Reserves the buffer data for this vertex buffer object to the given size with a new usage pattern
			void Reserve(int size, vertex_buffer_object::VertexBufferUsage usage) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the handle to this vertex buffer object
			///@details Returns nullopt if this vertex buffer has no handle
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}

			///@brief Returns the usage pattern of this vertex buffer object
			[[nodiscard]] inline auto Usage() const noexcept
			{
				return usage_;
			}

			///@brief Returns the size of this vertex buffer object
			[[nodiscard]] inline auto Size() const noexcept
			{
				return size_;
			}

			///@}

			/**
				@name Vertex buffer view
				@{
			*/

			///@brief Returns a vertex buffer view of this vertex buffer object in range [offset, offset + size]
			VertexBufferView SubBuffer(int offset, int size) const noexcept;

			///@}
	};
} //ion::graphics::render::vertex

#endif