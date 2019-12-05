/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptValidator.cpp
-------------------------------------------
*/

#include "IonScriptValidator.h"

#include <algorithm>

#include "IonScriptTypes.h"
#include "timers/IonStopwatch.h"

namespace ion::script
{

using namespace script_validator;
using namespace script_error;

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace script_validator
{

namespace detail
{

class_reference::class_reference(std::string_view name, const ClassDefinition &owner) noexcept :
	Name{name}, Owner{&owner}
{
	//Empty
}

bool class_reference::operator<(const class_reference &rhs) const noexcept
{
	return std::make_pair(Name, Owner) < std::make_pair(rhs.Name, rhs.Owner);
}


/*
	Searching
*/

const ClassDefinition* find_inherited_class_definition_impl(const class_pointers &classes, const class_reference &class_ref)
{
	if (!std::empty(classes))
	{
		class_pointers more_classes;

		for (auto &class_def : classes)
		{
			if (class_def->Name() == class_ref.Name)
				return class_def; //A class definition found, stop searching
			
			//Search inner classes of base
			for (auto &inner_class : class_def->InnerClasses())
			{
				if (inner_class.HasDefinition() &&
					inner_class.Definition().Name() == class_ref.Name)
				{
					if (inner_class.Definition().Name() == class_ref.Name)
						return &inner_class.Definition(); //A class definition found, stop searching
				}
			}

			//Search deeper in inheritance hierarchy
			for (auto &base_class : class_def->BaseClasses())
			{
				if (base_class.HasDefinition())
					more_classes.push_back(&base_class.Definition());
			}
		}

		if (auto result = find_inherited_class_definition_impl(more_classes, class_ref); result)
			return result;
	}

	return nullptr;
}

const ClassDefinition* find_class_definition_impl(bool &unwinding, bool inheriting,
	const ClassDefinition &class_def, const class_reference &class_ref)
{
	//Class owner found
	if (&class_def == class_ref.Owner)
	{
		if (class_def.Name() == class_ref.Name)
			return &class_def; //Class definition found, stop searching

		//Search inheritance hierarchy of class reference owner
		if (auto result = find_inherited_class_definition(class_def, {class_ref.Name, class_def}); result)
			return result; //Class definition found in inheritance hierarchy, stop searching
		else
			unwinding = true; //Continue searching for a class definition while unwinding the stack
	}
	else
	{
		if (!unwinding)
		{
			//Search base classes
			for (auto &base_class : class_def.BaseClasses())
			{
				if (base_class.HasDefinition())
				{
					if (auto result = find_class_definition_impl(unwinding, inheriting = true, base_class.Definition(), class_ref); result)
						return result;

					else if (unwinding)
						break;
				}
			}
		}

		if (!unwinding)
		{
			//Search inner classes
			for (auto &class_decl : class_def.InnerClasses())
			{
				if (class_decl.HasDefinition())
				{
					if (auto result = find_class_definition_impl(unwinding, inheriting, class_decl.Definition(), class_ref); result)
						return result;

					else if (unwinding)
						break;
				}
			}
		}

		if (unwinding && !inheriting)
		{
			//Search siblings
			for (auto &sibling : class_def.InnerClasses())
			{
				if (sibling.HasDefinition() &&
					sibling.Name() == class_ref.Name)
					return &sibling.Definition(); //Class definition found, stop searching
			}
		}
	}

	return nullptr;
}

void find_all_declarations_impl(declarations_result &result, inhertied_class_pointers &inherited_classes,
	const ClassDefinition &root, const ClassDefinition &class_def, int depth)
{
	if (auto [iter, inserted] = inherited_classes.insert(&class_def); !inserted)
		return; //Do not inherit from the same class multiple times

	//Add and group inner classes by name
	for (auto &class_decl : class_def.InnerClasses())
		result.InnerClasses[class_decl.Name()].push_back({&class_decl, &class_def, depth});
	
	//Add and group properties by name
	for (auto &property : class_def.Properties())
		result.Properties[property.Name()].push_back({&property, &class_def, depth});


	//Search further down the inheritance hierarchy
	for (auto &base_class : class_def.BaseClasses())
	{
		auto base_class_def =
			[&]() noexcept
			{
				if (base_class.HasDefinition())
					return &base_class.Definition();
				else
					return find_class_definition(root, {base_class.Name(), class_def});
			}();

		if (base_class_def)
			find_all_declarations_impl(result, inherited_classes, root, *base_class_def, depth + 1);
	}
}


const ClassDefinition* find_inherited_class_definition(const ClassDefinition &class_def, const class_reference &class_ref)
{
	class_pointers classes;

	for (auto &base_class : class_def.BaseClasses())
	{
		if (base_class.HasDefinition())
			classes.push_back(&base_class.Definition());
	}

	return find_inherited_class_definition_impl(classes, class_ref);
}

const ClassDefinition* find_class_definition(const ClassDefinition &root, const class_reference &class_ref)
{
	auto unwinding = false;
	return find_class_definition_impl(unwinding, false, root, class_ref);
}

declarations_result find_all_declarations(const ClassDefinition &root, const ClassDefinition &class_def)
{
	declarations_result result;
	inhertied_class_pointers inherited_classes;
	find_all_declarations_impl(result, inherited_classes, root, class_def, 0);

	//Sort inner classes by depth
	for (auto &inner_class : result.InnerClasses)
		std::sort(std::begin(inner_class.second), std::end(inner_class.second));

	//Sort properties by depth
	for (auto &property : result.Properties)
		std::sort(std::begin(property.second), std::end(property.second));

	return result;
}


class_definition_cacher::class_definition_cacher(const ClassDefinition &root) noexcept :
	root_{root}
{
	//Empty
}

const ClassDefinition* class_definition_cacher::Get(const class_reference &class_ref)
{
	auto iter = cache_.find(class_ref);
	return iter == std::end(cache_) ?
		cache_.emplace(class_ref, find_class_definition(root_, class_ref)).first->second :
		iter->second;
}

class_declarations_cacher::class_declarations_cacher(const ClassDefinition &root) noexcept :
	root_{root}
{
	//Empty
}

const declarations_result& class_declarations_cacher::Get(const ClassDefinition &class_def)
{
	auto iter = cache_.find(&class_def);
	return iter == std::end(cache_) ?
		cache_.emplace(&class_def, find_all_declarations(root_, class_def)).first->second :
		iter->second;
}


/*
	Validating
*/

adaptors::FlatSet<std::string_view> get_required_properties(const properties_map &properties)
{
	adaptors::FlatSet<std::string_view> required_properties;

	for (auto &property : properties)
	{
		auto visible_depth = !std::empty(property.second) ?
			property.second.front().Depth : 0;

		for (auto &candidate : property.second)
		{
			if (candidate.Depth > visible_depth)
				break; //Hide names deeper than visible depth

			if (candidate.Declaration->Required())
				required_properties.insert(candidate.Declaration->Name());
		}
	}

	return required_properties;
}

adaptors::FlatSet<std::string_view> get_required_classes(const inner_classes_map &inner_classes)
{
	adaptors::FlatSet<std::string_view> required_classes;

	for (auto &inner_class : inner_classes)
	{
		for (auto &candidate : inner_class.second)
		{
			if (candidate.Declaration->Required())
				required_classes.insert(candidate.Declaration->Name());
		}
	}

	return required_classes;
}


bool validate_property(const script_tree::PropertyNode &property, const property_declarations &overload_set)
{
	auto match = false;
	auto visible_depth = !std::empty(overload_set) ?
		overload_set.front().Depth : 0;

	for (auto &candidate : overload_set)
	{
		if (candidate.Depth > visible_depth)
			break; //Hide names deeper than visible depth

		//Candidate has no definition
		//Nothing more to validate
		if (!candidate.Declaration->HasDefinition())
			return true;
							
		auto &candidate_def = candidate.Declaration->Definition();

		//Check if candidate has correct number of arguments
		if (static_cast<int>(std::size(property.Arguments())) >= candidate_def.RequiredParameters() &&
			static_cast<int>(std::size(property.Arguments())) <= std::size(candidate_def.Parameters()))
		{
			match = true;
			auto arg_iter = std::begin(property.Arguments());
			auto param_iter = std::begin(candidate_def.Parameters());		

			//Check if each argument type is equal to each parameter type
			for (; arg_iter != std::end(property.Arguments()); ++arg_iter, ++param_iter)
			{
				switch (param_iter->Type())
				{
					case ParameterType::Boolean:
					{
						match &= arg_iter->Visit(
							[](const ScriptType::Boolean&) noexcept
							{
								return true;
							},
							[](auto&&) noexcept
							{
								return false;
							});
						break;
					}

					case ParameterType::Color:
					{
						match &= arg_iter->Visit(
							[](const ScriptType::Color&) noexcept
							{
								return true;
							},
							[](auto&&) noexcept
							{
								return false;
							});
						break;
					}

					case ParameterType::Enumerable:
					{
						match &= arg_iter->Visit(
							[&](const ScriptType::Enumerable &value) noexcept
							{
								return param_iter->HasValue(value.Get());
							},
							[](auto&&) noexcept
							{
								return false;
							});
						break;
					}

					case ParameterType::FloatingPoint:
					{
						match &= arg_iter->Visit(
							[](const ScriptType::FloatingPoint&) noexcept
							{
								return true;
							},
							[](const ScriptType::Integer&) noexcept //Okay, non-narrowing
							{
								return true;
							},
							[](auto&&) noexcept
							{
								return false;
							});
						break;
					}

					case ParameterType::Integer:
					{
						match &= arg_iter->Visit(
							[](const ScriptType::Integer&) noexcept
							{
								return true;
							},
							[](auto&&) noexcept
							{
								return false;
							});
						break;
					}

					case ParameterType::String:
					{
						match &= arg_iter->Visit(
							[](const ScriptType::String&) noexcept
							{
								return true;
							},
							[](auto&&) noexcept
							{
								return false;
							});
						break;
					}

					case ParameterType::Vector2:
					{
						match &= arg_iter->Visit(
							[](const ScriptType::Vector2&) noexcept
							{
								return true;
							},
							[](auto&&) noexcept
							{
								return false;
							});
						break;
					}

					default:
					match = false;
					break;
				}

				//An argument/parameter type mismatch found
				//Skip candidate
				if (!match)
					break;
			}

			//A candidate match found
			if (match)
				break;
		}
	}

	return match;
}

bool validate_properties(const ScriptTree &tree, const script_tree::ObjectNode &object, const ClassDefinition &class_def,
	class_declarations_cacher &declarations_cacher, std::vector<ValidateError> &errors)
{
	auto [inner_classes, properties] = declarations_cacher.Get(class_def);
	auto required_properties = get_required_properties(properties);
	ValidateError error;

	for (auto &property : object.Properties())
	{
		if (auto iter = properties.find(property.Name());
			iter != std::end(properties))
		{
			if (!validate_property(property, iter->second))
			{
				error = {ValidateErrorCode::InvalidPropertyArguments, *tree.GetFullyQualifiedName(object, property)};
				errors.push_back(error);
			}

			required_properties.erase(property.Name());
		}
		else
		{
			error = {ValidateErrorCode::UnexpectedProperty, *tree.GetFullyQualifiedName(object, property)};
			errors.push_back(error);
		}
	}

	for (auto &required_property : required_properties)
	{
		error = {ValidateErrorCode::MissingRequiredProperty, *tree.GetFullyQualifiedName(object) + "." +
															 std::string{required_property}};
		errors.push_back(error);
	}

	return !error;
}

const ClassDefinition* validate_class(const ScriptTree &tree, const script_tree::ObjectNode &object, const ClassDefinition &class_owner,
	class_definition_cacher &definition_cacher, class_declarations_cacher &declarations_cacher, std::vector<ValidateError> &errors)
{
	auto [inner_classes, properties] = declarations_cacher.Get(class_owner);
	inner_class_declarations::value_type *class_candidate = nullptr;
	ValidateError error;

	if (auto iter = inner_classes.find(object.Name());
		iter != std::end(inner_classes))
	{
		//Check closest candidate with definition
		for (auto &candidate : iter->second)
		{
			if (class_candidate &&
				class_candidate->Depth < candidate.Depth)
				break; //Unambiguous definition found

			//Ambiguous definition found
			if (class_candidate)
			{
				error = {ValidateErrorCode::AmbiguousClass, *tree.GetFullyQualifiedName(object)};
				errors.push_back(error);
				break;
			}
			//A candidate found
			else
				class_candidate = &candidate;
		}
	}

	if (error)
		return nullptr;
	else if (!class_candidate)
	{
		error = {ValidateErrorCode::UnexpectedClass, *tree.GetFullyQualifiedName(object)};
		errors.push_back(error);
		return nullptr;
	}
	else if (!class_candidate->Declaration->Instantiatable())
	{
		error = {ValidateErrorCode::AbstractClassInstantiated, *tree.GetFullyQualifiedName(object)};
		errors.push_back(error);
		return nullptr;
	}


	auto class_def = class_candidate->Declaration->HasDefinition() ?
		&class_candidate->Declaration->Definition() :
		definition_cacher.Get({class_candidate->Declaration->Name(), *class_candidate->Owner});

	//Class definition found, validate properties
	if (class_def)
	{
		validate_properties(tree, object, *class_def, declarations_cacher, errors); //Ignoring return value
		return class_def;
	}
	else
		return nullptr;
}

bool validate(const ScriptTree &tree, const ClassDefinition &root, ValidateError &error, std::vector<ValidateError> &errors)
{
	//Start by validating the given tree
	auto result = validate_tree(tree, root, errors);
	
	//Set to first error
	if (!std::empty(errors))
		error = errors.front();

	return result;
}

bool validate_tree(const ScriptTree &tree, const ClassDefinition &root, std::vector<ValidateError> &errors)
{
	//No top level (global) classes found
	if (std::empty(root.InnerClasses()))
		return true; //Nothing to validate against
	
	struct scope
	{
		const script_tree::ObjectNode *object;
		const ClassDefinition &class_def;
		adaptors::FlatSet<std::string_view> required_classes;
	};

	class_definition_cacher definition_cacher{root};
	class_declarations_cacher declarations_cacher{root};	

	auto [root_classes, root_properties] = declarations_cacher.Get(root);
	std::vector<scope> scopes{{nullptr, root, get_required_classes(root_classes)}};
	auto next_search_depth = -1;
	ValidateError error;

	for (auto &[object, parent, depth] : tree.DepthFirstSearch())
	{
		if (next_search_depth >= 0)
		{
			//Next search depth reached
			if (next_search_depth >= depth)
				next_search_depth = -1;
			//Skip object
			else
				continue;
		}

		//Pop back until depth is equal to scope size
		while (depth + 1 < static_cast<int>(std::size(scopes)))
		{
			const auto &scope = scopes.back();

			for (auto &required_class : scope.required_classes)
			{
				error = {ValidateErrorCode::MissingRequiredClass, *tree.GetFullyQualifiedName(*scope.object) + "." +
																  std::string{required_class}};
				errors.push_back(error);
			}

			scopes.pop_back();
		}

		if (auto class_def = validate_class(tree, object, scopes.back().class_def, definition_cacher, declarations_cacher, errors); class_def)
		{
			auto [inner_classes, properties] = declarations_cacher.Get(*class_def);

			scopes.back().required_classes.erase(class_def->Name());
			scopes.push_back({&object, *class_def, get_required_classes(inner_classes)});
		}
		else
		{
			next_search_depth = depth; //Forward to next search depth
			scopes.back().required_classes.erase(object.Name());
		}
	}

	//Check if some required classes are missing
	for (const auto &scope : adaptors::ranges::ReverseIterable<decltype(scopes)&>{scopes})
	{
		for (auto &required_class : scope.required_classes)
		{
			error = {ValidateErrorCode::MissingRequiredClass, (scope.object ? *tree.GetFullyQualifiedName(*scope.object) + "." : "") +
															  std::string{required_class}};
			errors.push_back(error);
		}
	}

	return std::empty(errors);
}


/*
	Outputting
*/

std::string print_output(duration validate_time, const std::vector<ValidateError> &errors, OutputOptions output_options)
{
	std::string output;
	{
		auto error = !std::empty(errors) ? errors.front() : ValidateError{};
	
		//Find first error (if any)
		if (!error)
		{
			if (auto iter = std::find_if(std::begin(errors), std::end(errors),
				[](auto &error) noexcept
				{
					return !error;
				}); iter != std::end(errors))
				//Has error
				error = *iter;
		}	

		if (output_options == OutputOptions::Summary ||
			output_options == OutputOptions::SummaryAndErrors)
		{
			if (!std::empty(output))
				output += "\n\n";

			output += ion::utilities::string::Concat(
				"[Validator summary]\n"
				"Message - ",
					!error ?
					"Validation succeeded!" :
					ion::utilities::string::Concat("Validation failed. ", error.Condition.message(), " (", error.FullyQualifiedName, ")"),
				"\n"
				"Validate time - ", ion::utilities::string::Format(validate_time.count(), "0.0000"sv), " seconds\n"
				"Validation errors - ", std::size(errors));
		}
	}

	if (!std::empty(errors) &&
		(output_options == OutputOptions::Errors ||
		 output_options == OutputOptions::SummaryAndErrors))
	{
		if (!std::empty(output))
			output += "\n\n";

		output += "[Validation errors]";

		for (const auto &error : errors)
			output += ion::utilities::string::Concat(
				"\n", "Error. ", error.Condition.message(), " (", error.FullyQualifiedName, ")");
	}

	return output;
}

} //detail


//Parameter

ParameterDefinition::ParameterDefinition(ParameterType type) noexcept :
	type_{type}
{
	assert(type_ != ParameterType::Enumerable);
}

ParameterDefinition::ParameterDefinition(Strings values) noexcept :
	type_{ParameterType::Enumerable},
	values_{std::move(values)}
{
	assert(!std::empty(*values_));
}

ParameterDefinition::ParameterDefinition(std::initializer_list<std::string> values) :
	ParameterDefinition{Strings{values}}
{
	//Empty
}


/*
	Lookup
*/

bool ParameterDefinition::HasValue(std::string_view value) const noexcept
{
	assert(type_ == ParameterType::Enumerable && values_);
	return values_->contains(value);
}


//Property

PropertyDefinition::PropertyDefinition(std::string name, ParameterDefinition parameter) :
	name_{std::move(name)},
	parameters_{{std::move(parameter)}}
{
	//Empty
}

PropertyDefinition::PropertyDefinition(std::string name, ParameterDefinitions parameters) noexcept :
	name_{std::move(name)},
	parameters_{std::move(parameters)}
{
	//Empty
}

PropertyDefinition::PropertyDefinition(std::string name, ParameterDefinitions parameters, int required_parameters) noexcept :
	name_{std::move(name)},
	parameters_{std::move(parameters)},
	required_parameters_{required_parameters > 0 && required_parameters <= static_cast<int>(std::size(parameters_)) ?
						 required_parameters : static_cast<int>(std::size(parameters_))}
{
	//Empty
}


//ClassDefinition

ClassDefinition::ClassDefinition(std::string name) noexcept :
	name_{std::move(name)}
{
	//Empty
}

ClassDefinition::ClassDefinition(std::string name, std::string base_class) :
	name_{std::move(name)},
	base_classes_{{std::move(base_class), Ordinality::Optional, ClassType::Abstract}}
{
	//Empty
}

ClassDefinition::ClassDefinition(std::string name, ClassDefinition base_class) :
	name_{std::move(name)},
	base_classes_{{std::move(base_class), Ordinality::Optional, ClassType::Abstract}}
{
	//Empty
}

ClassDefinition::ClassDefinition(std::string name, std::initializer_list<EntityType<ClassDefinition>> base_classes) :
	name_{std::move(name)}
{
	for (auto &base_class : base_classes)
	{
		if (std::holds_alternative<ClassDefinition>(base_class))
			base_classes_.emplace(std::move(std::get<ClassDefinition>(base_class)), Ordinality::Optional, ClassType::Abstract);
		else
			base_classes_.emplace(std::move(std::get<std::string>(base_class)), Ordinality::Optional, ClassType::Abstract);
	}
}


/*
	Static functions
*/

ClassDefinition ClassDefinition::Create(std::string name) noexcept
{
	return {std::move(name)};
}

ClassDefinition ClassDefinition::Create(std::string name, std::string base_class)
{
	return {std::move(name), std::move(base_class)};
}

ClassDefinition ClassDefinition::Create(std::string name, ClassDefinition base_class)
{
	return {std::move(name), std::move(base_class)};
}

ClassDefinition ClassDefinition::Create(std::string name, std::initializer_list<EntityType<ClassDefinition>> base_classes)
{
	return {std::move(name), base_classes};
}


/*
	Classes
*/

ClassDefinition& ClassDefinition::AddClass(std::string name)
{
	inner_classes_.emplace(std::move(name), Ordinality::Optional, ClassType::Concrete);
	return *this;
}

ClassDefinition& ClassDefinition::AddClass(ClassDefinition class_def)
{
	inner_classes_.emplace(std::move(class_def), Ordinality::Optional, ClassType::Concrete);
	return *this;
}


ClassDefinition& ClassDefinition::AddAbstractClass(std::string name)
{
	inner_classes_.emplace(std::move(name), Ordinality::Optional, ClassType::Abstract);
	return *this;
}

ClassDefinition& ClassDefinition::AddAbstractClass(ClassDefinition class_def)
{
	inner_classes_.emplace(std::move(class_def), Ordinality::Optional, ClassType::Abstract);
	return *this;
}


ClassDefinition& ClassDefinition::AddRequiredClass(std::string name)
{
	inner_classes_.emplace(std::move(name), Ordinality::Mandatory, ClassType::Concrete);
	return *this;
}

ClassDefinition& ClassDefinition::AddRequiredClass(ClassDefinition class_def)
{
	inner_classes_.emplace(std::move(class_def), Ordinality::Mandatory, ClassType::Concrete);
	return *this;
}


/*
	Properties
*/

ClassDefinition& ClassDefinition::AddProperty(std::string name)
{
	properties_.emplace(std::move(name), Ordinality::Optional);
	return *this;
}

ClassDefinition& ClassDefinition::AddProperty(std::string name, ParameterDefinition parameter)
{
	properties_.emplace(PropertyDefinition{std::move(name), std::move(parameter)}, Ordinality::Optional);
	return *this;
}

ClassDefinition& ClassDefinition::AddProperty(std::string name, ParameterDefinitions parameters)
{
	properties_.emplace(PropertyDefinition{std::move(name), std::move(parameters)}, Ordinality::Optional);
	return *this;
}

ClassDefinition& ClassDefinition::AddProperty(std::string name, ParameterDefinitions parameters, int required_parameters)
{
	properties_.emplace(PropertyDefinition{std::move(name), std::move(parameters), required_parameters}, Ordinality::Optional);
	return *this;
}

ClassDefinition& ClassDefinition::AddProperty(PropertyDefinition property)
{
	properties_.emplace(std::move(property), Ordinality::Optional);
	return *this;
}


ClassDefinition& ClassDefinition::AddRequiredProperty(std::string name)
{
	properties_.emplace(std::move(name), Ordinality::Mandatory);
	return *this;
}

ClassDefinition& ClassDefinition::AddRequiredProperty(std::string name, ParameterDefinition parameter)
{
	properties_.emplace(PropertyDefinition{std::move(name), std::move(parameter)}, Ordinality::Mandatory);
	return *this;
}

ClassDefinition& ClassDefinition::AddRequiredProperty(std::string name, ParameterDefinitions parameters)
{
	properties_.emplace(PropertyDefinition{std::move(name), std::move(parameters)}, Ordinality::Mandatory);
	return *this;
}

ClassDefinition& ClassDefinition::AddRequiredProperty(std::string name, ParameterDefinitions parameters, int required_parameters)
{
	properties_.emplace(PropertyDefinition{std::move(name), std::move(parameters), required_parameters}, Ordinality::Mandatory);
	return *this;
}

ClassDefinition& ClassDefinition::AddRequiredProperty(PropertyDefinition property)
{
	properties_.emplace(std::move(property), Ordinality::Mandatory);
	return *this;
}


/*
	Lookup
*/

const ClassDeclaration* ClassDefinition::GetBaseClass(std::string_view name) const noexcept
{
	auto iter = base_classes_.find(name);
	return iter != std::end(base_classes_) ? &*iter : nullptr;
}

const ClassDeclaration* ClassDefinition::GetInnerClass(std::string_view name) const noexcept
{
	auto iter = inner_classes_.find(name);
	return iter != std::end(inner_classes_) ? &*iter : nullptr;
}

const PropertyDeclaration* ClassDefinition::GetProperty(std::string_view name) const noexcept
{
	auto iter = properties_.find(name);
	return iter != std::end(properties_) ? &*iter : nullptr;
}


//ClassDeclaration

ClassDeclaration::ClassDeclaration(std::string name, Ordinality ordinality, ClassType class_type) noexcept :
	detail::declaration<ClassDefinition>{std::move(name), ordinality},
	class_type_{class_type}
{
	//Empty
}

ClassDeclaration::ClassDeclaration(ClassDefinition definition, Ordinality ordinality, ClassType class_type) noexcept :
	detail::declaration<ClassDefinition>{std::move(definition), ordinality},
	class_type_{class_type}
{
	//Empty
}

} //script_validator


/*
	Classes
*/

ScriptValidator& ScriptValidator::AddClass(std::string name)
{
	root_.AddClass(std::move(name));
	return *this;
}

ScriptValidator& ScriptValidator::AddClass(ClassDefinition class_def)
{
	root_.AddClass(std::move(class_def));
	return *this;
}


ScriptValidator& ScriptValidator::AddAbstractClass(ClassDefinition class_def)
{
	root_.AddAbstractClass(std::move(class_def));
	return *this;
}


ScriptValidator& ScriptValidator::AddRequiredClass(std::string name)
{
	root_.AddRequiredClass(std::move(name));
	return *this;
}

ScriptValidator& ScriptValidator::AddRequiredClass(ClassDefinition class_def)
{
	root_.AddRequiredClass(std::move(class_def));
	return *this;
}


/*
	Lookup
*/

const ClassDeclaration* ScriptValidator::GetClass(std::string_view name) const noexcept
{
	return root_.GetInnerClass(name);
}


/*
	Outputting
*/

std::string ScriptValidator::PrintOutput(OutputOptions output_options) const
{
	return script_validator::detail::print_output(validate_time_, validate_errors_, output_options);
}


/*
	Validating
*/

ScriptValidator ScriptValidator::Create() noexcept
{
	return {};
}

bool ScriptValidator::Validate(const ScriptTree &tree, ValidateError &error)
{
	validate_errors_.clear();
	validate_time_ = {}; //Reset

	auto stopwatch = timers::Stopwatch::StartNew();
	auto result = script_validator::detail::validate(tree, root_, error, validate_errors_);
	validate_time_ = stopwatch.Elapsed();
	return result;
}

} //ion::script