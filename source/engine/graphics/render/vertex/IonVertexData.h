/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render/vertex
File:	IonVertexData.h
-------------------------------------------
*/

#ifndef ION_VERTEX_DATA_H
#define ION_VERTEX_DATA_H

#include <vector>

namespace ion::graphics::render::vertex
{
	namespace vertex_data
	{
		template <typename T>
		using VertexContainer = std::vector<T>;

		namespace detail
		{
		} //detail
	} //vertex_data


	class VertexData
	{
		private:

			const void *pointer_ = nullptr;
			int size_ = 0;
			int element_size_ = 0;

		public:

			//Default constructor
			VertexData() = default;

			//Construct a new vertex data with the given first vertex pointer and size of vertices
			template <typename T>
			VertexData(const T *first_vertex, int size) noexcept :

				pointer_{first_vertex},
				size_{size * sizeof(T)},
				element_size_{sizeof(T)}
			{
				//Empty
			}

			//Construct a new vertex data with the given vertices
			template <typename T>
			VertexData(const vertex_data::VertexContainer<T> &vertices) noexcept :

				pointer_{std::data(vertices)},
				size_{std::ssize(vertices) * sizeof(T)},
				element_size_{sizeof(T)}
			{
				//Empty
			}


			/*
				Operators
			*/

			//Checks if two vertex data are equal (pointers and sizes are equal)
			[[nodiscard]] inline auto operator==(const VertexData &rhs) const noexcept
			{
				return pointer_ == rhs.pointer_ && size_ == rhs.size_;
			}

			//Checks if two vertex data are different (pointers and sizes are different)
			[[nodiscard]] inline auto operator!=(const VertexData &rhs) const noexcept
			{
				return !(*this == rhs);
			}

			//Returns true if vertex data points to some vertices
			[[nodiscard]] inline operator bool() const noexcept
			{
				return !!pointer_;
			}


			/*
				Modifiers
			*/

			//Sets the vertex data pointer to the given first vertex and size of vertices
			template <typename T>
			inline void Pointer(const T *first_vertex, int size) noexcept
			{
				pointer_ = first_vertex;
				size_ = size * sizeof(T);
				element_size_ = sizeof(T);
			}

			//Sets the vertex data pointer to the given vertices
			template <typename T>
			inline void Pointer(const vertex_data::VertexContainer<T> &vertices) noexcept
			{
				pointer_ = std::data(vertices);
				size_ = std::ssize(vertices) * sizeof(T);
				element_size_ = sizeof(T);
			}


			/*
				Observers
			*/

			//Returns a pointer to the vertices of this vertex data
			[[nodiscard]] inline auto Pointer() const noexcept
			{
				return pointer_;
			}

			//Returns the size of the vertices of this vertex data
			[[nodiscard]] inline auto Size() const noexcept
			{
				return size_;
			}
			
			//Returns the size of each element of this vertex data
			[[nodiscard]] inline auto ElementSize() const noexcept
			{
				return element_size_;
			}
	};
} //ion::graphics::render::vertex

#endif