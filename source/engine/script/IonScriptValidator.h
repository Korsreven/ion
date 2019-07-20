/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptValidator.h
-------------------------------------------
*/

#ifndef _ION_SCRIPT_VALIDATOR_
#define _ION_SCRIPT_VALIDATOR_

#include <cassert>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "IonScriptError.h"
#include "IonScriptTree.h"
#include "adaptors/IonFlatMap.h"
#include "adaptors/IonFlatSet.h"
#include "types/IonTypes.h"
#include "types/IonTypeTraits.h"

namespace ion::script
{
	namespace script_validator
	{
		enum class ParameterType
		{
			Boolean,
			Color,
			Enumerable,
			Number,
			String,
			Vector2
		};

		enum class Ordinality : bool
		{
			Mandatory,
			Optional
		};

		enum class ClassType : bool
		{
			Abstract,
			Concrete
		};


		class ParameterDefinition;
		class PropertyDefinition;
		class ClassDefinition;

		struct PropertyDeclaration;
		class ClassDeclaration;

		using ParameterDefinitions = std::vector<ParameterDefinition>;
		using PropertyDeclarations = adaptors::FlatSet<PropertyDeclaration>;
		using ClassDeclarations = adaptors::FlatSet<ClassDeclaration>;

		template <typename T>
		using EntityType = std::variant<std::string, T>;


		namespace detail
		{
			template <typename T>
			class declaration
			{
				static_assert(std::is_same_v<T, PropertyDefinition> || std::is_same_v<T, ClassDefinition>);

				private:

					EntityType<T> entity_;
					Ordinality ordinality_ = Ordinality::Optional;

				public:

					//Construct an incomplete declaration (no definition)
					declaration(std::string name, Ordinality ordinality) noexcept :
						entity_{std::move(name)},
						ordinality_{ordinality}
					{
						//Empty
					}

					//Construct a complete declaration (full definition)
					declaration(T definition, Ordinality ordinality) noexcept :
						entity_{std::move(definition)},
						ordinality_{ordinality}
					{
						//Empty
					}


					/*
						Observers
					*/

					//Returns true if this declaration has a definition
					inline auto HasDefinition() const noexcept
					{
						return std::holds_alternative<T>(entity_);
					}

					//Returns the name from this declaration
					//Could be just a name or the name of the actual definition
					inline const auto& Name() const noexcept
					{
						return HasDefinition() ?
							std::get<T>(entity_).Name() :
							std::get<std::string>(entity_);
					}

					//Returns an immutable reference to the definition
					inline const auto& Definition() const noexcept
					{
						assert(HasDefinition());
						return std::get<T>(entity_);
					}

					//Returns true if this declaration is required
					inline auto Required() const noexcept
					{
						return ordinality_ == Ordinality::Mandatory;
					}


					/*
						Comparators
					*/

					//Returns true if this declaration should be sorted before the given declaration
					inline auto operator<(const declaration &rhs) const noexcept
					{
						return Name() < rhs.Name();
					}

					//Returns true if this declaration should be sorted before the given name
					//This operator is only enabled for string types
					template <typename T, typename = std::enable_if_t<types::is_string_v<T>>>
					inline auto operator<(const T &rhs) const noexcept
					{
						return Name() < rhs;
					}

					//Returns true if the given name should be sorted before the given declaration
					//This operator is only enabled for string types
					template <typename T, typename = std::enable_if_t<types::is_string_v<T>>>
					inline friend auto operator<(const T &lhs, const declaration &rhs) noexcept
					{
						return lhs < rhs.Name();
					}
			};

			struct class_reference final
			{
				std::string_view Name;
				const ClassDefinition *Owner = nullptr;

				class_reference(std::string_view name, const ClassDefinition &owner) noexcept;
				bool operator<(const class_reference &rhs) const noexcept;
			};


			/*
				Searching
			*/

			template <typename T>
			struct declaration_result final
			{
				const T *Declaration = nullptr;
				const ClassDefinition *Owner = nullptr;
				int Depth = 0;

				inline auto operator<(const declaration_result &rhs) const noexcept
				{
					return Depth < rhs.Depth;
				}
			};

			using inner_class_declarations = std::vector<declaration_result<ClassDeclaration>>;
			using property_declarations = std::vector<declaration_result<PropertyDeclaration>>;
			using inner_classes_map = adaptors::FlatMap<std::string_view, inner_class_declarations>;
			using properties_map = adaptors::FlatMap<std::string_view, property_declarations>;

			struct declarations_result
			{
				inner_classes_map InnerClasses;
				properties_map Properties;
			};

			using class_pointers = std::vector<const ClassDefinition*>;
			using inhertied_class_pointers = adaptors::FlatSet<const ClassDefinition*>;


			const ClassDefinition* find_inherited_class_definition_impl(const class_pointers &classes, const class_reference &class_ref);
			const ClassDefinition* find_class_definition_impl(bool &unwinding, bool inheriting,
				const ClassDefinition &class_def, const class_reference &class_ref);
			void find_all_declarations_impl(declarations_result &result, inhertied_class_pointers &inherited_classes,
				const ClassDefinition &root, const ClassDefinition &class_def, int depth);		

			const ClassDefinition* find_inherited_class_definition(const ClassDefinition &class_def, const class_reference &class_ref);
			const ClassDefinition* find_class_definition(const ClassDefinition &root, const class_reference &class_ref);
			declarations_result find_all_declarations(const ClassDefinition &root, const ClassDefinition &class_def);


			class class_definition_cacher final
			{
				private:

					const ClassDefinition &root_;
					adaptors::FlatMap<class_reference, const ClassDefinition*> cache_;				

				public:

					class_definition_cacher(const ClassDefinition &root) noexcept;
					const ClassDefinition* Get(const class_reference &class_ref);
			};

			class class_declarations_cacher final
			{
				private:

					const ClassDefinition &root_;
					adaptors::FlatMap<const ClassDefinition*, declarations_result> cache_;

				public:

					class_declarations_cacher(const ClassDefinition &root) noexcept;
					const declarations_result& Get(const ClassDefinition &class_def);
			};


			/*
				Validating
			*/

			adaptors::FlatSet<std::string_view> get_required_properties(const properties_map &properties);
			adaptors::FlatSet<std::string_view> get_required_classes(const inner_classes_map &inner_classes);

			bool validate_property(const script_tree::PropertyNode &property, const property_declarations &overload_set);
			bool validate_properties(const ScriptTree &tree, const script_tree::ObjectNode &object, const ClassDefinition &class_def,
				class_declarations_cacher &declarations_cacher, ValidateError &error);
			const ClassDefinition* validate_class(const ScriptTree &tree, const script_tree::ObjectNode &object, const ClassDefinition &class_owner,
				class_definition_cacher &definition_cacher, class_declarations_cacher &declarations_cacher, ValidateError &error);

			bool validate(const ScriptTree &tree, const ClassDefinition &root, ValidateError &error);
		} //detail


		class ParameterDefinition final
		{
			private:

				ParameterType type_;
				std::optional<adaptors::FlatSet<std::string>> values_;

			public:

				//Construct a new parameter with the given type
				ParameterDefinition(ParameterType type) noexcept;

				//Construct a new enumerable parameter with the given values
				ParameterDefinition(Strings values) noexcept;

				//Construct a new enumerable parameter with the given values
				ParameterDefinition(std::initializer_list<std::string> values);


				/*
					Observers
				*/

				//Returns the parameter type
				[[nodiscard]] inline auto Type() const noexcept
				{
					return type_;
				}


				/*
					Lookup
				*/

				//Returns true if an enumerable has support for the given value
				[[nodiscard]] bool HasValue(std::string_view value) const noexcept;


				/*
					Ranges
				*/

				//Returns an immutable range of all values for this enumerable parameter
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto Values() const noexcept
				{
					assert(type_ == ParameterType::Enumerable && values_);
					return values_->Elements();
				}
		};

		class PropertyDefinition final
		{
			private:

				std::string name_;
				ParameterDefinitions parameters_;
				int required_parameters_ = std::size(parameters_);

			public:

				//Construct a new property with the given name and parameter
				PropertyDefinition(std::string name, ParameterDefinition parameter);

				//Construct a new property with the given name and parameters
				PropertyDefinition(std::string name, ParameterDefinitions parameters) noexcept;

				//Construct a new property with the given name, parameters and required parameters
				PropertyDefinition(std::string name, ParameterDefinitions parameters, int required_parameters) noexcept;


				/*
					Observers
				*/

				//Returns the name of this property
				[[nodiscard]] inline const auto &Name() const noexcept
				{
					return name_;
				}

				//Returns the max required parameters of this property
				[[nodiscard]] inline const auto &RequiredParameters() const noexcept
				{
					return required_parameters_;
				}


				/*
					Ranges
				*/

				//Returns an immutable range of all parameters for this property
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto Parameters() const noexcept
				{
					return adaptors::ranges::Iterable<const ParameterDefinitions&>{parameters_};
				}
		};

		class ClassDefinition final
		{
			private:

				std::string name_;
				PropertyDeclarations properties_;
				ClassDeclarations base_classes_;				
				ClassDeclarations inner_classes_;

			public:

				//Construct a new class definition with the given name
				ClassDefinition(std::string name) noexcept;

				//Construct a new class definition with the given name and base class name
				ClassDefinition(std::string name, std::string base_class);

				//Construct a new class definition with the given name and base class definition
				ClassDefinition(std::string name, ClassDefinition base_class);

				//Construct a new class with the given name and base classes
				ClassDefinition(std::string name, std::initializer_list<EntityType<ClassDefinition>> base_classes);


				/*
					Static functions
				*/

				//Returns a newly created class definition with the given name
				//Designed for fluent interface by using function chaining (named parameter idiom)
				[[nodiscard]] static ClassDefinition Create(std::string name) noexcept;

				//Returns a newly created class definition with the given name and base class name
				//Designed for fluent interface by using function chaining (named parameter idiom)
				[[nodiscard]] static ClassDefinition Create(std::string name, std::string base_class);

				//Returns a newly created class definition with the given name and base class definition
				//Designed for fluent interface by using function chaining (named parameter idiom)
				[[nodiscard]] static ClassDefinition Create(std::string name, ClassDefinition base_class);

				//Returns a newly created class definition with the given name and base classes
				//Designed for fluent interface by using function chaining (named parameter idiom)
				[[nodiscard]] static ClassDefinition Create(std::string name, std::initializer_list<EntityType<ClassDefinition>> base_classes);


				/*
					Observers
				*/

				//Returns the name of this class
				[[nodiscard]] inline const auto &Name() const noexcept
				{
					return name_;
				}


				/*
					Classes
				*/

				//Add an inner class with the given name
				//Could be a class with no definition or an existing class defined in an outer scope
				ClassDefinition& AddClass(std::string name);

				//Add an inner class with the given definition
				ClassDefinition& AddClass(ClassDefinition class_def);


				//Add an abstract inner class with the given name
				//Could be a class with no definition or an existing class defined in an outer scope
				ClassDefinition& AddAbstractClass(std::string name);

				//Add an abstract inner class with the given definition
				ClassDefinition& AddAbstractClass(ClassDefinition class_def);


				//Add a required inner class with the given name
				//Could be a class with no definition or an existing class defined in an outer scope
				ClassDefinition& AddRequiredClass(std::string name);

				//Add a required inner class with the given definition
				ClassDefinition& AddRequiredClass(ClassDefinition class_def);


				/*
					Properties
				*/

				//Add a property with the given name
				ClassDefinition& AddProperty(std::string name);

				//Add a property with the given name and parameter
				ClassDefinition& AddProperty(std::string name, ParameterDefinition parameter);
				
				//Add a property with the given name and parameters
				ClassDefinition& AddProperty(std::string name, ParameterDefinitions parameters);
				
				//Add a property with the given name, parameters and the required number of parameters in range [1, |parameters|]
				ClassDefinition& AddProperty(std::string name, ParameterDefinitions parameters, int required_parameters);

				//Add a property with the given property definition
				ClassDefinition& AddProperty(PropertyDefinition property);


				//Add a required property with the given name
				ClassDefinition& AddRequiredProperty(std::string name);

				//Add a required property with the given name and parameter
				ClassDefinition& AddRequiredProperty(std::string name, ParameterDefinition parameter);
				
				//Add a required property with the given name and parameters
				ClassDefinition& AddRequiredProperty(std::string name, ParameterDefinitions parameters);
				
				//Add a required property with the given name, parameters and the required number of parameters in range [1, |parameters|]
				ClassDefinition& AddRequiredProperty(std::string name, ParameterDefinitions parameters, int required_parameters);

				//Add a required property with the given property definition
				ClassDefinition& AddRequiredProperty(PropertyDefinition property);


				/*
					Lookup
				*/

				//Returns a pointer to an immutable ClassDeclaration with the given name
				//If no base class with the given name is found, it returns nullptr
				[[nodiscard]] const ClassDeclaration* GetBaseClass(std::string_view name) const noexcept;

				//Returns a pointer to an immutable ClassDeclaration with the given name
				//If no inner class with the given name is found, it returns nullptr
				[[nodiscard]] const ClassDeclaration* GetInnerClass(std::string_view name) const noexcept;

				//Returns a pointer to an immutable PropertyDeclaration with the given name
				//If no property with the given name is found, it returns nullptr
				[[nodiscard]] const PropertyDeclaration* GetProperty(std::string_view name) const noexcept;


				/*
					Ranges
				*/

				//Returns an immutable range of all base classes of this class
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto BaseClasses() const noexcept
				{
					return base_classes_.Elements();
				}

				//Returns an immutable range of all inner classes of this class
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto InnerClasses() const noexcept
				{
					return inner_classes_.Elements();
				}

				//Returns an immutable range of all properties of this class
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto Properties() const noexcept
				{
					return properties_.Elements();
				}
		};


		

		struct PropertyDeclaration final : detail::declaration<PropertyDefinition>
		{
			using detail::declaration<PropertyDefinition>::declaration;
		};

		class ClassDeclaration final : public detail::declaration<ClassDefinition>
		{
			private:

				ClassType class_type_ = ClassType::Concrete;

			public:

				//Construct an incomplete declaration (no definition)
				ClassDeclaration(std::string name, Ordinality ordinality, ClassType class_type) noexcept;

				//Construct a complete declaration (full definition)
				ClassDeclaration(ClassDefinition definition, Ordinality ordinality, ClassType class_type) noexcept;


				/*
					Observers
				*/

				//Returns true if this class declaration is instantiatable
				[[nodiscard]] inline auto Instantiatable() const noexcept
				{
					return class_type_ == ClassType::Concrete;
				}
		};
	} //script_validator


	class ScriptTree;

	class ScriptValidator final
	{
		private:

			script_validator::ClassDefinition root_{{}};
    
		public:

			ScriptValidator() = default;


			/*
				Static functions
			*/

			//Returns a newly created script validator
			//Designed for fluent interface by using function chaining (named parameter idiom)
			[[nodiscard]] static ScriptValidator Create() noexcept;


			/*
				Classes
			*/

			//Add a class with the given name (no definition)
			ScriptValidator& AddClass(std::string name);

			//Add a class with the given definition
			ScriptValidator& AddClass(script_validator::ClassDefinition class_def);


			//Add an abstract class with the given definition
			ScriptValidator& AddAbstractClass(script_validator::ClassDefinition class_def);


			//Add a required class with the given name (no definition)
			ScriptValidator& AddRequiredClass(std::string name);

			//Add a required class with the given definition
			ScriptValidator& AddRequiredClass(script_validator::ClassDefinition class_def);


			/*
				Validating
			*/

			//Returns true if this validator validates the given script tree
			[[nodiscard]] bool Validate(const ScriptTree &tree, ValidateError &error) const noexcept;


			/*
				Lookup
			*/

			//Returns a pointer to an immutable ClassDeclaration with the given name
			//If no class with the given name is found, it returns nullptr
			[[nodiscard]] const script_validator::ClassDeclaration* GetClass(std::string_view name) const noexcept;


			/*
				Ranges
			*/

			//Returns an immutable range of all classes in this validator
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Classes() const noexcept
			{
				return root_.InnerClasses();
			}
	};

} //ion::script

#endif