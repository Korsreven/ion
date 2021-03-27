/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/shapes
File:	IonTriangle.h
-------------------------------------------
*/

#ifndef ION_TRIANGLE_H
#define ION_TRIANGLE_H

#include <tuple>

#include "IonShape.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector3.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene::shapes
{
	using namespace types::type_literals;
	using namespace utilities;

	namespace triangle::detail
	{
		mesh::Vertices triangle_vertices(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Color &color);
	} //triangle::detail


	class Triangle : public Shape
	{
		private:

			Vector3 a_;
			Vector3 b_;
			Vector3 c_;

		public:
		
			//Construct a new triangle with the given positions a b c, color and visibility
			Triangle(const Vector3 &a, const Vector3 &b, const Vector3 &c, const Color &color, bool visible = true);


			/*
				Modifiers
			*/

			//Sets the position a of this triangle to the given position
			inline void A(const Vector3 &a) noexcept
			{
				if (a_ != a)
				{
					a_ = a;
					Mesh::VertexData(triangle::detail::triangle_vertices(a_, b_, c_, color_));
				}
			}

			//Sets the position b of this triangle to the given position
			inline void B(const Vector3 &b) noexcept
			{
				if (b_ != b)
				{
					b_ = b;
					Mesh::VertexData(triangle::detail::triangle_vertices(a_, b_, c_, color_));
				}
			}

			//Sets the position c of this triangle to the given position
			inline void C(const Vector3 &c) noexcept
			{
				if (c_ != c)
				{
					c_ = c;
					Mesh::VertexData(triangle::detail::triangle_vertices(a_, b_, c_, color_));
				}
			}

			//Sets the positions a b c of this triangle to the given positions
			inline void ABC(const Vector3 &a, const Vector3 &b, const Vector3 &c) noexcept
			{
				if (a_ != a || b_ != b || c_ != c)
				{
					a_ = a;
					b_ = b;
					c_ = c;
					Mesh::VertexData(triangle::detail::triangle_vertices(a_, b_, c_, color_));
				}
			}


			/*
				Observers
			*/

			//Returns the position a of this triangle
			[[nodiscard]] inline auto& A() const noexcept
			{
				return a_;
			}

			//Returns the position b of this triangle
			[[nodiscard]] inline auto& B() const noexcept
			{
				return b_;
			}

			//Returns the position c of this triangle
			[[nodiscard]] inline auto& C() const noexcept
			{
				return c_;
			}

			//Returns the positions a b c of this triangle
			[[nodiscard]] inline auto ABC() const noexcept
			{
				return std::tuple{a_, b_, c_};
			}
	};
} //ion::graphics::scene::shapes

#endif