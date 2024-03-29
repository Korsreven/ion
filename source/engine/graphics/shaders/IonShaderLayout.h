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

#include <algorithm>
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
		enum class StructName
		{
			Matrix,
			Scene,
			Camera,
			Primitive,
			Material,
			Fog,
			Light,
			EmissiveLight
		};

		enum class AttributeName
		{
			Vertex_Position,
			Vertex_Normal,
			Vertex_Color,
			Vertex_TexCoord,
			Vertex_Rotation,
			Vertex_PointSize
		};

		enum class UniformName
		{
			Matrix_ModelView,
			Matrix_Projection,
			Matrix_ModelViewProjection,
			Matrix_Normal,

			Scene_Ambient,
			Scene_Gamma,
			Scene_HasFog,
			Scene_Lights,
			Scene_EmissiveLights,
			Scene_LightCount,
			Scene_EmissiveLightCount,

			Camera_Position,
			Camera_Rotation,

			Primitive_Texture,
			Primitive_HasTexture,
			Primitive_HasMaterial,

			Material_Ambient,
			Material_Diffuse,
			Material_Specular,
			Material_Emissive,
			Material_Shininess,
			Material_DiffuseMap,
			Material_NormalMap,
			Material_SpecularMap,
			Material_EmissiveMap,
			Material_HasDiffuseMap,
			Material_HasNormalMap,
			Material_HasSpecularMap,
			Material_HasEmissiveMap,
			Material_LightingEnabled,

			Fog_Mode,
			Fog_Density,
			Fog_Near,
			Fog_Far,
			Fog_Color,

			Light_Type,
			Light_Position,
			Light_Direction,
			Light_Radius,
			Light_Ambient,
			Light_Diffuse,
			Light_Specular,
			Light_Constant,
			Light_Linear,
			Light_Quadratic,
			Light_Cutoff,
			Light_OuterCutoff,

			EmissiveLight_Position,
			EmissiveLight_Radius,
			EmissiveLight_Color
		};


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

		using StructBinding = std::pair<StructName, std::string>;
		using AttributeBinding = std::pair<AttributeName, VariableDeclaration>;
		using UniformBinding = std::pair<UniformName, VariableDeclaration>;

		using StructBindings = std::vector<StructBinding>;
		using AttributeBindings = std::vector<AttributeBinding>;
		using UniformBindings = std::vector<UniformBinding>;

		inline const auto DefaultStructBindings =
			StructBindings
			{
				{StructName::Matrix,		"matrix"},
				{StructName::Scene,			"scene"},
				{StructName::Camera,		"camera"},
				{StructName::Primitive,		"primitive"},
				{StructName::Material,		"material"},
				{StructName::Fog,			"fog"},
				{StructName::Light,			"light"},
				{StructName::EmissiveLight,	"emissive_light"}
			};

		inline const auto DefaultAttributeBindings =
			AttributeBindings
			{
				{AttributeName::Vertex_Position,	{"vertex_position",		0}},
				{AttributeName::Vertex_Normal,		{"vertex_normal",		1}},
				{AttributeName::Vertex_Color,		{"vertex_color",		2}},
				{AttributeName::Vertex_TexCoord,	{"vertex_tex_coord",	3}},
				{AttributeName::Vertex_Rotation,	{"vertex_rotation",		4}},
				{AttributeName::Vertex_PointSize,	{"vertex_point_size",	5}}
			};

		inline const auto DefaultUniformBindings =
			UniformBindings
			{
				{UniformName::Matrix_ModelView,				{"matrix.model_view",				0}},
				{UniformName::Matrix_Projection,			{"matrix.projection",				1}},
				{UniformName::Matrix_ModelViewProjection,	{"matrix.model_view_projection",	2}},
				{UniformName::Matrix_Normal,				{"matrix.normal",					3}},

				{UniformName::Scene_Ambient,				{"scene.ambient",					4}},
				{UniformName::Scene_Gamma,					{"scene.gamma",						5}},
				{UniformName::Scene_HasFog,					{"scene.has_fog",					6}},
				{UniformName::Scene_Lights,					{"scene.lights",					7}},
				{UniformName::Scene_EmissiveLights,			{"scene.emissive_lights",			8}},
				{UniformName::Scene_LightCount,				{"scene.light_count",				9}},
				{UniformName::Scene_EmissiveLightCount,		{"scene.emissive_light_count",		10}},

				{UniformName::Camera_Position,				{"camera.position",					11}},
				{UniformName::Camera_Rotation,				{"camera.rotation",					12}},

				{UniformName::Primitive_Texture,			{"primitive.texture",				13}},
				{UniformName::Primitive_HasTexture,			{"primitive.has_texture",			14}},
				{UniformName::Primitive_HasMaterial,		{"primitive.has_material",			15}},

				{UniformName::Material_Ambient,				{"material.ambient",				16}},
				{UniformName::Material_Diffuse,				{"material.diffuse",				17}},
				{UniformName::Material_Specular,			{"material.specular",				18}},
				{UniformName::Material_Emissive,			{"material.emissive",				19}},
				{UniformName::Material_Shininess,			{"material.shininess",				20}},
				{UniformName::Material_DiffuseMap,			{"material.diffuse_map",			21}},
				{UniformName::Material_NormalMap,			{"material.normal_map",				22}},
				{UniformName::Material_SpecularMap,			{"material.specular_map",			23}},
				{UniformName::Material_EmissiveMap,			{"material.emissive_map",			24}},
				{UniformName::Material_HasDiffuseMap,		{"material.has_diffuse_map",		25}},
				{UniformName::Material_HasNormalMap,		{"material.has_normal_map",			26}},
				{UniformName::Material_HasSpecularMap,		{"material.has_specular_map",		27}},
				{UniformName::Material_HasEmissiveMap,		{"material.has_emissive_map",		28}},
				{UniformName::Material_LightingEnabled,		{"material.lighting_enabled",		29}},

				{UniformName::Fog_Mode,						{"fog.mode",						30}},
				{UniformName::Fog_Density,					{"fog.density",						31}},
				{UniformName::Fog_Near,						{"fog.near",						32}},
				{UniformName::Fog_Far,						{"fog.far",							33}},
				{UniformName::Fog_Color,					{"fog.color",						34}},

				{UniformName::Light_Type,					{"light[0].type",					35}},
				{UniformName::Light_Position,				{"light[0].position",				36}},
				{UniformName::Light_Direction,				{"light[0].direction",				37}},
				{UniformName::Light_Radius,					{"light[0].radius",					38}},
				{UniformName::Light_Ambient,				{"light[0].ambient",				39}},
				{UniformName::Light_Diffuse,				{"light[0].diffuse",				40}},
				{UniformName::Light_Specular,				{"light[0].specular",				41}},
				{UniformName::Light_Constant,				{"light[0].constant",				42}},
				{UniformName::Light_Linear,					{"light[0].linear",					43}},
				{UniformName::Light_Quadratic,				{"light[0].quadratic",				44}},
				{UniformName::Light_Cutoff,					{"light[0].cutoff",					45}},
				{UniformName::Light_OuterCutoff,			{"light[0].outer_cutoff",			46}},

				{UniformName::EmissiveLight_Position,		{"emissive_light[0].position",		47}},
				{UniformName::EmissiveLight_Radius,			{"emissive_light[0].radius",		48}},
				{UniformName::EmissiveLight_Color,			{"emissive_light[0].color",			49}}
			};

		namespace detail
		{
			constexpr auto struct_name_count = static_cast<int>(StructName::EmissiveLight) + 1;
			constexpr auto attribute_name_count = static_cast<int>(AttributeName::Vertex_PointSize) + 1;
			constexpr auto uniform_name_count = static_cast<int>(UniformName::EmissiveLight_Color) + 1;

			using struct_binding_map = adaptors::FlatMap<StructName, std::string>;
			using attribute_binding_map = adaptors::FlatMap<AttributeName, shader_layout::VariableDeclaration>;
			using uniform_binding_map = adaptors::FlatMap<UniformName, shader_layout::VariableDeclaration>;


			adaptors::FlatMap<StructName, std::string> make_struct_binding_map(StructBindings struct_bindings) noexcept;

			template <typename T>
			auto make_variable_binding_map(T variable_bindings) noexcept
			{
				//Sort on location (asc)
				std::stable_sort(std::begin(variable_bindings), std::end(variable_bindings),
					[](auto &x, auto &y) noexcept
					{
						return x.second.Location() < y.second.Location();
					});

				//Remove duplicate locations
				variable_bindings.erase(
					std::unique(std::begin(variable_bindings), std::end(variable_bindings),
						[](auto &x, auto &y) noexcept
						{
							return x.second.Location() && y.second.Location() &&
								   x.second.Location() == y.second.Location();
						}), std::end(variable_bindings));


				//Sort on name (asc)
				std::stable_sort(std::begin(variable_bindings), std::end(variable_bindings),
					[](auto &x, auto &y) noexcept
					{
						return x.second.Name() < y.second.Name();
					});

				//Remove duplicate names
				variable_bindings.erase(
					std::unique(std::begin(variable_bindings), std::end(variable_bindings),
						[](auto &x, auto &y) noexcept
						{
							return x.second.Name() && y.second.Name() &&
								   x.second.Name() == y.second.Name();
						}), std::end(variable_bindings));

				return adaptors::FlatMap<typename T::value_type::first_type, typename T::value_type::second_type>{std::move(variable_bindings)};
			}


			bool is_struct_unique(std::string_view name, const struct_binding_map &struct_bindings) noexcept;

			template <typename T>
			auto is_declaration_unique(const VariableDeclaration &declaration, const T &variable_bindings) noexcept
			{
				//Declaration has location
				if (declaration.Location())
				{
					//No duplicate locations allowed
					for (auto &binding : variable_bindings)
					{
						if (binding.second.Location() == declaration.Location())
							return false;
					}
				}

				//Declaration has name
				if (declaration.Name())
				{
					//No duplicate names allowed
					for (auto &binding : variable_bindings)
					{
						if (binding.second.Name() == declaration.Name())
							return false;
					}
				}

				return true;
			}
		} //detail
	} //shader_layout


	///@brief A class representing an attribute and uniform binding layout for a shader program to follow
	///@details By using the same pre-defined attribute/uniform names, the engine will automatically send those values to the associated variabels.
	///If no shader layout is specified for a shader program, a default layout will be used
	class ShaderLayout final : public managed::ManagedObject<ShaderProgramManager>
	{
		private:

			shader_layout::detail::struct_binding_map struct_bindings_;
			shader_layout::detail::attribute_binding_map attribute_bindings_;
			shader_layout::detail::uniform_binding_map uniform_bindings_;

		public:

			///@brief Constructs a new empty shader layout with the given name
			explicit ShaderLayout(std::string name) noexcept;

			///@brief Constructs a new shader layout with the given name, struct, attribute and uniform bindings
			ShaderLayout(std::string name, shader_layout::StructBindings struct_bindings,
				shader_layout::AttributeBindings attribute_bindings, shader_layout::UniformBindings uniform_bindings) noexcept;


			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all structs in this shader layout
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Structs() noexcept
			{
				return struct_bindings_.Elements();
			}

			///@brief Returns an immutable range of all structs in this shader layout
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Structs() const noexcept
			{
				return struct_bindings_.Elements();
			}


			///@brief Returns a mutable range of all attributes in this shader layout
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Attributes() noexcept
			{
				return attribute_bindings_.Elements();
			}

			///@brief Returns an immutable range of all attributes in this shader layout
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Attributes() const noexcept
			{
				return attribute_bindings_.Elements();
			}


			///@brief Returns a mutable range of all uniforms in this shader layout
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Uniforms() noexcept
			{
				return uniform_bindings_.Elements();
			}

			///@brief Returns an immutable range of all uniforms in this shader layout
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Uniforms() const noexcept
			{
				return uniform_bindings_.Elements();
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Binds the given name to the given struct name
			///@details Returns true if the struct binding suceeded
			bool BindStruct(shader_layout::StructName name, std::string str_name);

			///@brief Binds the given name to the given attribute declaration
			///@details Returns true if the attribute binding suceeded
			bool BindAttribute(shader_layout::AttributeName name, shader_layout::VariableDeclaration declaration);

			///@brief Binds the given name to the given uniform declaration
			///@details Returns true if the uniform binding suceeded
			bool BindUniform(shader_layout::UniformName name, shader_layout::VariableDeclaration declaration);

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the struct name bound to the given standardized struct name
			///@details Returns nullopt if no struct binding could be found
			[[nodiscard]] std::optional<std::string> BoundStruct(shader_layout::StructName name) const noexcept;

			///@brief Returns the attribute declaration bound to the given standardized attribute name
			///@details Returns nullopt if no attribute binding could be found
			[[nodiscard]] std::optional<shader_layout::VariableDeclaration> BoundAttribute(shader_layout::AttributeName name) const noexcept;

			///@brief Returns the uniform declaration bound to the given standardized uniform name
			///@details Returns nullopt if no uniform binding could be found
			[[nodiscard]] std::optional<shader_layout::VariableDeclaration> BoundUniform(shader_layout::UniformName name) const noexcept;


			///@brief Returns the standardized struct name that is bound to the given struct name
			///@details Returns nullopt if no struct binding could be found
			[[nodiscard]] std::optional<shader_layout::StructName> GetStructName(std::string_view name) const noexcept;

			///@brief Returns the standardized attribute name that is bound to the given attribute name
			///@details Returns nullopt if no attribute binding could be found
			[[nodiscard]] std::optional<shader_layout::AttributeName> GetAttributeName(std::string_view name) const noexcept;

			///@brief Returns the standardized uniform name that is bound to the given uniform name
			///@details Returns nullopt if no uniform binding could be found
			[[nodiscard]] std::optional<shader_layout::UniformName> GetUniformName(std::string_view name) const noexcept;


			///@brief Returns the standardized attribute name that is bound to the given attribute location
			///@details Returns nullopt if no attribute binding could be found
			[[nodiscard]] std::optional<shader_layout::AttributeName> GetAttributeName(int location) const noexcept;

			///@brief Returns the standardized uniform name that is bound to the given uniform location
			///@details Returns nullopt if no uniform binding could be found
			[[nodiscard]] std::optional<shader_layout::UniformName> GetUniformName(int location) const noexcept;

			///@}
	};


	namespace shader_layout
	{
		/**
			@name Predefined shader layouts
			@{
		*/

		inline const auto DefaultShaderLayout =
			ShaderLayout{"", DefaultStructBindings, DefaultAttributeBindings, DefaultUniformBindings};

		///@}
	} //shader_layout
} //ion::graphics::shaders

#endif