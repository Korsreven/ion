/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders/variables
File:	IonShaderVariable.h
-------------------------------------------
*/

#ifndef ION_SHADER_VARIABLE_H
#define ION_SHADER_VARIABLE_H

#include <optional>
#include <string>
#include <variant>

#include "managed/IonManagedObject.h"

namespace ion::graphics::shaders
{
	class ShaderProgram; //Forward declaration
} //ion::graphics::shaders

namespace ion::graphics::shaders::variables
{
	namespace shader_variable::detail
	{
	} //shader_variable::detail
	

	///@brief A base class representing a shader variable with a specific location
	class ShaderVariable : public managed::ManagedObject<ShaderProgram>
	{
		private:

			std::optional<int> location_;

		public:

			///@brief Constructs a new shader variable with the given name
			///@details Name should be equal to the identifier name in the shader source
			explicit ShaderVariable(std::string name) noexcept :
				managed::ManagedObject<ShaderProgram>{std::move(name)}
			{
				//Empty
			}

			///@brief Default virtual destructor
			virtual ~ShaderVariable() = default;


			/**
				@name Modifiers
				@{
			*/

			///@brief Sets where the shader variable is located to the given location
			inline void Location(std::optional<int> location) noexcept
			{
				location_ = location;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the location of the shader variable
			///@details Returns nullopt if the variable could not be located in the shader source
			[[nodiscard]] inline auto Location() const noexcept
			{
				return location_;
			}

			///@}
	};
} //ion::graphics::shaders::variables

#endif