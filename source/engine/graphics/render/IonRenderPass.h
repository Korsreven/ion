/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonRenderPass.h
-------------------------------------------
*/

#ifndef ION_RENDER_PASS_H
#define ION_RENDER_PASS_H

#include <optional>
#include <utility>
#include <vector>

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
	class RenderPass; //Forward declaration

	namespace render_pass
	{
		using Passes = std::vector<RenderPass>;

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
			int render_pass_blend_factor_to_gl_blend_factor(BlendFactor factor) noexcept;
			int render_pass_blend_equation_mode_to_gl_blend_equation_mode(BlendEquationMode mode) noexcept;


			/**
				@name Graphics API
				@{
			*/

			void blend(BlendFactor source_factor, BlendFactor destination_factor, BlendEquationMode equation_mode) noexcept;
			void blend_separate(BlendFactor source_factor, BlendFactor destination_factor,
								BlendFactor source_factor_alpha, BlendFactor destination_factor_alpha,
								BlendEquationMode equation_mode, BlendEquationMode equation_mode_alpha) noexcept;

			///@}
		} //detail
	} //render_pass


	///@brief A class representing a render pass which contains a shader program and blending factors
	///@details If a render pass has not specified a shader program, the render pass will be rendered using the fixed-function pipeline
	class RenderPass final
	{
		private:

			int iterations_ = 1;

			render_pass::BlendFactor blend_source_factor_ = render_pass::BlendFactor::SourceAlpha;
			render_pass::BlendFactor blend_destination_factor_ = render_pass::BlendFactor::OneMinus_SourceAlpha;
			std::optional<render_pass::BlendFactor> blend_source_factor_alpha_;
			std::optional<render_pass::BlendFactor> blend_destination_factor_alpha_;

			render_pass::BlendEquationMode blend_equation_mode_ = render_pass::BlendEquationMode::Add;
			std::optional<render_pass::BlendEquationMode> blend_equation_mode_alpha_;

			NonOwningPtr<shaders::ShaderProgram> shader_program_;

		public:

			///@brief Constructs a new render pass with the given values
			explicit RenderPass(NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr, int iterations = 1) noexcept;


			///@brief Constructs a new render pass with non-separated blending with the given values
			RenderPass(render_pass::BlendFactor blend_source_factor, render_pass::BlendFactor blend_destination_factor,
				NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr, int iterations = 1) noexcept;

			///@brief Constructs a new render pass with non-separated blending with the given values
			RenderPass(render_pass::BlendFactor blend_source_factor, render_pass::BlendFactor blend_destination_factor,
				render_pass::BlendEquationMode blend_equation_mode,
				NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr, int iterations = 1) noexcept;


			///@brief Constructs a new render pass with separated blending with the given values
			RenderPass(render_pass::BlendFactor blend_source_factor, render_pass::BlendFactor blend_destination_factor,
				render_pass::BlendFactor blend_source_factor_alpha_, render_pass::BlendFactor blend_destination_factor_alpha,
				NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr, int iterations = 1) noexcept;

			///@brief Constructs a new render pass with separated blending with the given values
			RenderPass(render_pass::BlendFactor blend_source_factor, render_pass::BlendFactor blend_destination_factor,
				render_pass::BlendFactor blend_source_factor_alpha_, render_pass::BlendFactor blend_destination_factor_alpha,
				render_pass::BlendEquationMode blend_equation_mode, render_pass::BlendEquationMode blend_equation_mode_alpha,
				NonOwningPtr<shaders::ShaderProgram> shader_program = nullptr, int iterations = 1) noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Checks if two render passes are equal (all members are equal)
			[[nodiscard]] inline auto operator==(const RenderPass &rhs) const noexcept
			{
				return shader_program_ == rhs.shader_program_ && //Check first
					   iterations_ == rhs.iterations_ &&
					   blend_source_factor_ == rhs.blend_source_factor_ &&
					   blend_destination_factor_ == rhs.blend_destination_factor_ &&
					   blend_source_factor_alpha_ == rhs.blend_source_factor_alpha_ &&
					   blend_destination_factor_alpha_ == rhs.blend_destination_factor_alpha_ &&
					   blend_equation_mode_ == rhs.blend_equation_mode_ &&
					   blend_equation_mode_alpha_ == rhs.blend_equation_mode_alpha_;
			}

			///@brief Checks if two render passes are different (one or more members are different)
			[[nodiscard]] inline auto operator!=(const RenderPass &rhs) const noexcept
			{
				return !(*this == rhs);
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the iteration count for this render pass
			///@details Pass 0 to disable this render pass
			inline void Iterations(int count) noexcept
			{
				iterations_ = count;
			}


			///@brief Sets the blending factor this render pass uses to the given factors
			inline void BlendingFactor(render_pass::BlendFactor source, render_pass::BlendFactor destination) noexcept
			{
				blend_source_factor_ = source;
				blend_destination_factor_ = destination;

				blend_source_factor_alpha_ = {};
				blend_destination_factor_alpha_ = {};
			}

			///@brief Sets the blending factor this render pass uses to the given factors
			inline void BlendingFactor(render_pass::BlendFactor source, render_pass::BlendFactor destination,
									   render_pass::BlendFactor source_alpha, render_pass::BlendFactor destination_alpha) noexcept
			{
				blend_source_factor_ = source;
				blend_destination_factor_ = destination;

				blend_source_factor_alpha_ = source_alpha;
				blend_destination_factor_alpha_ = destination_alpha;
			}

			///@brief Sets the blending equation mode this render pass uses to the given mode
			inline void BlendingEquationMode(render_pass::BlendEquationMode mode) noexcept
			{
				blend_equation_mode_ = mode;
				blend_equation_mode_alpha_ = {};
			}

			///@brief Sets the blending equation mode this render pass uses to the given modes
			inline void BlendingEquationMode(render_pass::BlendEquationMode mode, render_pass::BlendEquationMode mode_alpha) noexcept
			{
				blend_equation_mode_ = mode;
				blend_equation_mode_alpha_ = mode_alpha;
			}


			///@brief Sets the shader program this render pass uses to the given shader
			///@details Pass nullptr to use the fixed-functionality pipeline
			inline void RenderProgram(NonOwningPtr<shaders::ShaderProgram> shader_program) noexcept
			{
				shader_program_ = shader_program;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the iteration count for this render pass
			///@details Returns 0 if this render pass is disabled
			[[nodiscard]] inline auto Iterations() const noexcept
			{
				return iterations_;
			}


			///@brief Returns the blending factors this render pass uses
			[[nodiscard]] inline auto BlendingFactor() const noexcept
			{
				return std::pair{blend_source_factor_, blend_destination_factor_};
			}

			///@brief Returns the blending factors this render pass uses
			///@details Returns nullopt if no separate alpha blending factors are in use
			[[nodiscard]] inline auto BlendingFactorAlpha() const noexcept ->
				std::optional<std::pair<render_pass::BlendFactor, render_pass::BlendFactor>>
			{
				if (blend_source_factor_alpha_ && blend_destination_factor_alpha_)
					return std::pair{*blend_source_factor_alpha_, *blend_destination_factor_alpha_};
				else
					return {};
			}

			///@brief Returns the blending equation mode this render pass uses
			[[nodiscard]] inline auto BlendingEquationMode() const noexcept
			{
				return blend_equation_mode_;
			}

			///@brief Returns the blending equation mode this render pass uses
			///@details Returns nullopt if no separate alpha blending equation mode is in use
			[[nodiscard]] inline auto BlendingEquationModeAlpha() const noexcept
			{
				return blend_equation_mode_alpha_;
			}


			///@brief Returns the shader program this render pass uses to render
			///@details Returns nullptr if this render pass uses the fixed-functionality pipeline
			[[nodiscard]] inline auto RenderProgram() const noexcept
			{
				return shader_program_;
			}

			///@}

			/**
				@name Blending
				@{
			*/

			///@brief Set up and enable blending before this render pass
			void Blend() const noexcept;

			///@}
	};
} //ion::graphics::render

#endif