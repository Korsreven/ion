/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script
File:	IonScriptTree.h
-------------------------------------------
*/

#ifndef _ION_SCRIPT_TREE_
#define _ION_SCRIPT_TREE_

#include <optional>
#include <string>
#include <variant>
#include <type_traits>
#include <vector>

#include "adaptors/ranges/IonIterable.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "types/IonTypes.h"
#include "utilities/IonStringUtility.h"

namespace ion::script
{
	namespace script_tree
	{
		using graphics::utilities::Color;
		using graphics::utilities::Vector2;

		class ObjectNode;
		class PropertyNode;
		class ArgumentNode;

		using ObjectNodes = std::vector<ObjectNode>;
		using PropertyNodes = std::vector<PropertyNode>;
		using ArgumentNodes = std::vector<ArgumentNode>;

		struct BooleanArgument;
		struct ColorArgument;
		struct EnumerableArgument;
		struct FloatingPointArgument;
		struct IntegerArgument;
		struct StringArgument;
		struct Vector2Argument;

		using ArgumentType =
			std::variant<
				BooleanArgument,
				ColorArgument,
				EnumerableArgument,
				FloatingPointArgument,
				IntegerArgument,
				StringArgument,
				Vector2Argument>;

		enum class DepthFirstSearchTraversal
		{
			PreOrder,
			PostOrder
		};

		enum class PrintOptions
		{
			Objects,
			ObjectsWithProperties,
			ObjectsWithPropertiesAndArguments
		};


		namespace detail
		{
			template <typename ...Ts> struct overloaded : Ts... { using Ts::operator()...; };
			template <typename ...Ts> overloaded(Ts...) -> overloaded<Ts...>;

			enum class serialization_section_token : std::underlying_type_t<std::byte>
			{	
				Object,
				Property,
				Argument
			};


			struct tree_node
			{
				const ObjectNode &Object;
				const ObjectNode *const Parent = nullptr;
				int Depth = 0;

				tree_node(const ObjectNode &object);
				tree_node(const ObjectNode &object, const ObjectNode &parent, int depth);
			};

			using search_result = std::vector<tree_node>;


			template <typename T>
			class argument
			{
				private:

					T value_{};

				public:

					using value_type = T;

					explicit argument(T value) noexcept :
						value_{std::move(value)}
					{
						//Empty
					}

					//Returns an immutable reference to the value of this argument
					[[nodiscard]] inline const auto& Value() const noexcept
					{
						return value_;
					}
			};

			template <typename T>
			struct arithmetic_argument : argument<T>
			{
				static_assert(std::is_arithmetic_v<T>);
				using argument<T>::argument;

				//Returns the value of this argument casted to another arithmetic type
				template <typename U>		
				[[nodiscard]] inline auto As() const noexcept
				{
					static_assert(std::is_arithmetic_v<U>);
					return static_cast<U>(argument<T>::Value());
				}
			};


			/*
				Serializing
			*/

			template <typename VariantType, typename T, size_t Index = 0>
			constexpr std::size_t variant_index() noexcept
			{
				if constexpr (Index == std::variant_size_v<VariantType>)
					static_assert(false);
				else if constexpr (std::is_same_v<std::variant_alternative_t<Index, VariantType>, T>)
					return Index;
				else
					return variant_index<VariantType, T, Index + 1>();
			}


			template <typename T>
			inline void serialize_value(const T &value, std::vector<std::byte> &bytes)
			{
				if constexpr (std::is_same_v<T, std::string>)
				{
					auto data_size = utilities::string::Serialize(std::size(value));
					auto data = reinterpret_cast<const std::byte*>(std::data(value));
					bytes.insert(std::end(bytes), std::begin(data_size), std::end(data_size));
					bytes.insert(std::end(bytes), data, data + std::size(value));
				}
				else
				{
					auto data = utilities::string::Serialize(value);
					bytes.insert(std::end(bytes), std::begin(data), std::end(data));
				}
			}
			
			template <typename T>
			inline void serialize_argument(const T &arg, std::vector<std::byte> &bytes)
			{
				static_assert(std::is_base_of_v<argument<typename T::value_type>, T>);

				bytes.push_back(static_cast<std::byte>(serialization_section_token::Argument));
				bytes.push_back(static_cast<std::byte>(variant_index<ArgumentType, T>()));
				serialize_value(arg.Value(), bytes);
			}


			template <typename T>
			inline auto deserialize_value(std::string_view bytes, T &value)
			{
				if constexpr (std::is_same_v<T, std::string>)
				{
					if (std::size(bytes) >= sizeof(size_t))
					{
						std::array<std::byte, sizeof(size_t)> output;
						std::copy(std::begin(bytes), std::begin(bytes) + std::size(output), reinterpret_cast<char*>(std::data(output)));
						
						size_t data_size;
						utilities::string::Deserialize(output, data_size);

						if (std::size(bytes) >= std::size(output) + data_size)
						{
							value = T{bytes.substr(std::size(output), data_size)};
							return static_cast<int>(std::size(output) + data_size);
						}
					}
				}
				else
				{
					if (std::size(bytes) >= sizeof(T))
					{		
						std::array<std::byte, sizeof(T)> output;
						std::copy(std::begin(bytes), std::begin(bytes) + std::size(output), reinterpret_cast<char*>(std::data(output)));
						utilities::string::Deserialize(output, value);
						return static_cast<int>(std::size(output));
					}
				}

				return 0;
			}

			template <typename T>
			inline auto deserialize_argument(std::string_view bytes, ArgumentNodes &arguments)
			{
				typename T::value_type value;
				auto bytes_deserialized = deserialize_value(bytes, value);

				if (bytes_deserialized > 0)
					arguments.emplace_back(T{value});

				return bytes_deserialized;
			}


			void serialize_argument(const ArgumentNode &argument, std::vector<std::byte> &bytes);
			void serialize_property(const PropertyNode &property, std::vector<std::byte> &bytes);
			void serialize_object(const tree_node &node, std::vector<std::byte> &bytes);
			std::vector<std::byte> serialize(const ObjectNodes &objects);

			int deserialize_argument(std::string_view bytes, ArgumentNodes &arguments);
			int deserialize_property(std::string_view bytes, PropertyNodes &properties, ArgumentNodes &arguments);
			int deserialize_object(std::string_view bytes, std::vector<ObjectNodes> &object_stack, PropertyNodes &properties);
			std::optional<ObjectNodes> deserialize(std::string_view bytes);


			/*
				Printing
			*/

			std::string print(const ObjectNodes &objects, PrintOptions print_options);


			/*
				Searching
			*/

			void breadth_first_search_impl(search_result &result, size_t off);
			void depth_first_search_post_order_impl(search_result &result, const tree_node &node);
			void depth_first_search_pre_order_impl(search_result &result, const tree_node &node);
			void fully_qualified_name_impl(std::string &name, const ObjectNode &parent, const ObjectNode &what_object);
			
			search_result breadth_first_search(const ObjectNodes &objects);
			search_result depth_first_search(const ObjectNodes &objects, DepthFirstSearchTraversal traversal);
			std::string fully_qualified_name(const ObjectNodes &objects, const ObjectNode &what_object);
		} //detail


		struct BooleanArgument final : detail::arithmetic_argument<bool>
		{
			using arithmetic_argument::arithmetic_argument;
		};

		struct ColorArgument final : detail::argument<Color>
		{
			using argument::argument;
		};

		struct EnumerableArgument final : detail::argument<std::string>
		{
			using argument::argument;
		};

		struct FloatingPointArgument final : detail::arithmetic_argument<float80>
		{
			using arithmetic_argument::arithmetic_argument;
		};

		struct IntegerArgument final : detail::arithmetic_argument<int64>
		{
			using arithmetic_argument::arithmetic_argument;
		};

		struct StringArgument final : detail::argument<std::string>
		{
			using argument::argument;
		};

		struct Vector2Argument final : detail::argument<Vector2>
		{
			using argument::argument;
		};

		class ObjectNode final
		{
			private:

				std::string name_;
				PropertyNodes properties_;
				ObjectNodes objects_;

			public:

				//Constructs a new object node with the given name and properties
				ObjectNode(std::string name, PropertyNodes properties) noexcept;

				//Constructs a new object node with the given name, properties and child objects
				ObjectNode(std::string name, PropertyNodes properties, ObjectNodes objects) noexcept;


				/*
					Observers
				*/

				//Returns the name of this object
				[[nodiscard]] inline const auto& Name() const noexcept
				{
					return name_;
				}


				/*
					Ranges
				*/

				//Returns an immutable (BFS) range of all child objects in this object
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto BreadthFirst() const
				{
					return adaptors::ranges::Iterable<const script_tree::detail::search_result>{script_tree::detail::breadth_first_search(objects_)};
				}

				//Returns an immutable (DFS) range of all child objects in this object
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto DepthFirst(DepthFirstSearchTraversal traversal = DepthFirstSearchTraversal::PreOrder) const
				{
					return adaptors::ranges::Iterable<const script_tree::detail::search_result>{script_tree::detail::depth_first_search(objects_, traversal)};
				}


				//Returns an immutable range of all top-level child objects in this object
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto Objects() const noexcept
				{
					return adaptors::ranges::Iterable<const ObjectNodes&>{objects_};
				}

				//Returns an immutable range of all properties in this object
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto Properties() const noexcept
				{
					return adaptors::ranges::Iterable<const PropertyNodes&>{properties_};
				}
		};

		class PropertyNode final
		{
			private:

				std::string name_;
				ArgumentNodes arguments_;

			public:

				//Constructs a new property node with the given name and arguments
				PropertyNode(std::string name, ArgumentNodes arguments) noexcept;


				/*
					Observers
				*/

				//Returns the name of the property
				[[nodiscard]] inline const auto& Name() const noexcept
				{
					return name_;
				}


				/*
					Ranges
				*/

				//Returns an immutable range of all arguments in this property
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto Arguments() const noexcept
				{
					return adaptors::ranges::Iterable<const ArgumentNodes&>{arguments_};
				}
		};

		class ArgumentNode final
		{
			private:

				ArgumentType argument_;
			
			public:

				//Constructs a new argument node with the given argument
				ArgumentNode(ArgumentType argument) noexcept;


				/*
					Observers
				*/

				//Returns the value of the argument with the given argument type
				template <typename T>
				[[nodiscard]] inline auto Get() const noexcept
				{
					auto value = std::get_if<T>(&argument_);
					return value ? std::make_optional(*value) : std::nullopt;
				}

				//Call the correct overload for the given overload set, based on the value of the argument
				template <typename T, typename ...Ts>
				inline auto Visit(T &&callable, Ts &&...callables) const noexcept
				{
					return std::visit(detail::overloaded{std::forward<T>(callable), std::forward<Ts>(callables)...}, argument_);
				}
		};
	} //script_tree


	class ScriptTree final
	{
		private:

			script_tree::ObjectNodes objects_;

		public:

			ScriptTree() = default;

			//Constructs a new script tree with the given objects
			explicit ScriptTree(script_tree::ObjectNodes objects) noexcept;


			/*
				Fully qualified names
			*/

			//Returns the fully qualified name to the given object
			//Returns nullopt if the given object is not found in this tree
			[[nodiscard]] std::optional<std::string> GetFullyQualifiedName(const script_tree::ObjectNode &object) const;

			//Returns the fully qualified name to the given property
			//Returns nullopt if the given object or property is not found in this tree
			[[nodiscard]] std::optional<std::string> GetFullyQualifiedName(const script_tree::ObjectNode &object, const script_tree::PropertyNode &property) const;


			/*
				Printing
			*/

			//Print out this script tree as an hierarchical list with objects, properties and arguments.
			//Choose what to print based on the given print options (optionally)
			[[nodiscard]] std::string Print(script_tree::PrintOptions print_options = script_tree::PrintOptions::ObjectsWithProperties) const;


			/*
				Ranges
			*/

			//Returns an immutable (BFS) range of all objects in this script tree
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto BreadthFirst() const
			{
				return adaptors::ranges::Iterable<const script_tree::detail::search_result>{script_tree::detail::breadth_first_search(objects_)};
			}

			//Returns an immutable (DFS) range of all objects in this script tree
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto DepthFirst(script_tree::DepthFirstSearchTraversal traversal = script_tree::DepthFirstSearchTraversal::PreOrder) const
			{
				return adaptors::ranges::Iterable<const script_tree::detail::search_result>{script_tree::detail::depth_first_search(objects_, traversal)};
			}


			//Returns an immutable range of all top-level objects in this script tree
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Objects() const noexcept
			{
				return adaptors::ranges::Iterable<const script_tree::ObjectNodes&>{objects_};
			}


			/*
				Serializing
			*/

			//Deserialize a given byte array to a script tree
			[[nodiscard]] static std::optional<ScriptTree> Deserialize(std::string_view bytes);

			//Serialize this script tree to a byte array
			[[nodiscard]] std::vector<std::byte> Serialize() const;
	};
} //ion::script

#endif