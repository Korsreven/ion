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

#ifndef ION_GEOMETRIC_OBJECT_H
#define ION_GEOMETRIC_OBJECT_H

#include "IonMovableObject.h"

namespace ion::graphics::scene
{
	namespace drawable_object::detail
	{
	} //drawable_object::detail


	//A drawable object that can be prepared and drawn with one or more passes
	class DrawableObject : public MovableObject
	{
		private:



		public:

			//Default constructor
			DrawableObject() = default;

			//Construct a drawable object with the given visibility
			explicit DrawableObject(bool visible);

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
				Rendering
			*/

			//Render this drawable object based on its defined passes
			//This is called once from a scene graph render queue, with the time in seconds since last frame
			//It will call elapse then prepare, and then call draw one time per pass
			void Render(duration time) noexcept override;


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
	};
} //ion::graphics::scene

#endif