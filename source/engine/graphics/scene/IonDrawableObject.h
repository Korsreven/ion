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

#include <span>
#include <string>
#include <vector>

#include "IonMovableObject.h"
#include "adaptors/ranges/IonIterable.h"
#include "graphics/render/IonPass.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene
{
	using namespace types::type_literals;

	namespace drawable_object
	{
		using RenderPrimitives = std::vector<render::RenderPrimitive*>;
		using ShaderPrograms = std::vector<shaders::ShaderProgram*>;
		using Lights = std::vector<Light*>;


		namespace detail
		{
		} //detail
	} //drawable_object


	//A class representing a drawable object that can be prepared and drawn with one or more passes
	class DrawableObject : public MovableObject
	{
		private:

			real opacity_ = 1.0_r;
			render::pass::Passes passes_;

		protected:

			mutable drawable_object::RenderPrimitives render_primitives_;
			mutable drawable_object::ShaderPrograms shader_programs_;


			void AddPrimitive(render::RenderPrimitive &primitive);
			void RemovePrimitive(render::RenderPrimitive &primitive) noexcept;

			void UpdatePassesOnAllPrimitives(const render::pass::Passes &passes) noexcept;
			void UpdateOpacityOnAllPrimitives(real opacity) noexcept;

		public:

			//Construct a drawable object with the given name and visibility
			explicit DrawableObject(std::optional<std::string> name = {}, bool visible = true);


			/*
				Ranges
			*/

			//Returns a mutable range of all passes of this drawable
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Passes() noexcept
			{
				return adaptors::ranges::Iterable<render::pass::Passes&>{passes_};
			}

			//Returns an immutable range of all passes of this drawable
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Passes() const noexcept
			{
				return adaptors::ranges::Iterable<const render::pass::Passes&>{passes_};
			}


			/*
				Modifiers
			*/

			//Sets the opacity of this drawable object to the given opacity
			void Opacity(real opacity) noexcept;


			/*
				Observers
			*/

			//Returns the opacity of this drawable object
			[[nodiscard]] inline auto Opacity() const noexcept
			{
				return opacity_;
			}


			//Returns all render primitives in this drawable object
			[[nodiscard]] virtual std::span<render::RenderPrimitive*> RenderPrimitives(bool derive = true) const override;

			//Returns all (distinct) shader programs used to render this drawable object
			[[nodiscard]] virtual std::span<shaders::ShaderProgram*> RenderPrograms(bool derive = true) const override;


			/*
				Preparing
			*/

			//Prepare this drawable object such that it is ready to be drawn
			//This function is typically called each frame
			virtual void Prepare() override;


			/*
				Elapse time
			*/

			//Elapse the total time for this drawable object by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			virtual void Elapse(duration time) noexcept;


			/*
				Passes
				Adding
			*/

			//Add the given pass for this drawable object
			void AddPass(render::Pass pass);

			//Add the given passes for this drawable object
			void AddPasses(render::pass::Passes passes);


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