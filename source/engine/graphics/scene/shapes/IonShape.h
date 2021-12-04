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
	} //shape::detail


	class Shape : public Mesh
	{
		protected:

			Color color_;
			bool update_vertices_ = false;


			//Construct a new shape with the given vertices, color and visibility
			//Can only be instantiated by derived
			Shape(const mesh::Vertices &vertices, const Color &color, bool visible = true);

			//Construct a new shape with the given draw mode, vertices, color and visibility
			//Can only be instantiated by derived
			Shape(vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices,
				const Color &color, bool visible = true);


			//Construct a new texturized shape with the given vertices, material, color and visibility
			//Can only be instantiated by derived
			Shape(const mesh::Vertices &vertices, NonOwningPtr<materials::Material> material,
				const Color &color, bool visible = true);

			//Construct a new texturized shape with the given draw mode, vertices, material, color and visibility
			//Can only be instantiated by derived
			Shape(vertex::vertex_batch::VertexDrawMode draw_mode, const mesh::Vertices &vertices,
				NonOwningPtr<materials::Material> material, const Color &color, bool visible = true);


			//Must be overridden to return the vertices of the derived shape
			virtual mesh::Vertices GetVertices() const noexcept = 0;


			/*
				Events
			*/

			virtual void VertexColorChanged() noexcept override;
			virtual void VertexOpacityChanged() noexcept override;

		public:

			/*
				Modifiers
			*/

			//Sets the color of this shape to the given color
			inline void FillColor(const Color &color) noexcept
			{
				if (color_ != color)
				{
					color_ = color;
					update_vertices_ = true;
				}
			}

			//Sets the opacity of this shape to the given percent
			inline void FillOpacity(real percent) noexcept
			{
				if (color_.A() != percent)
				{
					color_.A(percent);
					update_vertices_ = true;
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

			//Returns the color of this shape
			[[nodiscard]] inline auto& FillColor() const noexcept
			{
				return color_;
			}

			//Returns the opacity of this shape
			[[nodiscard]] inline auto FillOpacity() const noexcept
			{
				return color_.A();
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


			/*
				Preparing
			*/

			//Refresh this shape by generating new vertices
			inline void Refresh() noexcept
			{
				update_vertices_ = true;
			}


			//Prepare this shape such that it is ready to be drawn
			//This is called once regardless of passes
			virtual mesh::MeshBoundingVolumeStatus Prepare() noexcept override;
	};
} //ion::graphics::scene::shapes

#endif