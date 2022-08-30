/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	adaptors
File:	IonFlatAssociativeAdaptor.h
-------------------------------------------
*/

#ifndef ION_FLAT_ASSOCIATIVE_ADAPTOR_H
#define ION_FLAT_ASSOCIATIVE_ADAPTOR_H

#include <algorithm>
#include <type_traits>
#include <utility>

#include "ranges/IonIterable.h"
#include "types/IonTypeTraits.h"

namespace ion::adaptors
{
	namespace flat_associative_adaptor::detail
	{
		template <typename KeyType, typename ValueType, typename Compare>
		struct value_compare
		{
			Compare compare;

			template <typename T1, typename T2>
			constexpr auto operator()(const T1 &x, const T2 &y) const noexcept
			{
				if constexpr (std::is_same_v<KeyType, ValueType>) //Set
				{
					return compare(x, y);
				}
				else //Map
				{
					if constexpr (std::is_same_v<ValueType, T1> && std::is_same_v<ValueType, T2>)
						return compare(x.first, y.first);
					else if constexpr (std::is_same_v<ValueType, T1>)
						return compare(x.first, y);
					else if constexpr (std::is_same_v<ValueType, T2>)
						return compare(x, y.first);
					else
						return compare(x, y);
				}
			}
		};

		template <typename KeyType, typename ValueType>
		auto& extract_key(const ValueType &value) noexcept
		{
			if constexpr (std::is_same_v<KeyType, ValueType>)
				return value; //Set
			else
				return value.first; //Map
		}


		template <typename Iterator, typename T, typename Compare>
		inline auto lower_bound(Iterator first, Iterator last, const T &value, Compare compare) noexcept
		{
			return std::lower_bound(first, last, value, compare);
		}

		template <typename Iterator, typename T, typename Compare>
		inline auto upper_bound(Iterator first, Iterator last, const T &value, Compare compare) noexcept
		{
			return std::upper_bound(first, last, value, compare);
		}

		template <typename Iterator, typename T, typename Compare>
		inline auto find(Iterator first, Iterator last, const T &value, Compare compare) noexcept
		{
			first = std::lower_bound(first, last, value, compare);
			return std::pair{first, first != last && !compare(value, *first)};
		}

		template <typename Iterator, typename T, typename Compare>
		inline auto find_with_hint(Iterator hint, Iterator first, Iterator last, const T &value, Compare compare) noexcept
		{
			first =
				[&]() noexcept
				{
					//No range
					if (first == last)
						return first;

					//First
					if (hint == first)
					{
						//Value is not greater than hint
						if (!compare(*hint, value))
							return hint;
					}

					//Last
					else if (hint == last)
					{
						//Value is not less than hint - 1
						if (!compare(value, *(hint - 1)))
							return hint;
					}

					//Middle
					//Value is not greater than hint
					//Value is not less than hint - 1
					else
					{
						auto left_of = !compare(*hint, value);
						auto right_of = !compare(value, *(hint - 1));

						if (left_of && right_of)
							return hint;
						else if (left_of)
							return std::lower_bound(first, hint, value, compare);
						else if (right_of)
							return std::lower_bound(hint, last, value, compare);
					}

					return std::lower_bound(first, last, value, compare);
				}();

			return std::pair{first, first != last && !compare(value, *first)};
		}


		template <typename Iterator, typename Compare>
		inline void sort_keys(Iterator first, Iterator last, Compare compare) noexcept
		{
			std::sort(first, last, compare);
		}

		template <typename Container, typename Iterator, typename Compare>
		inline void erase_duplicate_keys(Container &container, Iterator first, Iterator last, Compare compare) noexcept
		{
			//Container must be sorted!
			container.erase(
				std::unique(first, last,
					[&](const auto &x, const auto &y) noexcept
					{
						return !compare(x, y) && !compare(y, x);
					}), std::end(container));			
		}

		template <typename Container, typename Compare>
		inline Container make_ordered_associative(Container container, Compare compare) noexcept
		{
			sort_keys(std::begin(container), std::end(container), compare);
			erase_duplicate_keys(container, std::begin(container), std::end(container), compare);
			return container;
		}

		template <typename Container, typename Compare>
		inline void merge_added(Container &container, typename Container::iterator first, Compare compare)
		{
			//Sort added keys
			sort_keys(first, std::end(container), compare);

			//Container contains previously added entries
			if (first != std::begin(container))
			{
				//Merge newly added entries with previous added entries
				std::inplace_merge(
					std::begin(container), first,
					std::end(container), compare);
			}

			//Make sure there are no duplicate keys
			erase_duplicate_keys(container, std::begin(container), std::end(container), compare);
		}
	} //flat_associative_adaptor::detail


	//An adaptor class that provides associative functionality to any flat container
	template <typename Container, typename Key, typename Compare,
		template <typename...> typename IteratorAdaptor = types::identity>
	struct FlatAssociativeAdaptor : ranges::BasicIterable<Container, IteratorAdaptor>
	{
		using my_base = ranges::BasicIterable<Container, IteratorAdaptor>;

		static_assert(std::is_base_of_v<std::random_access_iterator_tag,
					  typename std::iterator_traits<typename my_base::container_type::iterator>::iterator_category>);

		using key_type = Key;	
		using key_compare = Compare;
		using value_compare = flat_associative_adaptor::detail::value_compare<Key, typename my_base::value_type, Compare>;


		//Default constructor
		FlatAssociativeAdaptor() = default;

		//Construct a flat associative adaptor by the given container
		explicit FlatAssociativeAdaptor(typename my_base::container_type container) noexcept :
			my_base{flat_associative_adaptor::detail::make_ordered_associative(std::move(container), value_compare{})}
		{
			//Empty
		}

		//Construct a flat associative adaptor by the given initializer list
		explicit FlatAssociativeAdaptor(std::initializer_list<typename my_base::value_type> list) noexcept :
			my_base{flat_associative_adaptor::detail::make_ordered_associative(typename my_base::container_type{list}, value_compare{})}
		{
			//Empty
		}


		/*
			Ranges
		*/

		//Returns a mutable range of all elements in the underlying container
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto Elements() noexcept
		{
			return ranges::BasicIterable<typename my_base::container_type&, IteratorAdaptor>{this->container_};
		}

		//Returns an immutable range of all elements in the underlying container
		//This can be used directly with a range-based for loop
		[[nodiscard]] inline auto Elements() const noexcept
		{
			return ranges::BasicIterable<const typename my_base::container_type&, IteratorAdaptor>{this->container_};
		}


		/*
			Modifiers
		*/

		//Reserves more capacity by calling reserve on the underlying container
		//Reserve does not exist for std::deque
		inline void reserve(size_t new_capacity)
		{
			return this->container_.reserve(new_capacity);
		}

		//Skrinks the capacity of the underlying container to fit
		inline void shrink_to_fit()
		{
			return this->container_.shrink_to_fit();
		}

		//Swap this underlying container with the given adaptors underlying container
		inline void swap(FlatAssociativeAdaptor &flat_associative_adaptor)
		{
			return this->container_.swap(flat_associative_adaptor.container_);
		}


		/*
			Observers
		*/

		//Returns the capacity of the underlying container
		//Capacity does not exist for std::deque
		[[nodiscard]] inline auto capacity() const noexcept
		{
			return this->container_.capacity();
		}

		//Returns the function object that compares the keys
		[[nodiscard]] inline auto key_comp() const noexcept
		{
			return key_compare{};
		}

		//Returns the function object that compares objects of type value_type
		[[nodiscard]] inline auto value_comp() const noexcept
		{
			return value_compare{};
		}


		/*
			Lookup
		*/

		//Returns true if the given key exists
		[[nodiscard]] inline auto contains(const key_type &key) const noexcept
		{
			return contains<key_type>(key);
		}

		//Returns true if the given key exists
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, key_type> || types::is_transparent_comparator_v<Compare>>>
		[[nodiscard]] inline auto contains(const T &key) const noexcept
		{
			return find(key) != std::end(this->container_);
		}


		//Returns the count of all keys that compare equal with the given key
		//Since keys are unique, this function returns values in range [0, 1]
		[[nodiscard]] inline auto count(const key_type &key) const noexcept
		{
			return count<key_type>(key);
		}

		//Returns the count of all keys that compare equal with the given key
		//Since keys are unique, this function returns values in range [0, 1]
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, key_type> || types::is_transparent_comparator_v<Compare>>>
		[[nodiscard]] inline auto count(const T &key) const noexcept
		{
			auto [iter, found] =
				flat_associative_adaptor::detail::find(
					std::begin(this->container_), std::end(this->container_),
					key, value_compare{});
			auto key_count = 0;

			if (found)
			{
				++key_count;

				for (; ++iter != std::end(this->container_);)
				{
					if (!value_compare{}(*iter, key) && !value_compare{}(key, *iter))
						++key_count;
					else
						break;
				}
			}

			return key_count;
		}


		//Returns an iterator to the element with the given key
		//If key does not exist, the end iterator is returned
		[[nodiscard]] inline auto find(const key_type &key) noexcept
		{
			return find<key_type>(key);
		}

		//Returns an iterator to the element with the given key
		//If key does not exist, the end iterator is returned
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, key_type> || types::is_transparent_comparator_v<Compare>>>
		[[nodiscard]] inline auto find(const T &key) noexcept
		{
			auto [iter, found] =
				flat_associative_adaptor::detail::find(
					std::begin(this->container_), std::end(this->container_),
					key, value_compare{});

			return found ?
				typename my_base::iterator{iter} :
				this->end();
		}

		//Returns an const iterator to the element with the given key
		//If key does not exist, the end iterator is returned
		[[nodiscard]] inline auto find(const key_type &key) const noexcept
		{
			return find<key_type>(key);
		}

		//Returns an const iterator to the element with the given key
		//If key does not exist, the end iterator is returned
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, key_type> || types::is_transparent_comparator_v<Compare>>>
		[[nodiscard]] inline auto find(const T &key) const noexcept
		{
			auto [iter, found] =
				flat_associative_adaptor::detail::find(
					std::begin(this->container_), std::end(this->container_),
					key, value_compare{});

			return found ?
				typename my_base::const_iterator{iter} :
				this->end();
		}


		/*
			Lower/upper bound
		*/

		//Returns a pair of iterators [first, last), of all elements that compare equal to the given key
		[[nodiscard]] inline auto equal_range(const key_type &key) noexcept
		{
			return equal_range<key_type>(key);
		}

		//Returns a pair of iterators [first, last), of all elements that compare equal to the given key
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, key_type> || types::is_transparent_comparator_v<Compare>>>
		[[nodiscard]] inline auto equal_range(const T &key) noexcept
		{
			auto first = find(key);

			if (first != std::end(this->container_))
				return std::pair{
					typename my_base::iterator{first},
					typename my_base::iterator{upper_bound(key)}};
			else
				return std::pair{
					typename my_base::iterator{first},
					typename my_base::iterator{std::end(this->container_)}};
		}

		//Returns a pair of const iterators [first, last), of all elements that compare equal to the given key
		[[nodiscard]] inline auto equal_range(const key_type &key) const noexcept
		{
			return equal_range<key_type>(key);
		}

		//Returns a pair of const iterators [first, last), of all elements that compare equal to the given key
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, key_type> || types::is_transparent_comparator_v<Compare>>>
		[[nodiscard]] inline auto equal_range(const T &key) const noexcept
		{
			auto first = find(key);

			if (first != std::end(this->container_))
				return std::pair{
					typename my_base::const_iterator{first},
					typename my_base::const_iterator{upper_bound(key)}};
			else
				return std::pair{
					typename my_base::const_iterator{first},
					typename my_base::const_iterator{std::end(this->container_)}};
		}


		//Returns the lower bound of the given key
		//An iterator to the first element that is greater or equal
		[[nodiscard]] inline auto lower_bound(const key_type &key) noexcept
		{
			return lower_bound<key_type>(key);
		}

		//Returns the lower bound of the given key
		//An iterator to the first element that is greater or equal
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, key_type> || types::is_transparent_comparator_v<Compare>>>
		[[nodiscard]] inline auto lower_bound(const T &key) noexcept
		{
			return typename my_base::iterator{
				flat_associative_adaptor::detail::lower_bound(
					std::begin(this->container_), std::end(this->container_),
					key, value_compare{})};
		}

		//Returns the lower bound of the given key
		//A const iterator to the first element that is greater or equal
		[[nodiscard]] inline auto lower_bound(const key_type &key) const noexcept
		{
			return lower_bound<key_type>(key);
		}

		//Returns the lower bound of the given key
		//A const iterator to the first element that is greater or equal
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, key_type> || types::is_transparent_comparator_v<Compare>>>
		[[nodiscard]] inline auto lower_bound(const T &key) const noexcept
		{
			return typename my_base::const_iterator{
				flat_associative_adaptor::detail::lower_bound(
					std::begin(this->container_), std::end(this->container_),
					key, value_compare{})};
		}


		//Returns the upper bound of the given key
		//An iterator to the first element that is greater
		[[nodiscard]] inline auto upper_bound(const key_type &key) noexcept
		{
			return upper_bound<key_type>(key);
		}

		//Returns the upper bound of the given key
		//An iterator to the first element that is greater
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, key_type> || types::is_transparent_comparator_v<Compare>>>
		[[nodiscard]] inline auto upper_bound(const T &key) noexcept
		{
			return typename my_base::iterator{
				flat_associative_adaptor::detail::upper_bound(
					std::begin(this->container_), std::end(this->container_),
					key, value_compare{})};
		}

		//Returns the upper bound of the given key
		//A const iterator to the first element that is greater
		[[nodiscard]] inline auto upper_bound(const key_type &key) const noexcept
		{
			return upper_bound<key_type>(key);
		}

		//Returns the upper bound of the given key
		//A const iterator to the first element that is greater
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, key_type> || types::is_transparent_comparator_v<Compare>>>
		[[nodiscard]] inline auto upper_bound(const T &key) const noexcept
		{
			return typename my_base::const_iterator{
				flat_associative_adaptor::detail::upper_bound(
					std::begin(this->container_), std::end(this->container_),
					key, value_compare{})};
		}


		/*
			Inserting
		*/

		//Emplace a key-value pair by forwarding the given arguments to its constructor
		template <typename... Args>
		inline auto emplace(Args &&...args)
		{
			return insert(typename my_base::value_type{std::forward<Args>(args)...});
		}

		//Emplace with hint, a key-value pair by forwarding the given arguments to its constructor
		template <typename... Args>
		inline auto emplace_hint(typename my_base::iterator hint, Args &&...args)
		{
			return insert(hint, typename my_base::value_type{std::forward<Args>(args)...});
		}


		//Insert a key-value pair by copying the given value
		inline auto insert(const typename my_base::value_type &value)
		{
			auto [iter, found] =
				flat_associative_adaptor::detail::find(
					std::begin(this->container_), std::end(this->container_),
					flat_associative_adaptor::detail::extract_key<key_type>(value), value_compare{});

			//Not found, insert
			if (!found)
				iter = this->container_.insert(iter, value);

			return std::pair{typename my_base::iterator{iter}, !found};
		}

		//Insert a key-value pair by moving the given value in place
		inline auto insert(typename my_base::value_type &&value)
		{
			auto [iter, found] =
				flat_associative_adaptor::detail::find(
					std::begin(this->container_), std::end(this->container_),
					flat_associative_adaptor::detail::extract_key<key_type>(value), value_compare{});

			//Not found, insert
			if (!found)
				iter = this->container_.insert(iter, std::move(value));

			return std::pair{typename my_base::iterator{iter}, !found};
		}

		//Insert with hint, a key-value pair by copying the given value
		inline auto insert(typename my_base::iterator hint, const typename my_base::value_type &value)
		{
			auto [iter, found] =
				flat_associative_adaptor::detail::find_with_hint(
					typename my_base::container_type::iterator{hint},
					std::begin(this->container_), std::end(this->container_),
					flat_associative_adaptor::detail::extract_key<key_type>(value), value_compare{});

			//Not found, insert
			if (!found)
				iter = this->container_.insert(iter, value);

			return std::pair{typename my_base::iterator{iter}, !found};
		}

		//Insert with hint, a key-value pair by moving the given value in place
		inline auto insert(typename my_base::iterator hint, typename my_base::value_type &&value)
		{
			auto [iter, found] =
				flat_associative_adaptor::detail::find_with_hint(
					typename my_base::container_type::iterator{hint},
					std::begin(this->container_), std::end(this->container_),
					flat_associative_adaptor::detail::extract_key<key_type>(value), value_compare{});

			//Not found, insert
			if (!found)
				iter = this->container_.insert(iter, std::move(value));

			return typename my_base::iterator{iter};
		}


		//Insert multiple key_value pair at once, by copying, sorting and merging the given values
		//Warning. After all values are added, duplicate values will be removed
		inline auto insert(const typename my_base::container_type &container)
		{
			return insert<typename my_base::container_type>(container);
		}

		//Insert multiple key_value pair at once, by copying, sorting and merging the given values
		//Warning. After all values are added, duplicate values will be removed
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<typename T::value_type, typename my_base::value_type> &&
								std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<typename T::iterator>::iterator_category>>>
		inline auto insert(const T &container)
		{
			auto size = std::size(this->container_);

			this->container_.insert(
				std::end(this->container_),
				std::begin(container), std::end(container));
			flat_associative_adaptor::detail::merge_added(
				this->container_, std::begin(this->container_) + size, value_compare{});

			return size < std::size(this->container_);
		}

		//Insert multiple key_value pair at once, by moving, sorting and merging the given values
		//Warning. After all values are added, duplicate values will be removed
		inline auto insert(typename my_base::container_type &&container)
		{
			auto size = std::size(this->container_);

			this->container_.insert(
				std::end(this->container_), 
				std::make_move_iterator(std::begin(container)),
				std::make_move_iterator(std::end(container)));
			flat_associative_adaptor::detail::merge_added(
				this->container_, std::begin(this->container_) + size, value_compare{});

			return size < std::size(this->container_);
		}


		/*
			Removing
		*/

		//Clear all elements in the underlying container
		inline void clear() noexcept
		{
			this->container_.clear();
		}

		//Erase the element with the given key
		inline auto erase(const key_type &key) noexcept
		{
			return erase<key_type>(key);
		}

		//Erase the element with the given key
		template <typename T,
			typename = std::enable_if_t<std::is_same_v<T, key_type> || types::is_transparent_comparator_v<Compare>>>
		inline auto erase(const T &key) noexcept
		{
			auto [iter, found] =
				flat_associative_adaptor::detail::find(std::begin(this->container_), std::end(this->container_),
													   key, value_compare{});

			//Found, erase
			if (found)
				this->container_.erase(iter);

			return found;
		}

		//Erase the element at the given iterator
		inline auto erase(typename my_base::iterator where) noexcept
		{
			return typename my_base::iterator{this->container_.erase(where)};
		}

		//Erase all elements in range [first, last)
		inline auto erase(typename my_base::iterator first, typename my_base::iterator last) noexcept
		{
			return typename my_base::iterator{this->container_.erase(first, last)};
		}


		//Erase all elements satisfying the given predicate
		template <typename Predicate>
		inline auto erase_if(Predicate predicate) noexcept
		{
			return typename my_base::iterator{this->container_.erase(
				std::remove_if(std::begin(this->container_), std::end(this->container_), predicate),
				std::end(this->container_))};
		}

		//Erase all elements satisfying the given predicate, in range [first, last)
		template <typename Predicate>
		inline auto erase_if(typename my_base::iterator first, typename my_base::iterator last, Predicate predicate) noexcept
		{
			return typename my_base::iterator{this->container_.erase(
				std::remove_if(first, last, predicate),
				last)};
		}
	};
} //ion::adaptors

#endif