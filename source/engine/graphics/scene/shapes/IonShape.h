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
		Color first_vertex_color(const mesh::Vertices &vertices) noexcept;
	} //shape::detail


	class Shape : public Mesh
	{
		protected:

			Color color_;


			//Construct a new shape with the given visibility
			//Can only be instantiated by derived
			explicit Shape(const mesh::Vertices &vertices, bool visible = true);

			//Construct a new shape with the given draw mode, vertices and visibility
			//Can only be instantiated by derived
			Shape(vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices, bool visible = true);


			//Construct a new texturized shape with the given vertices, material and visibility
			//Can only be instantiated by derived
			Shape(const mesh::Vertices &vertices, NonOwningPtr<materials::Material> material, bool visible = true);

			//Construct a new texturized shape with the given draw mode, vertices, material and visibility
			//Can only be instantiated by derived
			Shape(vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices,
				NonOwningPtr<materials::Material> material, bool visible = true);


			//Must be overridden in derived to return vertices
			virtual mesh::Vertices GetVertices() const noexcept = 0;

		public:

			/*
				Modifiers
			*/

			//Sets the solid color of this shape to the given color
			inline void SolidColor(const Color &color) noexcept
			{
				if (color_ != color)
				{
					color_ = color;
					Mesh::SurfaceColor(color);
				}
			}

			//Sets if this shape should be shown in wireframe or not
			inline void ShowWireframe(bool show) noexcept
			{
				Mesh::ShowWireframe(show);
			}

			//Sets the visibility of this shape to the given value
			inline void Visible(bool visible) noexcept
			{
				Mesh::Visible(visible);
			}


			/*
				Observers
			*/

			//Returns the solid color of this rectangle
			[[nodiscard]] inline auto& SolidColor() const noexcept
			{
				return color_;
			}

			//Returns true if this shape is shown in wireframe
			[[nodiscard]] inline auto ShowWireframe() const noexcept
			{
				return Mesh::ShowWireframe();
			}

			//Returns true if this shape is visible
			[[nodiscard]] inline auto Visible() const noexcept
			{
				return Mesh::Visible();
			}


			//Returns the local axis-aligned bounding box (AABB) for this shape
			[[nodiscard]] inline auto& AxisAlignedBoundingBox() const noexcept
			{
				return Mesh::AxisAlignedBoundingBox();
			}

			//Returns the local oriented bounding box (OBB) for this shape
			[[nodiscard]] inline auto& OrientedBoundingBox() const noexcept
			{
				return Mesh::OrientedBoundingBox();
			}

			//Returns the local bounding sphere for this shape
			[[nodiscard]] inline auto& BoundingSphere() const noexcept
			{
				return Mesh::BoundingSphere();
			}
	};
} //ion::graphics::scene::shapes

#endif