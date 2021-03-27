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
	namespace shape::detail
	{
	} //shape::detail


	class Shape : public Mesh
	{
		protected:

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

		public:

			/*
				Modifiers
			*/

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