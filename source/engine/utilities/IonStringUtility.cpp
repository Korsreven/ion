/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonStringUtility.cpp
-------------------------------------------
*/

#include "IonStringUtility.h"

#include <algorithm>
#include <cmath>

namespace ion::utilities::string
{

namespace detail
{

size_t case_insensitive_find(std::string_view str1, std::string_view str2, size_t off) noexcept
{
	auto iter = std::search(std::begin(str1) + off, std::end(str1),
		std::begin(str2), std::end(str2), case_insensitive_equal);
	return iter != std::end(str1) ?
		//Found
		iter - std::begin(str1) :
		//Not found
		std::string::npos;
}

bool compare_pattern(std::string_view::const_iterator first, std::string_view::const_iterator last,
	std::string_view::const_iterator first_pattern, std::string_view::const_iterator last_pattern,
	StringCase string_case) noexcept
{
	if (last - first != last_pattern - first_pattern)
		return false;

	return string_case == StringCase::Insensitive ? 
		std::equal(first, last, first_pattern, case_insensitive_equal_with_pattern) :
		std::equal(first, last, first_pattern, case_sensitive_equal_with_pattern);
}

size_t find_pattern(std::string_view::const_iterator first, std::string_view::const_iterator last,
	std::string_view::const_iterator first_pattern, std::string_view::const_iterator last_pattern,
	StringCase string_case) noexcept
{
	auto iter = (string_case == StringCase::Insensitive ?
		std::search(first, last, first_pattern, last_pattern, case_insensitive_equal_with_pattern) :
		std::search(first, last, first_pattern, last_pattern, case_sensitive_equal_with_pattern));

	return iter != last ?
		//Found
		iter - first :
		//Not found
		std::string::npos;
}

std::pair<size_t, size_t> find_wildcard(std::string_view str, size_t off) noexcept
{
	if (auto first = str.find_first_of(pattern_wildcards, off); first != std::string::npos)
	{
		if (auto last = str.find_first_not_of(pattern_wildcards, first + 1); last != std::string::npos)
			return {first, last - first};

		return {first, std::size(str) - first};
	}

	return {std::string::npos, 0};
}

std::string combine(const Strings &parts, std::optional<std::string_view> delimiter,
	int start, std::optional<int> count)
{
	if (start < 0)
		start = 0;

	if (count && *count <= 0 ||
		start >= std::ssize(parts))
			return "";

	auto str = parts[start];
	auto last = std::min(
		start + count.value_or(std::size(parts)),
		std::ssize(parts));

	if (last > ++start)
	{
		//Combine with delimiter
		if (delimiter)
		{
			for (auto iter = std::cbegin(parts) + start,
				end = std::cbegin(parts) + last;
				iter != end; ++iter)
			{
				str += *delimiter;
				str += *iter;
			}
		}
		//Combine without delimiter
		else
		{
			for (auto iter = std::cbegin(parts) + start,
				end = std::cbegin(parts) + last;
				iter != end; ++iter)
					str += *iter;
		}
	}

	return str;
}

Strings tokenize(std::string_view str, std::string_view delimiters,
	std::optional<int> max_splits, StringSplitOptions split_options)
{
	if (max_splits && *max_splits <= 0)
		return {};

	Strings parts;

	for (size_t from = 0, to = 0;
		to != std::string::npos;
		from = to + 1)
	{
		to = str.find_first_of(delimiters, from);
		auto part = str.substr(from, to - from);

		if ((split_options == StringSplitOptions::PreserveDelimiters ||
			split_options == StringSplitOptions::PreserveEmptyEntriesAndDelimiters) &&
			from > 0)
		{
			parts.push_back(std::string(1, str[from - 1]));

			if (max_splits &&
				*max_splits == std::ssize(parts))
					break;
		}

		if (std::empty(part) &&
			!(split_options == StringSplitOptions::PreserveEmptyEntries ||
			split_options == StringSplitOptions::PreserveEmptyEntriesAndDelimiters))
				continue;

		parts.emplace_back(part);

		if (max_splits &&
			*max_splits == std::ssize(parts))
				break;
	}

	return parts;
}

std::string format(real number, std::string_view format_str)
{
	if (!std::isfinite(number))
		return convert::ToString(number); //inf, nan

	//Format specifiers

	auto decimal_point = format_str.find(detail::format_decimal_point);
	auto natural_placeholder = decimal_point != std::string::npos ?
		format_str.substr(0, decimal_point).find_last_of(detail::format_placeholders) :
		format_str.find_last_of(detail::format_placeholders);
	auto mantissa_placeholder = decimal_point != std::string::npos ?
		format_str.find_first_of(detail::format_placeholders, decimal_point + 1) :
		std::string::npos;

	auto min_digits =
		[&]() noexcept
		{
			auto digits = 0;
			auto zero_digits = 0;

			for (; natural_placeholder != std::string::npos;
				natural_placeholder = format_str.find_last_of(detail::format_placeholders, natural_placeholder - 1))
			{
				++digits;

				if (format_str[natural_placeholder] == detail::format_zero_placeholder)
					zero_digits = digits;

				if (natural_placeholder == 0)
					break;
			}

			return zero_digits;
		}();

	auto [min_decimal_digits, decimal_digits] =
		[&]() noexcept -> std::pair<int, int>
		{
			auto digits = 0;
			auto zero_digits = 0;

			for (; mantissa_placeholder != std::string::npos;
				mantissa_placeholder = format_str.find_first_of(detail::format_placeholders, mantissa_placeholder + 1))
			{
				++digits;

				if (format_str[mantissa_placeholder] == detail::format_zero_placeholder)
					zero_digits = digits;
			}

			return {zero_digits, digits};
		}();

	//Format number

	auto str = (decimal_digits == 0 && decimal_point != std::string::npos) ?
		//Arbitrary precision
		convert::ToString(number, {}) :
		//Fixed precision
		convert::ToString(number, decimal_digits);
	auto str_decimal_point = str.find('.');
	auto natural_digits = str_decimal_point != std::string::npos ?
		str_decimal_point :
		std::size(str);

	//Remove rightmost zeroes from decimals
	for (auto digits = decimal_digits; digits > min_decimal_digits; --digits)
	{
		if (str.back() != '0')
			break;

		str.pop_back();
	}

	//Do not end with decimal point
	if (str.back() == '.')
		str.pop_back();

	//Left pad with zeroes
	if (min_digits > static_cast<int>(natural_digits))
	{
		str.insert(0, min_digits - natural_digits, '0');
		natural_digits += min_digits - natural_digits;
	}

	//Add thousands separators
	if (format_str.find(detail::format_thousands_separator) != std::string::npos)
	{
		if (natural_digits > 3)
		{
			auto count = (natural_digits - 1) / 3;
			for (auto off = natural_digits - 3; count > 0; --count, off -= 3)
				str.insert(off, 1, ' ');
		}
	}

	return str;
}

} //detail


/*
	Comparing
	Case sensitive/insensitive
*/

bool Compare(std::string_view str1, std::string_view str2,
	StringCase string_case) noexcept
{
	return string_case == StringCase::Insensitive ?
		//Insensitive
		std::size(str1) == std::size(str2) &&
		std::equal(std::begin(str1), std::end(str1),
			std::begin(str2), detail::case_insensitive_equal) :
		//Sensitive
		str1 == str2;
}

bool StartsWith(std::string_view str1, std::string_view str2,
	StringCase string_case) noexcept
{
	if (std::empty(str2))
		return std::empty(str1);

	return string_case == StringCase::Insensitive ?
		//Insensitive
		std::size(str1) >= std::size(str2) &&
		std::equal(std::begin(str1), std::begin(str1) + std::size(str2),
			std::begin(str2), std::end(str2), detail::case_insensitive_equal) :
		//Sensitive
		str1.compare(0, std::size(str2), str2) == 0;
}

bool EndsWith(std::string_view str1, std::string_view str2,
	StringCase string_case) noexcept
{
	if (std::empty(str2))
		return std::empty(str1);

	return string_case == StringCase::Insensitive ?
		//Insensitive
		std::size(str1) >= std::size(str2) &&
		std::equal(std::end(str1) - std::size(str2), std::end(str1),
			std::begin(str2), std::end(str2), detail::case_insensitive_equal) :
		//Sensitive
		str1.compare(std::size(str1) - std::size(str2), std::size(str2), str2) == 0;
}

bool Match(std::string_view str, std::string_view pattern,
	StringCase string_case) noexcept
{
	//No pattern
	if (std::empty(pattern))
		return std::empty(str);

	//Find first wildcard
	auto [off, count] = detail::find_wildcard(pattern, 0);

	//Pattern contains no wildcards
	//	_
	if (off == std::string::npos)
		return detail::compare_pattern(std::begin(str), std::end(str),
			std::begin(pattern), std::end(pattern), string_case);

	//Pattern contains only wildcards
	//	%*
	if (off == 0 && count == std::size(pattern))
		return true;

	//Pattern contains only wildcard at back
	//	_%*
	if (off == std::size(pattern) - count)
		return detail::compare_pattern(std::begin(str), std::begin(str) + (std::size(pattern) - count),
			std::begin(pattern), std::end(pattern) - count, string_case);

	//Find second wildcard
	auto [off2, count2] = detail::find_wildcard(pattern, off + count);

	//Pattern contains only one wildcard
	if (off2 == std::string::npos)
	{
		//Pattern contains only one wildcard at front
		//	%*_
		if (off == 0)
			return detail::compare_pattern(
				std::size(str) < std::size(pattern) - (count - 1) ?
					//Str less than pattern
					std::begin(str) :
					//Str greater or equal to pattern
					std::end(str) - (std::size(pattern) - count),
				std::end(str),
				std::begin(pattern) + count, std::end(pattern), string_case);
		
		//Patterns contains only one wildcard in middle
		//	_%*_
		return	//Compare left
				detail::compare_pattern(std::begin(str), std::begin(str) + off,
				std::begin(pattern), std::begin(pattern) + off, string_case)
				&&
				//Compare right
				detail::compare_pattern(std::end(str) - (std::size(pattern) - off - count), std::end(str),
				std::begin(pattern) + off + count, std::end(pattern), string_case);
	}

	//Pattern contains only wildcard at front and back
	//	%*_%*
	if (off == 0 && off2 == std::size(pattern) - count2)
		return detail::find_pattern(std::begin(str), std::end(str),
			std::begin(pattern) + count, std::end(pattern) - count2, string_case) != std::string::npos;

	//First wildcard (not leftmost)
	//	_%*_
	if (off > 0 &&
		!detail::compare_pattern(std::begin(str), std::begin(str) + off,
		std::begin(pattern), std::begin(pattern) + off, string_case))
			return false;

	//Search through remaining wildcards
	for (size_t str_off = off; off2 != std::string::npos;)
	{
		//More wildcards to the right
		auto pos = detail::find_pattern(std::begin(str) + str_off, std::end(str),
			std::begin(pattern) + off + count, std::begin(pattern) + off2, string_case);

		if (pos == std::string::npos)
			return false;

		str_off += pos + (off2 - (off + count));

		//Get next wildcard
		auto [i, n] = detail::find_wildcard(pattern, off2 + count2);
		off = off2;
		count = count2;
		off2 = i;
		count2 = n;
	}

	//Last wildcard (not rightmost)
	return off + count < std::size(pattern) ?
		detail::compare_pattern(std::end(str) - (std::size(pattern) - (off + count)), std::end(str),
			std::begin(pattern) + off + count, std::end(pattern), string_case) :
		true;
}


/*
	Concatenating
	Strings and numbers
*/

std::string Concat(const Strings &parts)
{
	return detail::combine(parts, {}, 0, {});
}

std::string Concat(const Strings &parts, int count)
{
	return detail::combine(parts, {}, 0, count);
}

std::string Concat(const Strings &parts, int start, int count)
{
	return detail::combine(parts, {}, start, count);
}


/*
	Splitting
	Using current StringSplitOptions
*/

Strings Split(std::string_view str, std::string_view delimiters,
	StringSplitOptions split_options)
{
	return detail::tokenize(str, delimiters, {}, split_options);
}

Strings Split(std::string_view str, std::string_view delimiters,
	int max_splits, StringSplitOptions split_options)
{
	return detail::tokenize(str, delimiters, max_splits, split_options);
}


/*
	Joining
	Strings and numbers
*/

std::string Join(std::string_view delimiter, const Strings &parts)
{
	return detail::combine(parts, delimiter, 0, {});
}

std::string Join(std::string_view delimiter, const Strings &parts, int count)
{
	return detail::combine(parts, delimiter, 0, count);
}

std::string Join(std::string_view delimiter, const Strings &parts, int start, int count)
{
	return detail::combine(parts, delimiter, start, count);
}


/*
	Formating
	Strings and numbers
*/

std::string Format(real number, std::string_view format_str)
{
	return detail::format(number, format_str);
}


/*
	Padding
	Inplace or by copying
*/

std::string& PadLeft(std::string &str, int length, std::string_view characters)
{
	//String is too short, pad with the extra characters
	if (std::ssize(str) < length &&
		!std::empty(characters))
	{
		auto pad_length = length - std::size(str);
		str.insert(std::begin(str), pad_length, '\0');

		for (size_t i = 0; i < pad_length; ++i)
			str[i] = characters[i % std::size(characters)];
	}

	return str;
}

std::string& PadRight(std::string &str, int length, std::string_view characters)
{
	//String is too short, pad with the extra characters
	if (std::ssize(str) < length &&
		!std::empty(characters))
	{
		if (static_cast<int>(str.capacity()) < length)
			str.reserve(length);

		auto pad_length = length - std::size(str);

		for (size_t i = 0; i < pad_length; ++i)
			str += characters[i % std::size(characters)];
	}

	return str;
}

std::string PadLeftCopy(std::string str, int length, std::string_view characters)
{
	return PadLeft(str, length, characters);
}

std::string PadRightCopy(std::string str, int length, std::string_view characters)
{
	return PadRight(str, length, characters);
}


/*
	Removing
	Alpha/numeric
	Non-printable
*/

std::string& RemoveAlpha(std::string &str) noexcept
{
	str.erase(std::remove_if(std::begin(str), std::end(str), [](unsigned char c) { return std::isalpha(c); }), std::end(str));
	return str;
}

std::string& RemoveNumeric(std::string &str) noexcept
{
	str.erase(std::remove_if(std::begin(str), std::end(str), [](unsigned char c) { return std::isdigit(c); }), std::end(str));
	return str;
}

std::string& RemoveNonPrintable(std::string &str) noexcept
{
	str.erase(std::remove_if(std::begin(str), std::end(str), [](unsigned char c) { return !std::isprint(c); }), std::end(str));
	return str;
}

std::string RemoveAlphaCopy(std::string str) noexcept
{
	return RemoveAlpha(str);
}

std::string RemoveNumericCopy(std::string str) noexcept
{
	return RemoveNumeric(str);
}

std::string RemoveNonPrintableCopy(std::string str) noexcept
{
	return RemoveNonPrintable(str);
}


/*
	Replacing
	Case sensitive/insensitive
	Inplace or by copying
*/

std::string& ReplaceAll(std::string &str, std::string_view what, std::string_view with_what,
	StringCase string_case)
{
	if (string_case == StringCase::Insensitive)
	{
		for (size_t off = 0;
			(off = detail::case_insensitive_find(str, what, off)) != std::string::npos;
			off += std::size(with_what))
				str.replace(off, std::size(what), with_what);
	}
	//Case sensitive
	else
	{
		for (size_t off = 0;
			(off = str.find(what, off)) != std::string::npos;
			off += std::size(with_what))
				str.replace(off, std::size(what), with_what);
	}

	return str;
}

std::string ReplaceAllCopy(std::string str, std::string_view what, std::string_view with_what,
	StringCase string_case)
{
	return ReplaceAll(str, what, with_what, string_case);
}


/*
	Transforming
	Inplace or by copying
*/

std::string& ToLowerCase(std::string &str) noexcept
{
	std::transform(std::begin(str), std::end(str), std::begin(str), detail::to_lower);
	return str;
}

std::string& ToUpperCase(std::string &str) noexcept
{
	std::transform(std::begin(str), std::end(str), std::begin(str), detail::to_upper);
	return str;
}

std::string ToLowerCaseCopy(std::string str)
{
	return ToLowerCase(str);
}

std::string ToUpperCaseCopy(std::string str)
{
	return ToUpperCase(str);
}


/*
	Trimming
	Inplace or by copying
*/

std::string& TrimLeft(std::string &str, std::string_view characters) noexcept
{
	return str.erase(0, str.find_first_not_of(characters));
}

std::string& TrimRight(std::string &str, std::string_view characters) noexcept
{
	return str.erase(str.find_last_not_of(characters) + 1);
}

std::string& Trim(std::string &str, std::string_view characters) noexcept
{
	return TrimLeft(TrimRight(str, characters), characters);
}

std::string TrimLeftCopy(std::string str, std::string_view characters)
{
	return TrimLeft(str, characters);
}

std::string TrimRightCopy(std::string str, std::string_view characters)
{
	return TrimRight(str, characters);
}

std::string TrimCopy(std::string str, std::string_view characters)
{
	return Trim(str, characters);
}

} //ion::utilities::string