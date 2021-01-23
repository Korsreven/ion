/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShaderLayout.h
-------------------------------------------
*/

#ifndef ION_SHADER_LAYOUT_H
#define ION_SHADER_LAYOUT_H

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "adaptors/IonFlatMap.h"
#include "managed/IonManagedObject.h"

namespace ion::graphics::shaders
{
	class ShaderProgramManager; //Forward declaration

	namespace shader_layout
	{
		class VariableDeclaration final
		{
			private:

				std::optional<std::string> name_;
				std::optional<int> location_;

			public:

				VariableDeclaration(std::string name);
				VariableDeclaration(int location) noexcept;
				VariableDeclaration(std::string name, int location);


				inline auto& Name() const noexcept
				{
					return name_;
				}

				inline auto Location() const noexcept
				{
					return location_;
				}
		};

		using VariableBinding = std::pair<std::string, VariableDeclaration>;
		using VariableBindings = std::vector<VariableBinding>;

		inline const auto DefaultAttributeBindings =
			VariableBindings
			{
				{"vertex.position", 	{"vertex.position", 	0}},
				{"vertex.normal", 		{"vertex.normal", 		1}},
				{"vertex.color", 		{"vertex.color", 		2}},
				{"vertex.tex_coord", 	{"vertex.tex_coord", 	3}},
				{"vertex.point_size", 	{"vertex.point_size", 	4}}
			};

		inline const auto DefaultUniformBindings =
			VariableBindings
			{
				{"camera.position", 				{"camera.position", 				0}},

				{"light.position", 					{"light.position", 					1}},
				{"light.direction", 				{"light.direction", 				2}},
				{"light.cutoff", 					{"light.cutoff", 					3}},
				{"light.ambient", 					{"light.ambient", 					4}},
				{"light.diffuse", 					{"light.diffuse", 					5}},
				{"light.specular", 					{"light.specular", 					6}},

				{"material.ambient", 				{"material.ambient", 				7}},
				{"material.diffuse", 				{"material.diffuse", 				8}},
				{"material.specular", 				{"material.specular", 				9}},
				{"material.shininess", 				{"material.shininess", 				10}},
				{"material.diffuse_map", 			{"material.diffuse_map", 			11}},
				{"material.specular_map", 			{"material.specular_map", 			12}},
				{"material.normal_map", 			{"material.normal_map", 			13}},

				{"matrix.model_view", 				{"matrix.model_view", 				14}},
				{"matrix.projection", 				{"matrix.projection", 				15}},
				{"matrix.model_view_projection",	{"matrix.model_view_projection",	16}},

				{"scene.gamma",						{"scene.gamma",						17}}
			};

		namespace detail
		{
			using variable_binding_map = adaptors::FlatMap<std::string, shader_layout::VariableDeclaration>;

			bool is_declaration_unique(VariableDeclaration declaration, const variable_binding_map &variable_bindings);
			variable_binding_map make_bindings(VariableBindings variable_bindings);
		} //detail
	} //shader_layout


	class ShaderLayout final : public managed::ManagedObject<ShaderProgramManager>
	{
		private:

			shader_layout::detail::variable_binding_map attribute_bindings_;
			shader_layout::detail::variable_binding_map uniform_bindings_;

		public:

			//Construct a new empty shader layout with the given name
			ShaderLayout(std::string name);

			//Construct a new shader layout with the given name, attribute and uniform bindings
			ShaderLayout(std::string name,
				shader_layout::VariableBindings attribute_bindings, shader_layout::VariableBindings uniform_bindings);


			/*
				Ranges
			*/

			//Returns a mutable range of all attribute bindings in this shader layout
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttributeBindings() noexcept
			{
				return attribute_bindings_.Elements();
			}

			//Returns an immutable range of all attribute bindings in this shader layout
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto AttributeBindings() const noexcept
			{
				return attribute_bindings_.Elements();
			}


			//Returns a mutable range of all uniform bindings in this shader layout
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto UniformBindings() noexcept
			{
				return uniform_bindings_.Elements();
			}

			//Returns an immutable range of all uniform bindings in this shader layout
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto UniformBindings() const noexcept
			{
				return uniform_bindings_.Elements();
			}


			/*
				Modifiers
			*/

			//Bind the given name to the given attribute declaration
			//Returns true if the attribute binding suceeded
			bool BindAttribute(std::string name, shader_layout::VariableDeclaration declaration);

			//Bind the given name to the given uniform declaration
			//Returns true if the uniform binding suceeded
			bool BindUniform(std::string name, shader_layout::VariableDeclaration declaration);


			/*
				Observers
			*/

			//Returns the attribute declaration binded to the given name
			//Returns nullptr if no attribute binding could be found
			[[nodiscard]] const shader_layout::VariableDeclaration* BindedAttribute(std::string_view name) const noexcept;

			//Returns the uniform declaration binded to the given name
			//Returns nullptr if no uniform binding could be found
			[[nodiscard]] const shader_layout::VariableDeclaration* BindedUniform(std::string_view name) const noexcept;
	};


	namespace shader_layout
	{
		/*
			Predefined shader layouts
		*/

		inline const auto DefaultShaderLayout =
			ShaderLayout{"", DefaultAttributeBindings, DefaultUniformBindings};
	} //shader_layout
} //ion::graphics::shaders

#endif