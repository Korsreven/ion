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
	

	class ShaderVariable : public managed::ManagedObject<ShaderProgram>
	{
		private:

			std::string name_;
			std::optional<int> location_;

		public:

			//Constructor
			explicit ShaderVariable(std::string name) :
				name_{std::move(name)}
			{
				//Empty
			}

			//Default virtual destructor
			virtual ~ShaderVariable() = default;


			/*
				Modifiers
			*/

			//Sets where the shader variable is located to the given location
			inline void Location(std::optional<int> location) noexcept
			{
				location_ = location;
			}


			/*
				Observers
			*/

			//Returns the name of the shader variable as written in the shader source
			[[nodiscard]] inline auto& Name() const noexcept
			{
				return name_;
			}

			//Returns the location of the shader variable
			//Returns nullopt if the variable could not be located in the shader source
			[[nodiscard]] inline auto Location() const noexcept
			{
				return location_;
			}
	};
} //ion::graphics::shaders::variables

#endif