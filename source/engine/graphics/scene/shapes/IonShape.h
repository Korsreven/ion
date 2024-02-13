/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonShape.h
-------------------------------------------
*/

#ifndef ION_SHAPE_H
#define ION_SHAPE_H

#include <optional>
#include <string>

#include "IonMesh.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/utilities/IonColor.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics
{
	namespace materials
	{
		class Material; //Forward declaration
	}
}

namespace ion::graphics::scene::shapes
{
	using namespace utilities;

	namespace shape::detail
	{
	} //shape::detail


	///@brief A class representing any kind of predefined shape
	///@details This base class must support inheritance (open set of shapes)
	class Shape : public Mesh
	{
		protected:

			Color color_;

			bool update_vertices_ = false;
			bool update_colors_ = false;
			bool update_opacity_ = false;


			///@brief Constructs a new shape with the given name, vertices, color and visibility
			///@details Can only be instantiated by derived
			Shape(std::optional<std::string> name, const mesh::Vertices &vertices,
				const Color &color, bool visible = true);

			///@brief Constructs a new shape with the given name, draw mode, vertices, color and visibility
			///@details Can only be instantiated by derived
			Shape(std::optional<std::string> name, vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices,
				const Color &color, bool visible = true);


			///@brief Constructs a new texturized shape with the given name, vertices, material, color and visibility
			///@details Can only be instantiated by derived
			Shape(std::optional<std::string> name, const mesh::Vertices &vertices,
				NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);

			///@brief Constructs a new texturized shape with the given name, draw mode, vertices, material, color and visibility
			///@details Can only be instantiated by derived
			Shape(std::optional<std::string> name, vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices,
				NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);


			///@brief Must be overridden to return the vertices of the derived shape
			virtual mesh::Vertices GetVertices() const noexcept = 0;


			/**
				@name Events
				@{
			*/

			virtual void BaseColorChanged() noexcept override;
			virtual void BaseOpacityChanged() noexcept override;

			///@}

		public:

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the color of this shape to the given color
			inline void FillColor(const Color &color) noexcept
			{
				if (color_ != color)
				{
					color_ = color;
					update_colors_ = true;
				}
			}

			///@brief Sets the opacity of this shape to the given opacity
			inline void FillOpacity(real opacity) noexcept
			{
				if (color_.A() != opacity)
				{
					color_.A(opacity);
					update_opacity_ = true;
				}
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the color of this shape
			[[nodiscard]] inline auto& FillColor() const noexcept
			{
				return color_;
			}

			///@brief Returns the opacity of this shape
			[[nodiscard]] inline auto FillOpacity() const noexcept
			{
				return color_.A();
			}

			///@}

			/**
				@name Preparing
				@{
			*/

			///@brief Refreshes this shape by generating new vertices
			inline void Refresh() noexcept
			{
				update_vertices_ = true;
			}


			///@brief Prepares this shape such that it is ready to be drawn
			///@details This function is typically called each frame
			virtual void Prepare() override;

			///@}
	};
} //ion::graphics::scene::shapes

#endif