/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	types
File:	IonTypeTraits.h
-------------------------------------------
*/

#ifndef ION_TYPE_TRAITS_H
#define ION_TYPE_TRAITS_H

#include <functional>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>

#undef min
#undef max

namespace ion::types
{
	namespace detail
	{
		template <typename T>
		struct is_char_impl : std::false_type
		{
		};

		template<>
		struct is_char_impl<char> : std::true_type
		{
		};

		template<>
		struct is_char_impl<unsigned char> : std::true_type
		{
		};

		template<>
		struct is_char_impl<signed char> : std::true_type
		{
		};


		template <typename T>
		struct is_string_impl : std::false_type
		{
		};

		template <>
		struct is_string_impl<std::string> : std::true_type
		{
		};

		template <>
		struct is_string_impl<std::string_view> : std::true_type
		{
		};

		template <>
		struct is_string_impl<char*> : std::true_type
		{
		};

		template <>
		struct is_string_impl<const char*> : std::true_type
		{
		};

		template <>
		struct is_string_impl<volatile char*> : std::true_type
		{
		};
		
		template <>
		struct is_string_impl<const volatile char*> : std::true_type
		{
		};


		template <typename T>
		struct is_pair_impl : std::false_type
		{
		};

		template <typename T1, typename T2>
		struct is_pair_impl<std::pair<T1, T2>> : std::true_type
		{
		};


		template <typename T, typename = void>
		struct is_const_iterator_impl : std::false_type
		{
		};

		template <typename T>
		struct is_const_iterator_impl<T,
			std::enable_if_t<std::is_const_v<std::remove_pointer_t<typename std::iterator_traits<T>::pointer>>>>
				: std::true_type
		{
		};


		template <typename T, typename = void>
		struct is_transparent_comparator_impl : std::false_type
		{
		};

		template <typename T>
		struct is_transparent_comparator_impl<T,
			std::void_t<typename T::is_transparent>> : std::true_type
		{
		};


		template <typename T>
		struct max_whole_number_impl
		{
		};

		template <>
		struct max_whole_number_impl<float>
		{
			static constexpr auto value =
				1 << std::numeric_limits<float>::digits;
		};

		template <>
		struct max_whole_number_impl<double>
		{
			static constexpr auto value =
				1LL << std::numeric_limits<double>::digits;
		};

		template <>
		struct max_whole_number_impl<long double>
		{
			static constexpr auto value =
				1LL << std::numeric_limits<long double>::digits;
		};
	} //detail


	/*
		Remove const volatile reference
		Type trait scheduled for C++20
	*/

	template <typename T>
	struct remove_cvref :
		std::remove_reference<std::remove_cv_t<T>>
	{
	};

	template <typename T>
	using remove_cvref_t = typename remove_cvref<T>::type;


	/*
		Identity
		Type trait that does not modify T
	*/

	template <typename T>
	struct identity
	{
		using type = T;
	};

	template <typename T>
	using identity_t = typename identity<T>::type;
	

	/*
		Is number
		Type trait that checks if the type is a number
	*/

	template <typename T>
	struct is_number :
		std::is_arithmetic<std::remove_reference_t<T>>
	{
	};

	template <typename T>
	constexpr auto is_number_v = is_number<T>::value;


	/*
		Is pair
		Type trait that checks if the type is a pair
	*/

	template <typename T>
	struct is_pair :
		detail::is_pair_impl<remove_cvref_t<T>>
	{
	};

	template <typename T>
	constexpr auto is_pair_v = is_pair<T>::value;


	/*
		Is char
		Type trait that checks if the type is char
	*/

	template <typename T>
	struct is_char :
		detail::is_char_impl<remove_cvref_t<T>>
	{
	};

	template <typename T>
	constexpr auto is_char_v = is_char<T>::value;


	/*
		Is string
		Type trait that checks if the type is string like
	*/

	template <typename T>
	struct is_string :
		detail::is_string_impl<std::decay_t<T>>
	{
	};

	template <typename T>
	constexpr auto is_string_v = is_string<T>::value;


	/*
		Is const iterator
		Type trait that checks if an iterator is const iterator
	*/

	template <typename T>
	struct is_const_iterator :
		detail::is_const_iterator_impl<T>
	{
	};

	template <typename T>
	constexpr auto is_const_iterator_v = is_const_iterator<T>::value;


	/*
		Is transparent comparator
		Type trait that checks if an comparator is transparent
	*/

	template <typename T>
	struct is_transparent_comparator :
		detail::is_transparent_comparator_impl<T>
	{
	};

	template <typename T>
	constexpr auto is_transparent_comparator_v = is_transparent_comparator<T>::value;


	/*
		Floating point max integer
		Type trait that checks the max integer value that a floating point can hold
	*/

	template <typename T>
	struct max_whole_number :
		detail::max_whole_number_impl<std::remove_reference_t<T>>
	{
	};

	template <typename T>
	constexpr auto min_whole_number_v = -max_whole_number<T>::value;

	template <typename T>
	constexpr auto max_whole_number_v = max_whole_number<T>::value;
} //ion::types

#endif