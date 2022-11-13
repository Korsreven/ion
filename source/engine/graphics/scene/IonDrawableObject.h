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


	///@brief A class representing a drawable object that can be prepared and drawn with one or more render passes
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

			///@brief Constructs a drawable object with the given name and visibility
			explicit DrawableObject(std::optional<std::string> name = {}, bool visible = true) noexcept;


			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all render passes of this drawable
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto RenderPasses() noexcept
			{
				return adaptors::ranges::Iterable<render::render_pass::Passes&>{render_passes_};
			}

			///@brief Returns an immutable range of all render passes of this drawable
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto RenderPasses() const noexcept
			{
				return adaptors::ranges::Iterable<const render::render_pass::Passes&>{render_passes_};
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the opacity of this drawable object to the given opacity
			void Opacity(real opacity) noexcept;

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the opacity of this drawable object
			[[nodiscard]] inline auto Opacity() const noexcept
			{
				return opacity_;
			}


			///@brief Returns all render primitives in this drawable object
			[[nodiscard]] virtual movable_object::RenderPrimitiveRange AllRenderPrimitives() noexcept override;

			///@brief Returns all (distinct) shader programs used to render this drawable object
			[[nodiscard]] virtual movable_object::ShaderProgramRange AllShaderPrograms() noexcept override;

			///@}

			/**
				@name Notifying
				@{
			*/

			///@brief Called when render passes has changed on the given primitive
			void NotifyRenderPassesChanged(render::RenderPrimitive &primitive) noexcept;

			///@}

			/**
				@name Preparing
				@{
			*/

			///@brief Prepares this drawable object such that it is ready to be drawn
			///@details This function is typically called each frame
			virtual void Prepare() override;

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses the total time for this drawable object by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			virtual void Elapse(duration time) noexcept;

			///@}

			/**
				@name Render passes
				Adding
				@{
			*/

			///@brief Adds the given render pass for this drawable object
			void AddRenderPass(render::RenderPass pass);

			///@brief Adds the given render passes for this drawable object
			void AddRenderPasses(render::render_pass::Passes passes);

			///@}

			/**
				@name Render passes
				Retrieving
				@{
			*/

			///@brief Returns a mutable reference to the render pass at the given offset
			[[nodiscard]] render::RenderPass& GetRenderPass(int off) noexcept;

			///@brief Returns an immutable reference to the render pass at the given offset
			[[nodiscard]] const render::RenderPass& GetRenderPass(int off) const noexcept;

			///@}

			/**
				@name Render passes
				Removing
				@{
			*/

			///@brief Clears all render passes for this drawable object
			void ClearRenderPasses() noexcept;

			///@brief Removes a render pass at the given offset from this drawable object
			bool RemoveRenderPass(int off) noexcept;

			///@}
	};
} //ion::graphics::scene

#endif