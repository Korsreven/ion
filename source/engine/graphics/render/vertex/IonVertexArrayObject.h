/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render/vertex
File:	IonVertexArrayObject.h
-------------------------------------------
*/

#ifndef ION_VERTEX_ARRAY_OBJECT_H
#define ION_VERTEX_ARRAY_OBJECT_H

#include <optional>
#include <utility>

#include "IonVertexBufferView.h"
#include "IonVertexDeclaration.h"

namespace ion::graphics::render::vertex
{
	namespace vertex_array_object::detail
	{
		/**
			@name Graphics API
			@{
		*/

		std::optional<int> create_vertex_array_object() noexcept;
		void delete_vertex_array_object(int vao_handle) noexcept;
		void bind_vertex_array_object(int vao_handle) noexcept;

		void bind_vertex_attributes(const VertexDeclaration &vertex_declaration, int vao_handle, int vbo_handle, int vbo_offset) noexcept;

		void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset) noexcept;
		void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const void *data) noexcept;

		///@}
	} //vertex_array_object::detail


	///@brief A class representing a vertex array object (VAO)
	class VertexArrayObject final
	{
		private:

			std::optional<int> handle_;

		public:

			///@brief Constructor
			VertexArrayObject() noexcept;

			///@brief Copy constructor
			VertexArrayObject(const VertexArrayObject&) noexcept;

			///@brief Default move constructor
			VertexArrayObject(VertexArrayObject&&) = default;

			///@brief Destructor
			~VertexArrayObject() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Copy assignment
			inline auto& operator=(const VertexArrayObject&)
			{
				//Do nothing
				return *this;
			}

			///@brief Default move assignment
			VertexArrayObject& operator=(VertexArrayObject&&) = default;


			///@brief Checks if two vertex array objects are equal (handles are equal)
			[[nodiscard]] inline auto operator==(const VertexArrayObject &rhs) const noexcept
			{
				return handle_ == rhs.handle_;
			}

			///@brief Checks if two vertex array objects are different (handles are different)
			[[nodiscard]] inline auto operator!=(const VertexArrayObject &rhs) const noexcept
			{
				return !(*this == rhs);
			}

			///@brief Returns true if vertex array object handle is valid
			[[nodiscard]] inline operator bool() const noexcept
			{
				return handle_.has_value();
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Binds the given vertex array object
			void Bind() noexcept;

			///@brief Binds vertex attributes declared in the given declaration to the given vertex buffer and store bindings in this vertex array object
			void Bind(const VertexDeclaration &vertex_declaration, const VertexBufferView &vertex_buffer) noexcept;

			///@brief Unbinds the given vertex array object
			void Unbind() noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the handle to this vertex array object
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}

			///@}
	};
} //ion::graphics::render::vertex

#endif