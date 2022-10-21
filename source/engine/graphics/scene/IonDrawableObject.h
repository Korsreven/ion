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
#include "graphics/render/IonRenderPass.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

namespace ion::graphics::scene
{
	using namespace types::type_literals;

	namespace drawable_object
	{
		using RenderPrimitives = std::vector<render::RenderPrimitive*>;
		using ShaderPrograms = std::vector<NonOwningPtr<shaders::ShaderProgram>>;


		namespace detail
		{
		} //detail
	} //drawable_object


	//A class representing a drawable object that can be prepared and drawn with one or more render passes
	class DrawableObject : public MovableObject
	{
		private:

			real opacity_ = 1.0_r;
			render::render_pass::Passes render_passes_;

			bool update_render_passes_ = false;

		protected:

			drawable_object::RenderPrimitives render_primitives_;
			drawable_object::ShaderPrograms shader_programs_;


			void AddPrimitive(render::RenderPrimitive &primitive);
			void RemovePrimitive(render::RenderPrimitive &primitive) noexcept;

			void UpdateRenderPassesOnAllPrimitives(const render::render_pass::Passes &passes) noexcept;
			void UpdateOpacityOnAllPrimitives(real opacity) noexcept;

		public:

			//Construct a drawable object with the given name and visibility
			explicit DrawableObject(std::optional<std::string> name = {}, bool visible = true) noexcept;


			/*
				Ranges
			*/

			//Returns a mutable range of all render passes of this drawable
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto RenderPasses() noexcept
			{
				return adaptors::ranges::Iterable<render::render_pass::Passes&>{render_passes_};
			}

			//Returns an immutable range of all render passes of this drawable
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto RenderPasses() const noexcept
			{
				return adaptors::ranges::Iterable<const render::render_pass::Passes&>{render_passes_};
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
			[[nodiscard]] virtual movable_object::RenderPrimitiveRange AllRenderPrimitives() noexcept override;

			//Returns all (distinct) shader programs used to render this drawable object
			[[nodiscard]] virtual movable_object::ShaderProgramRange AllShaderPrograms() noexcept override;


			/*
				Notifying
			*/

			//Called when render passes has changed on the given primitive
			void NotifyRenderPassesChanged(render::RenderPrimitive &primitive) noexcept;


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
				Render passes
				Adding
			*/

			//Add the given render pass for this drawable object
			void AddRenderPass(render::RenderPass pass);

			//Add the given render passes for this drawable object
			void AddRenderPasses(render::render_pass::Passes passes);


			/*
				Render passes
				Retrieving
			*/

			//Returns a mutable reference to the render pass at the given offset
			[[nodiscard]] render::RenderPass& GetRenderPass(int off) noexcept;

			//Returns an immutable reference to the render pass at the given offset
			[[nodiscard]] const render::RenderPass& GetRenderPass(int off) const noexcept;


			/*
				Render passes
				Removing
			*/

			//Clear all render passes for this drawable object
			void ClearRenderPasses() noexcept;

			//Remove a render pass at the given offset from this drawable object
			bool RemoveRenderPass(int off) noexcept;
	};
} //ion::graphics::scene

#endif