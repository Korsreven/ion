/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptTree.cpp
-------------------------------------------
*/

#include "IonScriptTree.h"

#include <utility>
#include "utilities/IonParseUtility.h"

namespace ion::script
{

using namespace script_tree;

namespace script_tree
{

namespace detail
{

using namespace std::string_literals;
using namespace types::type_literals;


/*
	Serializing
*/

void serialize_argument(const ArgumentNode &argument, std::vector<std::byte> &bytes)
{
	argument.Visit(
		[&](auto &&arg)
		{
			return serialize_argument(arg, bytes);
		});
	serialize_value(argument.Unit(), bytes);
}

void serialize_property(const PropertyNode &property, std::vector<std::byte> &bytes)
{
	for (auto &argument : property.Arguments())
		serialize_argument(argument, bytes);

	bytes.push_back(static_cast<std::byte>(serialization_section_token::Property));
	serialize_value(property.Name(), bytes);
}

void serialize_object(const TreeNode &node, std::vector<std::byte> &bytes)
{
	for (auto &property : node.Object.Properties())
		serialize_property(property, bytes);

	bytes.push_back(static_cast<std::byte>(serialization_section_token::Object));
	serialize_value(node.Depth, bytes);
	serialize_value(node.Object.Name(), bytes);
	serialize_value(node.Object.Classes(), bytes);
}

std::vector<std::byte> serialize(const ObjectNodes &objects)
{
	std::vector<std::byte> bytes;

	for (auto &node : depth_first_search(objects, DepthFirstTraversal::PostOrder))
		serialize_object(node, bytes);

	return bytes;
}


int deserialize_argument(std::string_view bytes, ArgumentNodes &arguments)
{
	auto bytes_deserialized = 0;

	if (!std::empty(bytes))
	{
		switch (static_cast<size_t>(bytes.front()))
		{
			case variant_index<ArgumentType, BooleanArgument>():
			{
				bytes_deserialized = deserialize_argument<BooleanArgument>(bytes.substr(1), arguments);
				break;
			}

			case variant_index<ArgumentType, ColorArgument>():
			{
				bytes_deserialized = deserialize_argument<ColorArgument>(bytes.substr(1), arguments);
				break;
			}

			case variant_index<ArgumentType, EnumerableArgument>():
			{
				bytes_deserialized = deserialize_argument<EnumerableArgument>(bytes.substr(1), arguments);
				break;
			}

			case variant_index<ArgumentType, FloatingPointArgument>():
			{
				bytes_deserialized = deserialize_argument<FloatingPointArgument>(bytes.substr(1), arguments);
				break;
			}

			case variant_index<ArgumentType, IntegerArgument>():
			{
				bytes_deserialized = deserialize_argument<IntegerArgument>(bytes.substr(1), arguments);
				break;
			}

			case variant_index<ArgumentType, StringArgument>():
			{
				bytes_deserialized = deserialize_argument<StringArgument>(bytes.substr(1), arguments);
				break;
			}

			case variant_index<ArgumentType, Vector2Argument>():
			{
				bytes_deserialized = deserialize_argument<Vector2Argument>(bytes.substr(1), arguments);
				break;
			}
		}
	}
	
	if (bytes_deserialized > 0)
		++bytes_deserialized;

	return bytes_deserialized;
}

int deserialize_property(std::string_view bytes, PropertyNodes &properties, ArgumentNodes &arguments)
{
	std::string name;
	auto bytes_deserialized = deserialize_value(bytes, name);

	if (bytes_deserialized > 0)
		properties.emplace_back(std::move(name), std::move(arguments));

	return bytes_deserialized;
}

int deserialize_object(std::string_view bytes, std::vector<ObjectNodes> &object_stack, PropertyNodes &properties)
{
	int depth;
	auto depth_bytes_deserialized = deserialize_value(bytes, depth);

	std::string name;
	auto name_bytes_deserialized = depth_bytes_deserialized > 0 ?
		deserialize_value(bytes.substr(depth_bytes_deserialized), name) : 0;

	std::string classes;
	auto classes_bytes_deserialized = name_bytes_deserialized > 0 ?
		deserialize_value(bytes.substr(depth_bytes_deserialized + name_bytes_deserialized), classes) : 0;

	//Node depth, object name and classes serialized
	if (classes_bytes_deserialized > 0)
	{
		if (depth >= static_cast<int>(std::size(object_stack)))
			object_stack.insert(std::end(object_stack), depth - std::size(object_stack) + 1, {});

		//Leaf object, no childrens
		if (auto &object = object_stack[depth];
			&object == &object_stack.back())
			object.emplace_back(std::move(name), std::move(classes), std::move(properties));
		//Take childrens (if any) from one depth deeper
		else
			object.emplace_back(std::move(name), std::move(classes), std::move(properties),
								std::move(object_stack[depth + 1]));

		return depth_bytes_deserialized + name_bytes_deserialized + classes_bytes_deserialized;
	}
	
	return 0;
}

std::optional<ObjectNodes> deserialize(std::string_view bytes)
{
	std::vector<ObjectNodes> object_stack;
	PropertyNodes properties;
	ArgumentNodes arguments;

	for (auto iter = std::begin(bytes), end = std::end(bytes); iter != end;)
	{
		auto c = *iter;
		auto off = iter - std::begin(bytes);

		auto bytes_deserialized = 0;

		switch (static_cast<serialization_section_token>(c))
		{
			case serialization_section_token::Object:
			{	
				//An object requires that any previous arguments, should have been processed by a property first
				if (std::empty(arguments))
					bytes_deserialized = deserialize_object(bytes.substr(off + 1), object_stack, properties);
				
				break;
			}

			case serialization_section_token::Property:
			{
				//A property requires one or more previous arguments
				if (!std::empty(arguments))
					bytes_deserialized = deserialize_property(bytes.substr(off + 1), properties, arguments);
				
				break;
			}

			case serialization_section_token::Argument:
			{
				bytes_deserialized = deserialize_argument(bytes.substr(off + 1), arguments);
				break;
			}
		}

		if (bytes_deserialized > 0)
			iter += bytes_deserialized + 1;
		else
		{
			object_stack.clear();
			break;
		}
	}

	return !std::empty(object_stack) ?
		//Top-level objects
		std::make_optional<ObjectNodes>(std::move(object_stack.front())) :
		std::nullopt;
}


/*
	Printing
*/

std::string print(const ObjectNodes &objects, PrintOptions print_options)
{
	std::string output;

	//Print objects
	for (auto &[object, parent, depth] : depth_first_search(objects, DepthFirstTraversal::PreOrder))
	{
		output += "\n" + std::string(depth * 4, ' ') + "+-- " + object.Name();

		if (auto &classes = object.Classes(); !std::empty(classes))
			output += " \"" + classes + "\"";

		if (print_options == PrintOptions::ObjectsWithProperties ||
			print_options == PrintOptions::ObjectsWithPropertiesAndArguments)
		{
			//Print properties
			for (auto &property : object.Properties())
			{
				output += "\n" + std::string(depth * 4 + 4, ' ') + "|-- " + property.Name();

				if (print_options == PrintOptions::ObjectsWithPropertiesAndArguments)
				{
					if (auto remaining_args = static_cast<int>(property.Arguments().size());
						remaining_args > 0)
					{
						output += ": ";

						//Print arguments
						for (auto &argument : property.Arguments())
						{
							output += argument.Visit(
								[](const BooleanArgument &arg)
								{
									return arg.Get() ? "true"s : "false"s;
								},
								[](const ColorArgument &arg)
								{
									auto name = std::string{utilities::parse::AsString(arg.Get()).value_or("")};
									auto [r, g, b] = arg.Get().ToRGB();
									auto a = arg.Get().A();

									if (a < 1.0_r)
										return ion::utilities::string::Format(name + "({0}, {1}, {2}, {3:0.##})", r, g, b, a);
									else
										return ion::utilities::string::Format(name + "({0}, {1}, {2})", r, g, b);
								},
								[](const EnumerableArgument &arg)
								{
									return arg.Get();
								},
								[](const StringArgument &arg)
								{
									return ion::utilities::string::Concat('"', arg.Get(), '"');
								},
								[](const Vector2Argument &arg)
								{
									auto [x, y] = arg.Get().XY();
									return ion::utilities::string::Concat('{', x, ", ", y, '}');
								},
								//Default
								[&](auto &&arg)
								{
									return ion::utilities::convert::ToString(arg.Get()) + argument.Unit();
								});

							if (--remaining_args > 0)
								output += " ";
						}
					}
				}
			}
		}
	}

	return output;
}


/*
	Searching
*/

void breadth_first_search_impl(search_result &result, size_t off)
{
	auto last = std::size(result);

	if (last - off > 0)
	{
		for (; off < last; ++off)
		{
			for (auto &object : result[off].Object.Objects())
				result.emplace_back(object, result[off].Object, result[off].Depth + 1);
		}

		breadth_first_search_impl(result, last);
	}
}

void depth_first_search_post_order_impl(search_result &result, const TreeNode &node)
{
	for (auto &object : node.Object.Objects())
		depth_first_search_post_order_impl(result, {object, node.Object, node.Depth + 1});

	result.push_back(node);
}

void depth_first_search_pre_order_impl(search_result &result, const TreeNode &node)
{
	result.push_back(node);

	for (auto &object : node.Object.Objects())
		depth_first_search_pre_order_impl(result, {object, node.Object, node.Depth + 1});
}

void fully_qualified_name_impl(std::string &name, const ObjectNode &parent, const ObjectNode &what_object)
{
	if (&parent == &what_object)
	{
		name = what_object.Name();
		return;
	}

	for (auto &object : parent.Objects())
	{
		fully_qualified_name_impl(name, object, what_object);

		if (!std::empty(name))
		{
			name = parent.Name() + "." + name;
			break;
		}
	}
}


search_result breadth_first_search(const ObjectNodes &objects)
{
	search_result result;

	for (auto &object : objects)
		result.emplace_back(object);

	breadth_first_search_impl(result, 0);
	return result;
}

search_result depth_first_search(const ObjectNodes &objects, DepthFirstTraversal traversal)
{
	search_result result;

	//Post-order
	if (traversal == DepthFirstTraversal::PostOrder)
	{
		for (auto &object : objects)
			depth_first_search_post_order_impl(result, {object});
	}
	//Pre-order
	else
	{
		for (auto &object : objects)
			depth_first_search_pre_order_impl(result, {object});
	}

	return result;
}

std::string fully_qualified_name(const ObjectNodes &objects, const ObjectNode &what_object)
{
	std::string name;

	for (auto &object : objects)
	{
		fully_qualified_name_impl(name, object, what_object);

		if (!std::empty(name))
			break;
	}

	return name;
}


void lineage_depth_first_search_impl(lineage_search_result &result, generations &descendants, ObjectNode &object)
{
	result.push_back(descendants);

	if (!std::empty(object.Objects()))
	{
		descendants.emplace_back(); //Add next generation

		for (auto &sibling : object.Objects())
		{
			descendants.back().push_back(&sibling);
			lineage_depth_first_search_impl(result, descendants, sibling);
		}

		descendants.pop_back();
	}
}

lineage_search_result lineage_depth_first_search(ObjectNodes &objects)
{
	lineage_search_result result;
	generations descendants;
	descendants.emplace_back(); //Add next generation

	for (auto &sibling : objects)
	{
		descendants.back().push_back(&sibling);
		lineage_depth_first_search_impl(result, descendants, sibling);
	}

	return result;
}

} //detail


//TreeNode

TreeNode::TreeNode(const ObjectNode &object) noexcept :
	Object{object}
{
	//Empty
}

TreeNode::TreeNode(const ObjectNode &object, const ObjectNode &parent, int depth) noexcept :
	Object{object},
	Parent{&parent},
	Depth{depth}
{
	//Empty
}


//ObjectNode

ObjectNode::ObjectNode(std::string name, std::string classes, PropertyNodes properties) noexcept :
	ObjectNode{std::move(name), std::move(classes), std::move(properties), {}}
{
	//Empty
}

ObjectNode::ObjectNode(std::string name, std::string classes, PropertyNodes properties, ObjectNodes objects) noexcept :
	name_{std::move(name)},
	classes_{std::move(classes)},
	properties_{std::move(properties)},
	objects_{std::move(objects)}
{
	//Empty
}


/*
	Appending
*/

void ObjectNode::Append(const ObjectNodes &objects, AppendCondition append_condition)
{
	if (*this) //Not allowed to modify if not valid
		detail::append_nodes(objects_, objects, append_condition);
}

void ObjectNode::Append(const adaptors::ranges::Iterable<ObjectNodes&> &objects, AppendCondition append_condition)
{
	if (*this) //Not allowed to modify if not valid
		detail::append_nodes(objects_, objects, append_condition);
}

void ObjectNode::Append(const adaptors::ranges::Iterable<const ObjectNodes&> &objects, AppendCondition append_condition)
{
	if (*this) //Not allowed to modify if not valid
		detail::append_nodes(objects_, objects, append_condition);
}


void ObjectNode::Append(const PropertyNodes &properties, AppendCondition append_condition)
{
	if (*this) //Not allowed to modify if not valid
		detail::append_nodes(properties_, properties, append_condition);
}

void ObjectNode::Append(const adaptors::ranges::Iterable<PropertyNodes&> &properties, AppendCondition append_condition)
{
	if (*this) //Not allowed to modify if not valid
		detail::append_nodes(properties_, properties, append_condition);
}

void ObjectNode::Append(const adaptors::ranges::Iterable<const PropertyNodes&> &properties, AppendCondition append_condition)
{
	if (*this) //Not allowed to modify if not valid
		detail::append_nodes(properties_, properties, append_condition);
}


/*
	Finding / searching
*/

ObjectNode& ObjectNode::Find(std::string_view name) noexcept
{
	for (auto &object : objects_)
	{
		if (object.Name() == name)
			return object;
	}

	return const_cast<ObjectNode&>(InvalidObjectNode);
}

const ObjectNode& ObjectNode::Find(std::string_view name) const noexcept
{
	for (const auto &object : objects_)
	{
		if (object.Name() == name)
			return object;
	}

	return InvalidObjectNode;
}


ObjectNode& ObjectNode::Search(std::string_view name, SearchStrategy strategy) noexcept
{
	if (strategy == SearchStrategy::BreadthFirst)
	{
		for (auto &node : BreadthFirstSearch())
		{
			if (node.Object.Name() == name)
				return const_cast<ObjectNode&>(node.Object);
		}
	}
	else if (strategy == SearchStrategy::DepthFirst)
	{
		for (auto &node : DepthFirstSearch())
		{
			if (node.Object.Name() == name)
				return const_cast<ObjectNode&>(node.Object);
		}
	}

	return const_cast<ObjectNode&>(InvalidObjectNode);
}

const ObjectNode& ObjectNode::Search(std::string_view name, SearchStrategy strategy) const noexcept
{
	if (strategy == SearchStrategy::BreadthFirst)
	{
		for (auto &node : BreadthFirstSearch())
		{
			if (node.Object.Name() == name)
				return node.Object;
		}
	}
	else if (strategy == SearchStrategy::DepthFirst)
	{
		for (auto &node : DepthFirstSearch())
		{
			if (node.Object.Name() == name)
				return node.Object;
		}
	}

	return InvalidObjectNode;
}


PropertyNode& ObjectNode::Property(std::string_view name) noexcept
{
	for (auto &property : properties_)
	{
		if (property.Name() == name)
			return property;
	}

	return const_cast<PropertyNode&>(InvalidPropertyNode);
}

const PropertyNode& ObjectNode::Property(std::string_view name) const noexcept
{
	for (const auto &property : properties_)
	{
		if (property.Name() == name)
			return property;
	}

	return InvalidPropertyNode;
}


//PropertyNode

PropertyNode::PropertyNode(std::string name, ArgumentNodes arguments) noexcept :
	name_{std::move(name)},
	arguments_{std::move(arguments)}
{
	//Empty
}


/*
	Arguments
*/

ArgumentNode& PropertyNode::Argument(int number) noexcept
{
	return number < NumberOfArguments() ?
		arguments_[number] :
		const_cast<ArgumentNode&>(InvalidArgumentNode);
}

const ArgumentNode& PropertyNode::Argument(int number) const noexcept
{
	return number < NumberOfArguments() ?
		arguments_[number] :
		InvalidArgumentNode;
}


//ArgumentNode

ArgumentNode::ArgumentNode(ArgumentType argument) noexcept :
	argument_{std::move(argument)}
{
	//Empty
}

ArgumentNode::ArgumentNode(ArgumentType argument, std::string unit) noexcept :
	argument_{std::move(argument)}, unit_{std::move(unit)}
{
	//Empty
}

ArgumentNode::ArgumentNode(std::nullopt_t) noexcept
{
	//Empty
}

} //script_tree


//ScriptTree

ScriptTree::ScriptTree(ObjectNodes objects) noexcept :
	objects_{std::move(objects)}
{
	//Empty
}


/*
	Appending
*/

void ScriptTree::Append(const script_tree::ObjectNodes &objects, script_tree::AppendCondition append_condition)
{
	detail::append_nodes(objects_, objects, append_condition);
}

void ScriptTree::Append(const adaptors::ranges::Iterable<script_tree::ObjectNodes&> &objects, script_tree::AppendCondition append_condition)
{
	detail::append_nodes(objects_, objects, append_condition);
}

void ScriptTree::Append(const adaptors::ranges::Iterable<const script_tree::ObjectNodes&> &objects, script_tree::AppendCondition append_condition)
{
	detail::append_nodes(objects_, objects, append_condition);
}


/*
	Finding / searching
*/

script_tree::ObjectNode& ScriptTree::Find(std::string_view name) noexcept
{
	for (auto &object : objects_)
	{
		if (object.Name() == name)
			return object;
	}

	return const_cast<ObjectNode&>(script_tree::InvalidObjectNode);
}

const script_tree::ObjectNode& ScriptTree::Find(std::string_view name) const noexcept
{
	for (const auto &object : objects_)
	{
		if (object.Name() == name)
			return object;
	}

	return script_tree::InvalidObjectNode;
}


script_tree::ObjectNode& ScriptTree::Search(std::string_view name, script_tree::SearchStrategy strategy) noexcept
{
	if (strategy == script_tree::SearchStrategy::BreadthFirst)
	{
		for (auto &node : BreadthFirstSearch())
		{
			if (node.Object.Name() == name)
				return const_cast<ObjectNode&>(node.Object);
		}
	}
	else if (strategy == script_tree::SearchStrategy::DepthFirst)
	{
		for (auto &node : DepthFirstSearch())
		{
			if (node.Object.Name() == name)
				return const_cast<ObjectNode&>(node.Object);
		}
	}

	return const_cast<ObjectNode&>(script_tree::InvalidObjectNode);
}

const script_tree::ObjectNode& ScriptTree::Search(std::string_view name, script_tree::SearchStrategy strategy) const noexcept
{
	if (strategy == script_tree::SearchStrategy::BreadthFirst)
	{
		for (auto &node : BreadthFirstSearch())
		{
			if (node.Object.Name() == name)
				return node.Object;
		}
	}
	else if (strategy == script_tree::SearchStrategy::DepthFirst)
	{
		for (auto &node : DepthFirstSearch())
		{
			if (node.Object.Name() == name)
				return node.Object;
		}
	}

	return script_tree::InvalidObjectNode;
}


/*
	Fully qualified names
*/

std::optional<std::string> ScriptTree::GetFullyQualifiedName(const script_tree::ObjectNode &object) const
{
	auto name = detail::fully_qualified_name(objects_, object);
	return !std::empty(name) ?
		std::make_optional(name) :
		std::nullopt;
}

std::optional<std::string> ScriptTree::GetFullyQualifiedName(const script_tree::ObjectNode &object, const script_tree::PropertyNode &property) const
{
	auto found = false;

	for (auto &object_property : object.Properties())
	{
		if (&object_property == &property)
		{
			found = true;
			break;
		}
	}

	if (found)
	{
		auto name = detail::fully_qualified_name(objects_, object);
		return !std::empty(name) ?
			std::make_optional(name + "." + property.Name()) :
			std::nullopt;
	}
	else
		return {};
}


/*
	Printing
*/

std::string ScriptTree::Print(PrintOptions print_options) const
{
	return detail::print(objects_, print_options);
}


/*
	Serializing
*/

std::optional<ScriptTree> ScriptTree::Deserialize(std::string_view bytes)
{
	auto objects = detail::deserialize(bytes);
	return objects ?
		std::make_optional<ScriptTree>(std::move(*objects)) :
		std::nullopt;
}

std::vector<std::byte> ScriptTree::Serialize() const
{
	return detail::serialize(objects_);
}

} //ion::script