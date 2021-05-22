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

#include <optional>
#include <utility>

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
		enum class BlendFactor
		{
			Zero,
			One,

			SourceColor,
			OneMinus_SourceColor,
			DestinationColor,
			OneMinus_DestinationColor,

			SourceAlpha,
			OneMinus_SourceAlpha,
			DestinationAlpha,
			OneMinus_DestinationAlpha,

			ConstantColor,
			OneMinus_ConstantColor,
			ConstantAlpha,
			OneMinus_ConstantAlpha,
			
			SourceOneColor,
			OneMinus_SourceOneColor,
			SourceOneAlpha,
			OneMinus_SourceOneAlpha,

			SourceAlphaSaturate
		};

		enum class BlendEquationMode
		{
			Add,
			Subtract,
			ReverseSubtract,
			Min,
			Max
		};

		namespace detail
		{
			int pass_blend_factor_to_gl_blend_factor(BlendFactor factor) noexcept;
			int pass_blend_equation_mode_to_gl_blend_equation_mode(BlendEquationMode mode) noexcept;


			/*
				Graphics API
			*/

			void blend(BlendFactor source_factor, BlendFactor destination_factor, BlendEquationMode equation_mode) noexcept;
			void blend_separate(BlendFactor source_factor, BlendFactor destination_factor,
								BlendFactor source_factor_alpha, BlendFactor destination_factor_alpha,
								BlendEquationMode equation_mode, BlendEquationMode equation_mode_alpha) noexcept;
		} //detail
	} //pass


	class Pass final
	{
		private:

			int iterations_ = 1;

			pass::BlendFactor blend_source_factor_ = pass::BlendFactor::SourceAlpha;
			pass::BlendFactor blend_destination_factor_ = pass::BlendFactor::OneMinus_SourceAlpha;
			std::optional<pass::BlendFactor> blend_source_factor_alpha_;
			std::optional<pass::BlendFactor> blend_destination_factor_alpha_;

			pass::BlendEquationMode blend_equation_mode_ = pass::BlendEquationMode::Add;
			std::optional<pass::BlendEquationMode> blend_equation_mode_alpha_;

			NonOwningPtr<shaders::ShaderProgram> shader_program_;

		public:

			//Construct a new pass with the given values
			explicit Pass(NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr, int iterations = 1) noexcept;


			//Construct a new pass with non-separated blending with the given values
			Pass(pass::BlendFactor blend_source_factor, pass::BlendFactor blend_destination_factor,
				NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr, int iterations = 1) noexcept;

			//Construct a new pass with non-separated blending with the given values
			Pass(pass::BlendFactor blend_source_factor, pass::BlendFactor blend_destination_factor,
				pass::BlendEquationMode blend_equation_mode,
				NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr, int iterations = 1) noexcept;


			//Construct a new pass with separated blending with the given values
			Pass(pass::BlendFactor blend_source_factor, pass::BlendFactor blend_destination_factor,
				pass::BlendFactor blend_source_factor_alpha_, pass::BlendFactor blend_destination_factor_alpha,
				NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr, int iterations = 1) noexcept;

			//Construct a new pass with separated blending with the given values
			Pass(pass::BlendFactor blend_source_factor, pass::BlendFactor blend_destination_factor,
				pass::BlendFactor blend_source_factor_alpha_, pass::BlendFactor blend_destination_factor_alpha,
				pass::BlendEquationMode blend_equation_mode, pass::BlendEquationMode blend_equation_mode_alpha,
				NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr, int iterations = 1) noexcept;


			/*
				Modifiers
			*/

			//Sets the iteration count for this pass
			//Pass 0 to disable this pass
			inline void Iterations(int count) noexcept
			{
				iterations_ = count;
			}


			//Sets the blending factor this pass uses to the given factors
			inline void BlendingFactor(pass::BlendFactor source, pass::BlendFactor destination) noexcept
			{
				blend_source_factor_ = source;
				blend_destination_factor_ = destination;

				blend_source_factor_alpha_ = {};
				blend_destination_factor_alpha_ = {};
			}

			//Sets the blending factor this pass uses to the given factors
			inline void BlendingFactor(pass::BlendFactor source, pass::BlendFactor destination,
									   pass::BlendFactor source_alpha, pass::BlendFactor destination_alpha) noexcept
			{
				blend_source_factor_ = source;
				blend_destination_factor_ = destination;

				blend_source_factor_alpha_ = source_alpha;
				blend_destination_factor_alpha_ = destination_alpha;
			}

			//Sets the blending equation mode this pass uses to the given mode
			inline void BlendingEquationMode(pass::BlendEquationMode mode) noexcept
			{
				blend_equation_mode_ = mode;
				blend_equation_mode_alpha_ = {};
			}

			//Sets the blending equation mode this pass uses to the given modes
			inline void BlendingEquationMode(pass::BlendEquationMode mode, pass::BlendEquationMode mode_alpha) noexcept
			{
				blend_equation_mode_ = mode;
				blend_equation_mode_alpha_ = mode_alpha;
			}


			//Sets the shader program this pass uses to the given shader
			//Pass nullptr to use the fixed-functionality pipeline
			inline void RenderProgram(NonOwningPtr<shaders::ShaderProgram> shader_program) noexcept
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


			//Returns the blending factors this pass uses
			[[nodiscard]] inline auto BlendingFactor() const noexcept
			{
				return std::pair{blend_source_factor_, blend_destination_factor_};
			}

			//Returns the blending factors this pass uses
			//Returns nullopt if no separate alpha blending factors are in use
			[[nodiscard]] inline auto BlendingFactorAlpha() const noexcept ->
				std::optional<std::pair<pass::BlendFactor, pass::BlendFactor>>
			{
				if (blend_source_factor_alpha_ && blend_destination_factor_alpha_)
					return std::pair{*blend_source_factor_alpha_, *blend_destination_factor_alpha_};
				else
					return {};
			}

			//Returns the blending equation mode this pass uses
			[[nodiscard]] inline auto BlendingEquationMode() const noexcept
			{
				return blend_equation_mode_;
			}

			//Returns the blending equation mode this pass uses
			//Returns nullopt if no separate alpha blending equation mode is in use
			[[nodiscard]] inline auto BlendingEquationModeAlpha() const noexcept
			{
				return blend_equation_mode_alpha_;
			}


			//Returns the shader program this pass uses to render
			//Returns nullptr if this pass uses the fixed-functionality pipeline
			[[nodiscard]] inline auto RenderProgram() const noexcept
			{
				return shader_program_;
			}


			/*
				Blending
			*/

			//Set up and enable blending before this render pass
			void Blend() noexcept;
	};
} //ion::graphics::render

#endif