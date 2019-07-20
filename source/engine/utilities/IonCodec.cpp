/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonCodec.cpp
-------------------------------------------
*/

#include "IonCodec.h"

namespace ion::utilities::codec
{

namespace detail
{

bool is_base(std::string_view str, int base) noexcept
{
	static const auto first_number = '0';

	if (base <= 10)
	{
		auto last_number =
			static_cast<char>(first_number + (base - 1));

		for (auto c : str)
		{
			if (!(c >= first_number && c <= last_number))
				return false;
		}
	}
	else
	{
		static const auto last_number = '9';
		static const auto first_letter_lower = 'a';
		static const auto first_letter_upper = 'A';
				
		auto last_letter_lower =
			static_cast<char>(first_letter_lower + (base - 11));
		auto last_letter_upper =
			static_cast<char>(first_letter_upper + (base - 11));

		for (auto c : str)
		{
			if (!((c >= first_number && c <= last_number) ||
				(c >= first_letter_lower && c <= last_letter_lower) ||
				(c >= first_letter_upper && c <= last_letter_upper)))
					return false;
		}
	}

	return true;
}

std::string string_to_hex(std::string_view str)
{
	std::string result;
	result.reserve(std::size(str) * 2);

	for (unsigned char c : str)
	{
		result.push_back(base36_map[c >> 4]);
		result.push_back(base36_map[c & 15]);
	}

	return result;
}

std::string string_to_base32(std::string_view str, std::optional<char> padding_character)
{
	return rfc_base<base32_map>::Encode(str, padding_character);
}

std::string string_to_base64(std::string_view str, std::optional<char> padding_character)
{
	return rfc_base<base64_map>::Encode(str, padding_character);
}

std::string string_to_base64_url(std::string_view str, std::optional<char> padding_character)
{
	return rfc_base<base64url_map>::Encode(str, padding_character);
}

std::optional<std::string> hex_to_string(std::string_view str)
{
	if (std::empty(str))
		return "";

	auto size = static_cast<int>(std::size(str));

	std::string result;
    result.reserve(size / 2);

    for (auto i = 1; i < size; i += 2)
	{
		auto first = rfc_base<base36_map, 16>::
			Unmap[static_cast<unsigned char>(str[i - 1])];
		auto second = rfc_base<base36_map, 16>::
			Unmap[static_cast<unsigned char>(str[i])];

		//Input contains invalid character (not hexadecimal)
		if (first >= 16 || second >= 16)
			return {};

		result.push_back((first << 4) | second);
	}

	return result;
}

std::optional<std::string> base32_to_string(std::string_view str)
{
	return rfc_base<base32_map>::Decode(str);
}

std::optional<std::string> base64_to_string(std::string_view str)
{
	return rfc_base<base64_map>::Decode(str);
}

std::optional<std::string> base64_url_to_string(std::string_view str)
{
	return rfc_base<base64url_map>::Decode(str);
}

} //detail


/*
	Encoding (RFC 4648)
	Base 16/32/64
*/

std::string EncodeToHex(std::string_view str)
{
	return detail::string_to_hex(str);
}


std::string EncodeToBase32(std::string_view str)
{
	return detail::string_to_base32(str, {});
}

std::string EncodeToBase32(std::string_view str, char padding_character)
{
	return detail::string_to_base32(str, padding_character);
}


std::string EncodeToBase64(std::string_view str)
{
	return detail::string_to_base64(str,{});
}

std::string EncodeToBase64(std::string_view str, char padding_character)
{
	return detail::string_to_base64(str, padding_character);
}

std::string EncodeToBase64_URL(std::string_view str)
{
	return detail::string_to_base64_url(str,{});
}

std::string EncodeToBase64_URL(std::string_view str, char padding_character)
{
	return detail::string_to_base64_url(str, padding_character);
}


/*
	Decoding (RFC 4648)
	Base 16/32/64
*/

std::optional<std::string> DecodeFromHex(std::string_view str)
{
	return detail::hex_to_string(str);
}

std::optional<std::string> DecodeFromBase32(std::string_view str)
{
	return detail::base32_to_string(str);
}

std::optional<std::string> DecodeFromBase64(std::string_view str)
{
	return detail::base64_to_string(str);
}

std::optional<std::string> DecodeFromBase64_URL(std::string_view str)
{
	return detail::base64_url_to_string(str);
}

} //ion::utilities::codec