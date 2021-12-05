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

#include "utilities/IonCodec.h"
#include "utilities/IonConvert.h"
#include "utilities/IonStringUtility.h"

namespace ion::script::utilities::parse
{

using namespace ion::utilities;
using namespace graphics::utilities;

namespace detail
{

std::optional<graphics::utilities::Color> hex_as_color(std::string_view str) noexcept
{
	auto value =
		[&]() noexcept
		{
			//HTML hexadecimal prefix (#)
			if (!std::empty(str) && str.front() == '#')
				return codec::DecodeFrom<uint32>(str.substr(1, std::size(str) - 1), 16);

			//Numeric value
			else
				return convert::To<uint32>(str);
		}();

	return value ?
		std::make_optional(Color::Hex(*value)) :
		std::nullopt;
}

std::optional<graphics::utilities::Color> rgb_as_color(std::string_view str) noexcept
{
	if (auto first = str.find(',');
		first != std::string_view::npos)
	{
		if (auto second = str.find(',', first + 1);
			second != std::string_view::npos)
		{
			//Found r,g,b (required)
			//Search for alpha (optional)
			auto third = str.find(',', second + 1);

			auto rgb_8 =
				parse_as_integer(str.substr(0, first)) &&
				parse_as_integer(str.substr(first + 1, second - first - 1)) &&
				parse_as_integer(str.substr(second + 1, third - second - 1));

			//Alpha [0.0, 1.0]
			auto a = (third != std::string_view::npos) ?
				convert::To<real>(str.substr(third + 1)) :
				std::nullopt;

			//RGB [0, 255]
			if (rgb_8)
			{
				auto r = convert::To<uint8>(str.substr(0, first));
				auto g = convert::To<uint8>(str.substr(first + 1, second - first - 1));
				auto b = convert::To<uint8>(str.substr(second + 1, third - second - 1));

				if (r && g && b)
				{
					if (a)
						return Color::RGB(*r, *g, *b, *a);
					//No alpha expected
					else if (third == std::string_view::npos)
						return Color::RGB(*r, *g, *b);
				}
			}
			//RGB percentages [0.0, 1.0]
			else
			{
				auto r = convert::To<real>(str.substr(0, first));
				auto g = convert::To<real>(str.substr(first + 1, second - first - 1));
				auto b = convert::To<real>(str.substr(second + 1, third - second - 1));

				if (r && g && b)
				{
					if (a)
						return Color{*r, *g, *b, *a};
					//No alpha expected
					else if (third == std::string_view::npos)
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
		std::lower_bound(std::begin(color_map), std::end(color_map), str,
			[](const auto &x, const auto &y) noexcept
			{
				return std::lexicographical_compare(
					std::begin(x.first), std::end(x.first),
					std::begin(y), std::end(y),
					string::detail::case_insensitive_less);
			});

	return iter != std::end(color_map) &&
		string::Compare(iter->first, str, string::StringCase::Insensitive) ?

		std::make_optional(*iter->second) :
		std::nullopt;
}

std::optional<std::string_view> color_as_color_name(graphics::utilities::Color color) noexcept
{
	static auto color_name_map = make_color_name_map();

	auto [r, g, b] = color.ToRGB(); //Convert to RGB in range [0, 255]
	color = Color::RGB(
		static_cast<uint8>(r),
		static_cast<uint8>(g),
		static_cast<uint8>(b)); //Create new color from RGB (no alpha)

	//Look up in color name map
	auto iter =
		std::lower_bound(std::begin(color_name_map), std::end(color_name_map), color,
			[](const auto &x, const auto &y) noexcept
			{
				return *x.second < y;
			});

	return iter != std::end(color_name_map) && *iter->second == color ?
		std::make_optional(iter->first) :
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

std::optional<Color> string_as_color(std::string_view str) noexcept
{
	if (std::empty(str))
		return {};

	//Components (r,g,b,a)
	else if (str.find(',') != std::string_view::npos)
		return rgb_as_color(std::move(str));

	//Hexadecimal or numeric value
	else if (str.front() == '#' || convert::detail::is_digit(str.front()))
		return hex_as_color(str);

	//Color name
	else
		return color_name_as_color(str);
}

std::optional<Vector2> string_as_vector2(std::string_view str) noexcept
{
	//Components (x,y)
	if (auto off = str.find(',');
		off != std::string_view::npos)
	{
		auto x = convert::To<real>(str.substr(0, off));
		auto y = convert::To<real>(str.substr(off + 1));

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

std::optional<Vector3> string_as_vector3(std::string_view str) noexcept
{
	//Components (x,y,z)
	if (auto off = str.find(','), off2 = str.find(',', off + 1);
		off != std::string_view::npos && off2 != std::string_view::npos)
	{
		auto x = convert::To<real>(str.substr(0, off));
		auto y = convert::To<real>(str.substr(off + 1, off2 - off - 1));
		auto z = convert::To<real>(str.substr(off2 + 1));

		if (x && y && z)
			return Vector3{*x, *y, *z};
	}
	//Scalar
	else
	{
		auto scalar = convert::To<real>(str);

		if (scalar)
			return Vector3{*scalar};
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

						case '\r': //Consume escaped carriage return + line feed (multi-line string)
						{
							//CR LF
							if (iter + 2 != end && *(iter + 2) == '\n')
							{
								++iter; //Skip CR
								break;
							}

							[[fallthrough]];
						}

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

std::optional<Color> AsColor(std::string_view str) noexcept
{
	return detail::string_as_color(str);
}


/*
	String literal as string
	Color value as string
*/

std::optional<std::string> AsString(std::string_view str)
{
	return detail::string_literal_as_string(str);
}

std::optional<std::string_view> AsString(graphics::utilities::Color color)
{
	return detail::color_as_color_name(color);
}


/*
	String as vector
*/

std::optional<Vector2> AsVector2(std::string_view str) noexcept
{
	return detail::string_as_vector2(str);
}

std::optional<Vector3> AsVector3(std::string_view str) noexcept
{
	return detail::string_as_vector3(str);
}

} //ion::script::utilities::parse