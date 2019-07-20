/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/utilities
File:	IonParseUtility.cpp
-------------------------------------------
*/

#include "IonParseUtility.h"

#include <algorithm>

#include "utilities/IonCodec.h"
#include "utilities/IonConvert.h"
#include "utilities/IonStringUtility.h"

namespace ion::script::utilities::parse
{

using namespace ion::utilities;
using namespace ion::graphics::utilities;

namespace detail
{

std::optional<graphics::utilities::Color> hex_as_color(std::string_view str) noexcept
{
	auto value =
		[&]() noexcept
		{
			//HTML hexadecimal prefix (#)
			if (!std::empty(str) && str.front() == '#')
				return codec::DecodeFrom<uint32>({std::data(str) + 1, std::size(str) - 1}, 16);

			//Numeric value
			else
				return convert::To<uint32>(std::data(str));
		}();

	return value ?
		std::make_optional(Color::Hex(*value)) :
		std::nullopt;
}

std::optional<graphics::utilities::Color> rgb_as_color(std::string str) noexcept
{
	if (auto first = str.find(',');
		first != std::string::npos)
	{
		if (auto second = str.find(',', first + 1);
			second != std::string::npos)
		{
			//Found r,g,b (required)
			//Search for alpha (optional)
			auto third = str.find(',', second + 1);

			str[first] = '\0';
			str[second] = '\0';

			if (third != std::string::npos)
				str[third] = '\0';

			auto rgb_8 =
				parse_as_integer({std::data(str), first}) &&
				parse_as_integer({std::data(str) + first + 1, second - first - 1}) &&
				parse_as_integer(std::data(str) + second + 1);

			//Alpha [0.0, 1.0]
			auto a = (third != std::string::npos) ?
				convert::To<real>(std::data(str) + third + 1) :
				std::nullopt;

			//RGB [0, 255]
			if (rgb_8)
			{
				auto r = convert::To<uint8>(std::data(str));
				auto g = convert::To<uint8>(std::data(str) + first + 1);
				auto b = convert::To<uint8>(std::data(str) + second + 1);

				if (r && g && b)
				{
					if (a)
						return Color::RGB(*r, *g, *b, *a);
					//No alpha expected
					else if (third == std::string::npos)
						return Color::RGB(*r, *g, *b);
				}
			}
			//RGB percentages [0.0, 1.0]
			else
			{
				auto r = convert::To<real>(std::data(str));
				auto g = convert::To<real>(std::data(str) + first + 1);
				auto b = convert::To<real>(std::data(str) + second + 1);

				if (r && g && b)
				{
					if (a)
						return Color{*r, *g, *b, *a};
					//No alpha expected
					else if (third == std::string::npos)
						return Color{*r, *g, *b};
				}
			}
		}
	}

	return {};
}

std::optional<graphics::utilities::Color> color_name_as_color(std::string_view str) noexcept
{
	//Look up in color map
	auto iter =
		std::lower_bound(std::begin(detail::color_map), std::end(detail::color_map), str,
			[](const color_pair_type &x, std::string_view y) noexcept
			{
				return std::lexicographical_compare(
					std::begin(x.first), std::end(x.first),
					std::begin(y), std::end(y),
					string::detail::case_insensitive_less);
			});

	return iter != std::end(detail::color_map) &&
		string::Compare(iter->first, str, string::StringCase::Insensitive) ?

		std::make_optional(iter->second) :
		std::nullopt;
}

std::optional<bool> string_as_boolean(std::string_view str) noexcept
{
	//1 or 0
	if (str == "1")
		return true;
	else if (str == "0")
		return false;

	//true or false
	else if (str == "true")
		return true;
	else if (str == "false")
		return false;

	return {};
}

std::optional<Color> string_as_color(std::string str) noexcept
{
	if (std::empty(str))
		return {};

	//Components (r,g,b,a)
	else if (str.find(',') != std::string::npos)
		return rgb_as_color(std::move(str));

	//Hexadecimal or numeric value
	else if (str.front() == '#' || convert::detail::is_digit(str.front()))
		return hex_as_color(str);

	//Color name
	else
		return color_name_as_color(str);
}

std::optional<Vector2> string_as_vector2(std::string str) noexcept
{
	//Components (x,y)
	if (auto off = str.find(',');
		off != std::string::npos)
	{
		str[off] = '\0';

		auto x = convert::To<real>(std::data(str));
		auto y = convert::To<real>(std::data(str) + off + 1);

		if (x && y)
			return Vector2{*x, *y};
	}
	//Scalar
	else
	{
		auto scalar = convert::To<real>(str);

		if (scalar)
			return Vector2{*scalar};
	}

	return {};
}

std::optional<std::string> string_literal_as_string(std::string_view str)
{
	//Double or single quoted
	if (std::size(str) > 1 &&
		(str.front() == '"' || str.front() == '\'') &&
		str.front() == str.back()) //Matching front/back
	{
		std::string result;

		for (auto iter = std::begin(str) + 1,
			end = std::end(str); iter != end; ++iter)
		{
			auto c = *iter;
			auto next_c = iter + 1 != end ? *(iter + 1) : '\0';

			//Skip all control characters
			if (is_control_character(c))
			{
				//Line feed must be escaped
				if (c == '\n')
					break;
				else
					continue;
			}

			//Character escape sequences
			else if (c == '\\' && next_c)
			{
				//Double or single quote (escaped)
				if (next_c == str.front())
					result += next_c;
				else
				{
					//Control characters
					switch (next_c)
					{
						case '\\': //Backslash
						result += '\\';
						break;

						case 'a': //CSS
						case 'n': //Line feed
						result += '\n';
						break;

						case 'f': //Form feed
						result += '\f';
						break;

						case 'r': //Carriage return
						result += '\r';
						break;

						case 't': //Horizontal tab
						result += '\t';
						break;

						case 'v': //Vertical tab
						result += '\v';
						break;

						case '0': //Null character
						result += '\0';
						break;

						case '\n': //Consume escaped line feed (multi-line string)
						break;

						//Unrecognized character escape sequence
						default:
						continue;
					}
				}

				++iter; //Skip next character
			}

			//Double or single quote (not escaped)
			else if (c == str.front())
			{
				if (iter + 1 == end)
					return result;
				else
					break;
			}

			//Append character
			else
				result += c;
		}
	}

	return {};
}

} //detail


/*
	String as boolean
*/

std::optional<bool> AsBoolean(std::string_view str) noexcept
{
	return detail::string_as_boolean(str);
}


/*
	String as color
*/

std::optional<Color> AsColor(std::string str) noexcept
{
	return detail::string_as_color(std::move(str));
}


/*
	String literal as string
*/

std::optional<std::string> AsString(std::string_view str)
{
	return detail::string_literal_as_string(str);
}


/*
	String as vector2
*/

std::optional<Vector2> AsVector2(std::string str) noexcept
{
	return detail::string_as_vector2(std::move(str));
}

} //ion::script::utilities::parse