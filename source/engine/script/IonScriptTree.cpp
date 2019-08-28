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

namespace ion::script
{

using namespace script_tree;

namespace script_tree
{

namespace detail
{

using namespace std::string_literals;
using namespace types::type_literals;

tree_node::tree_node(const ObjectNode &object) :
	Object{object}
{
	//Empty
}

tree_node::tree_node(const ObjectNode &object, const ObjectNode &parent, int depth) :
	Object{object},
	Parent{&parent},
	Depth{depth}
{
	//Empty
}


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
}

void serialize_property(const PropertyNode &property, std::vector<std::byte> &bytes)
{
	for (auto &argument : property.Arguments())
		serialize_argument(argument, bytes);

	auto name_size = utilities::string::Serialize(std::size(property.Name()));
	bytes.push_back(static_cast<std::byte>(serialization_section_token::Property));
	serialize_value(property.Name(), bytes);
}

void serialize_object(const tree_node &node, std::vector<std::byte> &bytes)
{
	for (auto &property : node.Object.Properties())
		serialize_property(property, bytes);

	auto name_size = utilities::string::Serialize(std::size(node.Object.Name()));
	bytes.push_back(static_cast<std::byte>(serialization_section_token::Object));
	serialize_value(node.Depth, bytes);
	serialize_value(node.Object.Name(), bytes);
}

std::vector<std::byte> serialize(const ObjectNodes &objects)
{
	std::vector<std::byte> bytes;

	for (auto &node : depth_first_search(objects, DepthFirstSearchTraversal::PostOrder))
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

	//Node depth serialized
	if (depth_bytes_deserialized > 0)
	{
		std::string name;
		auto name_bytes_deserialized = deserialize_value(bytes.substr(depth_bytes_deserialized), name);

		//Object name serialized
		if (name_bytes_deserialized > 0)
		{
			if (depth >= static_cast<int>(std::size(object_stack)))
				object_stack.insert(std::end(object_stack), depth - std::size(object_stack) + 1, {});

			//Leaf object, no childrens
			if (auto &object = object_stack[depth];
				&object == &object_stack.back())
				object.emplace_back(std::move(name), std::move(properties));
			//Take childrens (if any) from one depth deeper
			else
				object.emplace_back(std::move(name), std::move(properties),
									std::move(object_stack[depth + 1]));

			return depth_bytes_deserialized + name_bytes_deserialized;
		}
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
	for (auto &[object, parent, depth] : depth_first_search(objects, DepthFirstSearchTraversal::PreOrder))
	{
		output += "\n" + std::string(depth * 4, ' ') + "[-] " + object.Name();

		if (print_options == PrintOptions::ObjectsWithProperties ||
			print_options == PrintOptions::ObjectsWithPropertiesAndArguments)
		{
			//Print properties
			for (auto &property : object.Properties())
			{
				output += "\n" + std::string(depth * 4 + 1, ' ') + "|-- " + property.Name();

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
									return arg.Value() ? "true"s : "false"s;
								},
								[](const ColorArgument &arg)
								{
									auto [r, g, b] = arg.Value().ToRGB();
									auto a = arg.Value().A();
									return "(" + utilities::convert::ToString(r) + ", " + utilities::convert::ToString(g) + ", " + utilities::convert::ToString(b) +
										(a < 1.0_r ? ", " + utilities::convert::ToString(a, 2) : "") + ")";
								},
								[](const EnumerableArgument &arg)
								{
									return arg.Value();
								},
								[](const StringArgument &arg)
								{
									return "\"" + arg.Value() + "\"";
								},
								[](const Vector2Argument &arg)
								{
									auto [x, y] = arg.Value().XY();
									return "{" + utilities::convert::ToString(x) + ", " + utilities::convert::ToString(y) + "}";
								},
								//Default
								[](auto &&arg)
								{
									return utilities::convert::ToString(arg.Value());
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

void depth_first_search_post_order_impl(search_result &result, const tree_node &node)
{
	for (auto &object : node.Object.Objects())
		depth_first_search_post_order_impl(result, {object, node.Object, node.Depth + 1});

	result.push_back(node);
}

void depth_first_search_pre_order_impl(search_result &result, const tree_node &node)
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

search_result depth_first_search(const ObjectNodes &objects, DepthFirstSearchTraversal traversal)
{
	search_result result;

	//Post-order
	if (traversal == DepthFirstSearchTraversal::PostOrder)
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

} //detail


ObjectNode::ObjectNode(std::string name, PropertyNodes properties) noexcept :
	ObjectNode(std::move(name), std::move(properties), {})
{
	//Empty
}

ObjectNode::ObjectNode(std::string name, PropertyNodes properties, ObjectNodes objects) noexcept :
	name_{std::move(name)},
	properties_{std::move(properties)},
	objects_{std::move(objects)}
{
	//Empty
}

PropertyNode::PropertyNode(std::string name, ArgumentNodes arguments) noexcept :
	name_{std::move(name)},
	arguments_{std::move(arguments)}
{
	//Empty
}

ArgumentNode::ArgumentNode(ArgumentType argument) noexcept :
	argument_{std::move(argument)}
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