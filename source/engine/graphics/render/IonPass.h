/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonPass.h
-------------------------------------------
*/

#ifndef ION_PASS_H
#define ION_PASS_H

#include "memory/IonNonOwningPtr.h"

namespace ion::graphics
{
	namespace shaders
	{
		class ShaderProgram;
	}
}

namespace ion::graphics::render
{
	namespace pass
	{
		enum class BlendingMode
		{
			Default,
			Additive,
			Subtractive,
			Multiplicative
		};

		namespace detail
		{
		} //detail
	} //pass


	class Pass final
	{
		private:

			int iterations_ = 1;
			pass::BlendingMode blending_ = pass::BlendingMode::Default;
			NonOwningPtr<shaders::ShaderProgram> shader_program_;

		public:

			//Default constructor
			Pass() = default;

			//Construct a new pass with the given values
			Pass(int iterations, pass::BlendingMode blending, NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr) noexcept;


			/*
				Static pass conversions
			*/

			//Returns a new default pass from the given values
			[[nodiscard]] static Pass Default(int iterations, NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr) noexcept;

			//Returns a new additive pass from the given values
			[[nodiscard]] static Pass Additive(int iterations, NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr) noexcept;

			//Returns a new subtractive pass from the given values
			[[nodiscard]] static Pass Subtractive(int iterations, NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr) noexcept;

			//Returns a new multiplicative pass from the given values
			[[nodiscard]] static Pass Multiplicative(int iterations, NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr) noexcept;


			/*
				Modifiers
			*/

			//Sets the iteration count for this pass
			//Pass 0 to disable this pass
			inline void Iterations(int count) noexcept
			{
				iterations_ = count;
			}

			//Sets the blending mode this pass uses to the given mode
			inline void Blending(pass::BlendingMode blending) noexcept
			{
				blending_ = blending;
			}

			//Sets the shader program this pass uses to the given shader
			//Pass nullptr to use the fixed-functionality pipeline
			inline void ShaderProg(NonOwningPtr<shaders::ShaderProgram> shader_program) noexcept
			{
				shader_program_ = shader_program;
			}


			/*
				Observers
			*/

			//Returns the iteration count for this pass
			//Returns 0 if this pass is disabled
			[[nodiscard]] inline auto Iterations() const noexcept
			{
				return iterations_;
			}

			//Returns the blending mode this pass uses
			[[nodiscard]] inline auto Blending() const noexcept
			{
				return blending_;
			}

			//Returns the shader program this pass uses
			//Returns nullptr if this pass uses the fixed-functionality pipeline
			[[nodiscard]] inline auto ShaderProg() const noexcept
			{
				return shader_program_;
			}
	};
} //ion::graphics::render

#endif