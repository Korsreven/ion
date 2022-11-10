/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render/vertex
File:	IonVertexDeclaration.h
-------------------------------------------
*/

#ifndef ION_VERTEX_DECLARATION_H
#define ION_VERTEX_DECLARATION_H

#include <optional>
#include <vector>

#include "adaptors/ranges/IonIterable.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/shaders/variables/IonShaderTypes.h"

namespace ion::graphics::render::vertex
{
	namespace vertex_declaration
	{
		enum class VertexElementType
		{
			Float1 = 1,	//1-component float
			Float2,		//2-components float (vec2)
			Float3,		//3-components float (vec3)
			Float4		//4-components float (vec4)
		};


		struct VertexElement final
		{
			shaders::shader_layout::AttributeName Name;
			VertexElementType Type;
			int Offset = 0;
			int Stride = 0;

			///@brief Constructs a new vertex element with the given attribute name (semantic), type, offset and stride
			VertexElement(shaders::shader_layout::AttributeName name, VertexElementType type, int offset = 0, int stride = 0) noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Checks if two vertex elements are equal (all members are equal)
			[[nodiscard]] inline auto operator==(const VertexElement &rhs) const noexcept
			{
				return Name == rhs.Name &&
					   Type == rhs.Type &&
					   Offset == rhs.Offset &&
					   Stride == rhs.Stride;
			}

			///@brief Checks if two vertex elements are different (one or more members are different)
			[[nodiscard]] inline auto operator!=(const VertexElement &rhs) const noexcept
			{
				return !(*this == rhs);
			}

			///@}

			/**
				@name Observer
				@{
			*/

			///@brief Returns the total number of components for this vertex element
			[[nodiscard]] int Components() const noexcept;

			///@}
		};

		using VertexElements = std::vector<VertexElement>;


		namespace detail
		{
			template <VertexElementType ElementType>
			struct glsl_type
			{
				using type = void;
			};

			template <>
			struct glsl_type<VertexElementType::Float1>
			{
				using type = float32;
			};

			template <>
			struct glsl_type<VertexElementType::Float2>
			{
				using type = shaders::variables::glsl::vec2;
			};

			template <>
			struct glsl_type<VertexElementType::Float3>
			{
				using type = shaders::variables::glsl::vec3;
			};

			template <>
			struct glsl_type<VertexElementType::Float4>
			{
				using type = shaders::variables::glsl::vec4;
			};


			template <VertexElementType ElementType>
			using glsl_type_t = typename glsl_type<ElementType>::type;
		} //detail
	} //vertex_declaration


	///@brief A class representing a vertex declaration containing multiple vertex elements
	///@details A vertex declaration describes the layout of where elements are located in the vertex data.
	///Each vertex elements has an attribute name, type, offset and stride
	class VertexDeclaration final
	{
		private:
			
			vertex_declaration::VertexElements vertex_elements_;
			std::optional<int> vertex_size_;

		public:

			///@brief Default constructor
			VertexDeclaration() = default;

			///@brief Constructs a new vertex declaration with the given vertex elements and vertex size (optional)
			explicit VertexDeclaration(vertex_declaration::VertexElements vertex_elements, std::optional<int> vertex_size = std::nullopt);


			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all vertex elements in this vertex declaration
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Elements() noexcept
			{
				return adaptors::ranges::Iterable<vertex_declaration::VertexElements&>{vertex_elements_};
			}

			///@brief Returns an immutable range of all vertex elements in this vertex declaration
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Elements() const noexcept
			{
				return adaptors::ranges::Iterable<const vertex_declaration::VertexElements&>{vertex_elements_};
			}

			///@}

			/**
				@name Operators
				@{
			*/

			///@brief Checks if two vertex declarations are equal (all elements are equal)
			[[nodiscard]] bool operator==(const VertexDeclaration &rhs) const noexcept;

			///@brief Checks if two vertex declarations are different (one or more elements are different)
			[[nodiscard]] bool operator!=(const VertexDeclaration &rhs) const noexcept;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the size of one vertex for this vertex declaration to the given size
			///@details If nullopt is passed the vertex size would be automatically calculated
			inline void VertexSize(std::optional<int> size) noexcept
			{
				vertex_size_ = size;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the total number of components for this vertex declaration
			[[nodiscard]] int Components() const noexcept;

			///@brief Returns the size of one vertex for this vertex declaration
			[[nodiscard]] inline auto VertexSize() const noexcept
			{
				return vertex_size_ ? *vertex_size_ : Components() * static_cast<int>(sizeof(real));
			}

			///@}

			/**
				@name Vertex elements
				Adding
				@{
			*/

			///@brief Creates a vertex element with the given attribute name (semantic), type, offset and stride
			void AddElement(shaders::shader_layout::AttributeName name, vertex_declaration::VertexElementType type, int offset = 0, int stride = 0);

			///@brief Creates a vertex element as a copy of the given vertex element
			void AddElement(const vertex_declaration::VertexElement &vertex_element);

			///@}

			/**
				@name Vertex elements
				Removing
				@{
			*/

			///@brief Clears all vertex elements from this vertex declaration
			void ClearElements() noexcept;

			///@}
	};
} //ion::graphics::render::vertex

#endif