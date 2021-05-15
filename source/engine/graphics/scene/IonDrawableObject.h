/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene
File:	IonDrawableObject.h
-------------------------------------------
*/

#ifndef ION_DRAWABLE_OBJECT_H
#define ION_DRAWABLE_OBJECT_H

#include <vector>

#include "IonMovableObject.h"
#include "adaptors/ranges/IonIterable.h"
#include "graphics/render/IonPass.h"

namespace ion::graphics::scene
{
	namespace drawable_object
	{
		using Passes = std::vector<render::Pass>;

		namespace detail
		{
		} //detail
	} //drawable_object


	//A drawable object that can be prepared and drawn with one or more passes
	class DrawableObject : public MovableObject
	{
		private:

			drawable_object::Passes passes_;

		public:

			//Construct a drawable object with the given visibility
			explicit DrawableObject(bool visible = true);

			//Construct a drawable object with the given name and visibility
			explicit DrawableObject(std::string name, bool visible = true);

			//Copy constructor
			DrawableObject(const DrawableObject &rhs) noexcept;


			/*
				Operators
			*/

			//Copy assignment
			inline auto& operator=(const MovableObject &rhs) noexcept
			{
				MovableObject::operator=(rhs);
				return *this;
			}


			/*
				Ranges
			*/

			//Returns a mutable range of all passes of this drawable
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Passes() noexcept
			{
				return adaptors::ranges::Iterable<drawable_object::Passes&>{passes_};
			}

			//Returns an immutable range of all passes of this drawable
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Passes() const noexcept
			{
				return adaptors::ranges::Iterable<const drawable_object::Passes&>{passes_};
			}


			/*
				Rendering
			*/

			//Render this drawable object based on its defined passes
			//This is called once from a scene graph render queue
			//It will call prepare, and then draw one time per pass
			void Render() noexcept override;


			/*
				Preparing / drawing
			*/

			//Prepare this movable object such that it is ready to be drawn
			//This is called once regardless of passes
			virtual void Prepare() noexcept = 0;

			//Draw this movable object with the given shader program (optional)
			//This can be called multiple times if more than one pass
			virtual void Draw(shaders::ShaderProgram *shader_program = nullptr) noexcept = 0;


			/*
				Elapse time
			*/

			//Elapse the total time for this movable object by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			virtual void Elapse(duration time) noexcept;


			/*
				Passes
				Adding
			*/

			//Add the given pass for this drawable object
			void AddPass(render::Pass pass);

			//Add the given passes for this drawable object
			void AddPasses(drawable_object::Passes passes);


			/*
				Passes
				Retrieving
			*/

			//Returns a mutable reference to the pass at the given offset
			[[nodiscard]] render::Pass& GetPass(int off) noexcept;

			//Returns an immutable reference to the pass at the given offset
			[[nodiscard]] const render::Pass& GetPass(int off) const noexcept;


			/*
				Passes
				Removing
			*/

			//Clear all passes for this drawable object
			void ClearPasses() noexcept;

			//Remove a pass at the given offset from this drawable object
			bool RemovePass(int off) noexcept;
	};
} //ion::graphics::scene

#endif