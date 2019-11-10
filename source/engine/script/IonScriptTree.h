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

#include <cassert>
#include <optional>
#include <string>
#include <variant>
#include <type_traits>
#include <vector>

#include "IonScriptTypes.h"
#include "adaptors/ranges/IonIterable.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "types/IonTypes.h"
#include "utilities/IonStringUtility.h"

namespace ion::script
{
	namespace script_tree
	{
		/*
			Forward declaration
			Node types
		*/

		struct TreeNode;
		class ObjectNode;
		class PropertyNode;
		class ArgumentNode;

		using ObjectNodes = std::vector<ObjectNode>;
		using PropertyNodes = std::vector<PropertyNode>;
		using ArgumentNodes = std::vector<ArgumentNode>;

		using ArgumentType =
			std::variant<
				ScriptType::Boolean,
				ScriptType::Color,
				ScriptType::Enumerable,
				ScriptType::FloatingPoint,
				ScriptType::Integer,
				ScriptType::String,
				ScriptType::Vector2>;


		enum class SearchStrategy : bool
		{
			DepthFirst,
			BreadthFirst
		};

		enum class DepthFirstTraversal : bool
		{
			PreOrder,
			PostOrder
		};

		enum class AppendCondition
		{
			Unconditionally,
			NoDuplicateNames,
			NoDuplicateClasses //Same as above for properties
		};

		enum class PrintOptions
		{
			Objects,
			Properties,
			Arguments
		};


		namespace detail
		{
			template <typename ...Ts> struct overloaded : Ts... { using Ts::operator()...; };
			template <typename ...Ts> overloaded(Ts...) -> overloaded<Ts...>;

			template <typename T, typename Ts>
			struct is_contained_in;

			template <typename T, template <typename...> typename U, typename ...Ts>
			struct is_contained_in<T, U<Ts...>> : std::disjunction<std::is_same<T, Ts>...> {};

			template <typename T, typename U>
			constexpr auto is_contained_in_v = is_contained_in<T, U>::value;


			enum class serialization_section_token : std::underlying_type_t<std::byte>
			{	
				Object,
				Property,
				Argument
			};

			using search_result = std::vector<TreeNode>;

			using generation = std::vector<ObjectNode*>;
			using generations = std::vector<generation>;
			using lineage_search_result = std::vector<generations>;


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
			inline void serialize_argument(const T &value, std::vector<std::byte> &bytes)
			{
				static_assert(is_contained_in_v<T, ArgumentType>);

				bytes.push_back(static_cast<std::byte>(serialization_section_token::Argument));
				bytes.push_back(static_cast<std::byte>(variant_index<ArgumentType, T>()));
				serialize_value(value.Get(), bytes);
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

				std::string unit;
				auto unit_bytes_deserialized = bytes_deserialized > 0 ?
					deserialize_value(bytes.substr(bytes_deserialized), unit) : 0;

				if (unit_bytes_deserialized > 0)
					arguments.emplace_back(T{value}, unit);

				return bytes_deserialized + unit_bytes_deserialized;
			}


			void serialize_argument(const ArgumentNode &argument, std::vector<std::byte> &bytes);
			void serialize_property(const PropertyNode &property, std::vector<std::byte> &bytes);
			void serialize_object(const TreeNode &node, std::vector<std::byte> &bytes);
			std::vector<std::byte> serialize(const ObjectNodes &objects);

			int deserialize_argument(std::string_view bytes, ArgumentNodes &arguments);
			int deserialize_property(std::string_view bytes, PropertyNodes &properties, ArgumentNodes &arguments);
			int deserialize_object(std::string_view bytes, std::vector<ObjectNodes> &object_stack, PropertyNodes &properties);
			std::optional<ObjectNodes> deserialize(std::string_view bytes);


			/*
				Appending
			*/

			template <typename DestNodes, typename SrcNodes>
			void append_nodes(DestNodes &destination, const SrcNodes &source, AppendCondition append_condition)
			{
				auto count = std::size(destination); //Count, before appending

				switch (append_condition)
				{
					case AppendCondition::Unconditionally:
					{
						destination.insert(std::end(destination), std::begin(source), std::end(source));
						break;
					}

					case AppendCondition::NoDuplicateClasses:
					{
						if constexpr (std::is_same_v<std::remove_cvref_t<decltype(*std::begin(source))>, ObjectNode>)
						{
							for (auto &candidate : source)
							{
								if (auto iter = std::find_if(std::cbegin(destination), std::cbegin(destination) + count,
									[&](auto &node) noexcept
									{
										return node.Name() == candidate.Name() && node.Classes() == candidate.Classes();
									}); iter == std::cbegin(destination) + count)
									//Candidate is approved
									destination.push_back(candidate);
							}
							break;
						}
						else
							[[fallthrough]];
					}

					case AppendCondition::NoDuplicateNames:
					{
						for (auto &candidate : source)
						{
							if (auto iter = std::find_if(std::cbegin(destination), std::cbegin(destination) + count,
								[&](auto &node) noexcept
								{
									return node.Name() == candidate.Name();
								}); iter == std::cbegin(destination) + count)
								//Candidate is approved
								destination.push_back(candidate);
						}
						break;
					}
				}
			}


			/*
				Printing
			*/

			std::string print(const ObjectNodes &objects, PrintOptions print_options);


			/*
				Searching
			*/

			void breadth_first_search_impl(search_result &result, size_t off);
			void depth_first_search_post_order_impl(search_result &result, const TreeNode &node);
			void depth_first_search_pre_order_impl(search_result &result, const TreeNode &node);
			void fully_qualified_name_impl(std::string &name, const ObjectNode &parent, const ObjectNode &what_object);
			
			search_result breadth_first_search(const ObjectNodes &objects);
			search_result depth_first_search(const ObjectNodes &objects, DepthFirstTraversal traversal);
			std::string fully_qualified_name(const ObjectNodes &objects, const ObjectNode &what_object);

			void lineage_depth_first_search_impl(lineage_search_result &result, generations &descendants, ObjectNode &object);
			lineage_search_result lineage_depth_first_search(ObjectNodes &objects);
		} //detail


		/*
			Node types
		*/

		struct TreeNode final
		{
			const ObjectNode &Object;
			const ObjectNode *const Parent = nullptr;
			const int Depth = 0;

			//Constructs a new tree node containing the given root object
			TreeNode(const ObjectNode &object) noexcept;

			//Constructs a new tree node containing the given child object, parent object and depth
			TreeNode(const ObjectNode &object, const ObjectNode &parent, int depth) noexcept;
		};

		class ObjectNode final
		{
			private:

				std::string name_;
				std::string classes_;
				PropertyNodes properties_;
				ObjectNodes objects_;

			public:

				//Constructs a new object node with the given name and properties
				ObjectNode(std::string name, std::string classes, PropertyNodes properties) noexcept;

				//Constructs a new object node with the given name, properties and child objects
				ObjectNode(std::string name, std::string classes, PropertyNodes properties, ObjectNodes objects) noexcept;


				/*
					Operators
				*/

				//Returns true if this object is valid
				[[nodiscard]] inline operator bool() const noexcept
				{
					return !std::empty(name_);
				}


				/*
					Observers
				*/

				//Returns the name of this object
				[[nodiscard]] inline const auto& Name() const noexcept
				{
					return name_;
				}

				//Returns the classes of this object
				[[nodiscard]] inline const auto& Classes() const noexcept
				{
					return classes_;
				}


				/*
					Appending
				*/

				//Append all of the given objects that satisfy the given append condition
				void Append(const ObjectNodes &objects, AppendCondition append_condition = AppendCondition::Unconditionally);

				//Append all of the given mutable iterable objects that satisfy the given append condition
				void Append(const adaptors::ranges::Iterable<ObjectNodes&> &objects, AppendCondition append_condition = AppendCondition::Unconditionally);

				//Append all of the given immutable iterable objects that satisfy the given append condition
				void Append(const adaptors::ranges::Iterable<const ObjectNodes&> &objects, AppendCondition append_condition = AppendCondition::Unconditionally);


				//Append all of the given properties that satisfy the given append condition
				void Append(const PropertyNodes &properties, AppendCondition append_condition = AppendCondition::Unconditionally);

				//Append all of the given mutable iterable properties that satisfy the given append condition
				void Append(const adaptors::ranges::Iterable<PropertyNodes&> &properties, AppendCondition append_condition = AppendCondition::Unconditionally);

				//Append all of the given immutable iterable properties that satisfy the given append condition
				void Append(const adaptors::ranges::Iterable<const PropertyNodes&> &properties, AppendCondition append_condition = AppendCondition::Unconditionally);


				/*
					Finding / searching
				*/

				//Find an object (top-level child object) by the given name, and return a mutable reference to it
				//Returns InvalidObjectNode if the given object is not found
				[[nodiscard]] ObjectNode& Find(std::string_view name) noexcept;

				//Find an object (top-level child object) by the given name, and return an immutable reference to it
				//Returns InvalidObjectNode if the given object is not found
				[[nodiscard]] const ObjectNode& Find(std::string_view name) const noexcept;


				//Search for an object (all child objects) by the given name and search strategy and return a mutable reference to it
				//Returns InvalidObjectNode if the given object is not found
				[[nodiscard]] ObjectNode& Search(std::string_view name, SearchStrategy strategy = SearchStrategy::BreadthFirst) noexcept;

				//Search for an object (all child objects) by the given name and search strategy and return an immutable reference to it
				//Returns InvalidObjectNode if the given object is not found
				[[nodiscard]] const ObjectNode& Search(std::string_view name, SearchStrategy strategy = SearchStrategy::BreadthFirst) const noexcept;


				//Find a property by the given name and return a mutable reference to it
				//Returns InvalidPropertyNode if the given property is not found
				[[nodiscard]] PropertyNode& Property(std::string_view name) noexcept;

				//Find a property by the given name and return an immutable reference to it
				//Returns InvalidPropertyNode if the given property is not found
				[[nodiscard]] const PropertyNode& Property(std::string_view name) const noexcept;


				/*
					Ranges
				*/

				//Returns an immutable (BFS) range of all child objects in this object
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto BreadthFirstSearch() const
				{
					return adaptors::ranges::Iterable<const detail::search_result>{detail::breadth_first_search(objects_)};
				}

				//Returns an immutable (DFS) range of all child objects in this object
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto DepthFirstSearch(DepthFirstTraversal traversal = DepthFirstTraversal::PreOrder) const
				{
					return adaptors::ranges::Iterable<const detail::search_result>{detail::depth_first_search(objects_, traversal)};
				}


				//Returns a mutable range of all top-level child objects in this object
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Objects() noexcept
				{
					return adaptors::ranges::Iterable<ObjectNodes&>{objects_};
				}

				//Returns an immutable range of all top-level child objects in this object
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline const auto Objects() const noexcept
				{
					return adaptors::ranges::Iterable<const ObjectNodes&>{objects_};
				}

				//Returns a mutable range of all properties in this object
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Properties() noexcept
				{
					return adaptors::ranges::Iterable<PropertyNodes&>{properties_};
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
					Operators
				*/

				//Returns true if this property is valid
				[[nodiscard]] inline operator bool() const noexcept
				{
					return !std::empty(name_);
				}


				/*
					Observers
				*/

				//Returns the name of the property
				[[nodiscard]] inline const auto& Name() const noexcept
				{
					return name_;
				}


				/*
					Arguments
				*/

				//Returns a mutable argument at the given argument number
				[[nodiscard]] ArgumentNode& Argument(int number) noexcept;

				//Returns an immutable argument at the given argument number
				[[nodiscard]] const ArgumentNode& Argument(int number) const noexcept;


				//Returns a mutable argument at the given argument number
				[[nodiscard]] inline auto& operator[](int number) noexcept
				{
					return Argument(number);
				}

				//Returns an immutable argument at the given argument number
				[[nodiscard]] inline const auto& operator[](int number) const noexcept
				{
					return Argument(number);
				}


				//Return the number of arguments in this property
				[[nodiscard]] inline auto NumberOfArguments() const noexcept
				{
					return static_cast<int>(std::size(arguments_));
				}


				/*
					Ranges
				*/

				//Returns a mutable range of all arguments in this property
				//This can be used directly with a range-based for loop
				[[nodiscard]] inline auto Arguments() noexcept
				{
					return adaptors::ranges::Iterable<ArgumentNodes&>{arguments_};
				}

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

				std::optional<ArgumentType> argument_;
				std::string unit_;
			
			public:

				//Constructs a new argument node with the given argument
				ArgumentNode(ArgumentType argument) noexcept;

				//Constructs a new argument node with the given argument and unit
				ArgumentNode(ArgumentType argument, std::string unit) noexcept;

				//Construct an invalid argument node
				explicit ArgumentNode(std::nullopt_t) noexcept;


				/*
					Operators
				*/

				//Returns true if this argument is valid
				[[nodiscard]] inline operator bool() const noexcept
				{
					return argument_.has_value();
				}


				/*
					Observers
				*/

				//Returns the value of the argument with the given argument type
				template <typename T>
				[[nodiscard]] inline auto Get() const noexcept
				{
					auto value = *this ? std::get_if<T>(&*argument_) : nullptr;

					if constexpr (std::is_same_v<T, ScriptType::FloatingPoint>)
					{		
						if (!value)
						{
							//Try to get as integer
							if (auto val = *this ? std::get_if<ScriptType::Integer>(&*argument_) : nullptr; val)
								//Okay, non-narrowing
								return std::make_optional(ScriptType::FloatingPoint{val->As<ScriptType::FloatingPoint::value_type>()});
						}
					}

					return value ? std::make_optional(*value) : std::nullopt;
				}

				//Call the correct overload for the given overload set, based on the value of the argument
				template <typename T, typename ...Ts>
				inline auto Visit(T &&callable, Ts &&...callables) const noexcept
				{
					assert(*this);
					return std::visit(detail::overloaded{std::forward<T>(callable), std::forward<Ts>(callables)...}, *argument_);
				}

				//Returns the unit of this argument
				[[nodiscard]] inline const auto& Unit() const noexcept
				{
					return unit_;
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
				Appending
			*/

			//Append all of the given objects that satisfy the given append condition
			void Append(const script_tree::ObjectNodes &objects, script_tree::AppendCondition append_condition = script_tree::AppendCondition::Unconditionally);

			//Append all of the given mutable iterable objects that satisfy the given append condition
			void Append(const adaptors::ranges::Iterable<script_tree::ObjectNodes&> &objects, script_tree::AppendCondition append_condition = script_tree::AppendCondition::Unconditionally);

			//Append all of the given immutable iterable objects that satisfy the given append condition
			void Append(const adaptors::ranges::Iterable<const script_tree::ObjectNodes&> &objects, script_tree::AppendCondition append_condition = script_tree::AppendCondition::Unconditionally);


			/*
				Finding / searching
			*/

			//Find an object (top-level object) by the given name, and return a mutable reference to it
			//Returns InvalidObjectNode if the given object is not found
			[[nodiscard]] script_tree::ObjectNode& Find(std::string_view name) noexcept;

			//Find an object (top-level object) by the given name, and return an immutable reference to it
			//Returns InvalidObjectNode if the given object is not found
			[[nodiscard]] const script_tree::ObjectNode& Find(std::string_view name) const noexcept;


			//Search for an object (all objects) by the given name and search strategy and return a mutable reference to it
			//Returns InvalidObjectNode if the given object is not found
			[[nodiscard]] script_tree::ObjectNode& Search(std::string_view name, script_tree::SearchStrategy strategy = script_tree::SearchStrategy::BreadthFirst) noexcept;

			//Search for an object (all objects) by the given name and search strategy and return an immutable reference to it
			//Returns InvalidObjectNode if the given object is not found
			[[nodiscard]] const script_tree::ObjectNode& Search(std::string_view name, script_tree::SearchStrategy strategy = script_tree::SearchStrategy::BreadthFirst) const noexcept;


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
			[[nodiscard]] std::string Print(script_tree::PrintOptions print_options = script_tree::PrintOptions::Arguments) const;


			/*
				Ranges
			*/

			//Returns an immutable (BFS) range of all objects in this script tree
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto BreadthFirstSearch() const
			{
				return adaptors::ranges::Iterable<const script_tree::detail::search_result>{script_tree::detail::breadth_first_search(objects_)};
			}

			//Returns an immutable (DFS) range of all objects in this script tree
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto DepthFirstSearch(script_tree::DepthFirstTraversal traversal = script_tree::DepthFirstTraversal::PreOrder) const
			{
				return adaptors::ranges::Iterable<const script_tree::detail::search_result>{script_tree::detail::depth_first_search(objects_, traversal)};
			}


			//Returns a mutable range of all top-level objects in this script tree
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Objects() noexcept
			{
				return adaptors::ranges::Iterable<script_tree::ObjectNodes>{objects_};
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

	namespace script_tree
	{
		/*
			Predefined node constants
			For fluent interface design
		*/

		inline const auto InvalidObjectNode = ObjectNode{"", "", {}};
		inline const auto InvalidPropertyNode = PropertyNode{"", {}};
		inline const auto InvalidArgumentNode = ArgumentNode{std::nullopt};
	} //script_tree

} //ion::script

#endif