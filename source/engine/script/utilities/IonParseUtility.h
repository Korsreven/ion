/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	script/utilities
File:	IonParseUtility.h
-------------------------------------------
*/

#ifndef _ION_PARSE_UTILITY_
#define _ION_PARSE_UTILITY_

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"

namespace ion::script::utilities::parse
{
	namespace detail
	{
		using color_pair_type = std::pair<std::string_view, const graphics::utilities::Color&>;

		inline const std::array color_map
		{
			//X11 color names
			color_pair_type{"AliceBlue",				graphics::utilities::color::AliceBlue},
			color_pair_type{"AntiqueWhite",				graphics::utilities::color::AntiqueWhite},
			color_pair_type{"Aqua",						graphics::utilities::color::Aqua},
			color_pair_type{"Aquamarine",				graphics::utilities::color::Aquamarine},
			color_pair_type{"Azure",					graphics::utilities::color::Azure},
			color_pair_type{"Beige",					graphics::utilities::color::Beige},
			color_pair_type{"Bisque",					graphics::utilities::color::Bisque},
			color_pair_type{"Black",					graphics::utilities::color::Black},
			color_pair_type{"BlanchedAlmond",			graphics::utilities::color::BlanchedAlmond},
			color_pair_type{"Blue",						graphics::utilities::color::Blue},
			color_pair_type{"BlueViolet",				graphics::utilities::color::BlueViolet},
			color_pair_type{"Brown",					graphics::utilities::color::Brown},
			color_pair_type{"BurlyWood",				graphics::utilities::color::BurlyWood},
			color_pair_type{"CadetBlue",				graphics::utilities::color::CadetBlue},
			color_pair_type{"Chartreuse",				graphics::utilities::color::Chartreuse},
			color_pair_type{"Chocolate",				graphics::utilities::color::Chocolate},
			color_pair_type{"Coral",					graphics::utilities::color::Coral},
			color_pair_type{"CornflowerBlue",			graphics::utilities::color::CornflowerBlue},
			color_pair_type{"Cornsilk",					graphics::utilities::color::Cornsilk},
			color_pair_type{"Crimson",					graphics::utilities::color::Crimson},
			color_pair_type{"Cyan",						graphics::utilities::color::Cyan},
			color_pair_type{"DarkBlue",					graphics::utilities::color::DarkBlue},
			color_pair_type{"DarkCyan",					graphics::utilities::color::DarkCyan},
			color_pair_type{"DarkGoldenrod",			graphics::utilities::color::DarkGoldenrod},
			color_pair_type{"DarkGray",					graphics::utilities::color::DarkGray},
			color_pair_type{"DarkGrey",					graphics::utilities::color::DarkGray},
			color_pair_type{"DarkGreen",				graphics::utilities::color::DarkGreen},
			color_pair_type{"DarkKhaki",				graphics::utilities::color::DarkKhaki},
			color_pair_type{"DarkMagenta",				graphics::utilities::color::DarkMagenta},
			color_pair_type{"DarkOliveGreen",			graphics::utilities::color::DarkOliveGreen},
			color_pair_type{"DarkOrange",				graphics::utilities::color::DarkOrange},
			color_pair_type{"DarkOrchid",				graphics::utilities::color::DarkOrchid},
			color_pair_type{"DarkRed",					graphics::utilities::color::DarkRed},
			color_pair_type{"DarkSalmon",				graphics::utilities::color::DarkSalmon},
			color_pair_type{"DarkSeaGreen",				graphics::utilities::color::DarkSeaGreen},
			color_pair_type{"DarkSlateBlue",			graphics::utilities::color::DarkSlateBlue},
			color_pair_type{"DarkSlateGray",			graphics::utilities::color::DarkSlateGray},
			color_pair_type{"DarkSlateGrey",			graphics::utilities::color::DarkSlateGray},
			color_pair_type{"DarkTurquoise",			graphics::utilities::color::DarkTurquoise},
			color_pair_type{"DarkViolet",				graphics::utilities::color::DarkViolet},
			color_pair_type{"DeepPink",					graphics::utilities::color::DeepPink},
			color_pair_type{"DeepSkyBlue",				graphics::utilities::color::DeepSkyBlue},
			color_pair_type{"DimGray",					graphics::utilities::color::DimGray},
			color_pair_type{"DimGrey",					graphics::utilities::color::DimGray},
			color_pair_type{"DodgerBlue",				graphics::utilities::color::DodgerBlue},
			color_pair_type{"Firebrick",				graphics::utilities::color::Firebrick},
			color_pair_type{"FloralWhite",				graphics::utilities::color::FloralWhite},
			color_pair_type{"ForestGreen",				graphics::utilities::color::ForestGreen},
			color_pair_type{"Fuchsia",					graphics::utilities::color::Fuchsia},
			color_pair_type{"Gainsboro",				graphics::utilities::color::Gainsboro},
			color_pair_type{"GhostWhite",				graphics::utilities::color::GhostWhite},
			color_pair_type{"Gold",						graphics::utilities::color::Gold},
			color_pair_type{"Goldenrod",				graphics::utilities::color::Goldenrod},
			color_pair_type{"Gray",						graphics::utilities::color::Gray},
			color_pair_type{"Grey",						graphics::utilities::color::Gray},
			color_pair_type{"Green",					graphics::utilities::color::Green},
			color_pair_type{"GreenYellow",				graphics::utilities::color::GreenYellow},
			color_pair_type{"Honeydew",					graphics::utilities::color::Honeydew},
			color_pair_type{"HotPink",					graphics::utilities::color::HotPink},
			color_pair_type{"IndianRed",				graphics::utilities::color::IndianRed},
			color_pair_type{"Indigo",					graphics::utilities::color::Indigo},
			color_pair_type{"Ivory",					graphics::utilities::color::Ivory},
			color_pair_type{"Khaki",					graphics::utilities::color::Khaki},
			color_pair_type{"Lavender",					graphics::utilities::color::Lavender},
			color_pair_type{"LavenderBlush",			graphics::utilities::color::LavenderBlush},
			color_pair_type{"LawnGreen",				graphics::utilities::color::LawnGreen},
			color_pair_type{"LemonChiffon",				graphics::utilities::color::LemonChiffon},
			color_pair_type{"LightBlue",				graphics::utilities::color::LightBlue},
			color_pair_type{"LightCoral",				graphics::utilities::color::LightCoral},
			color_pair_type{"LightCyan",				graphics::utilities::color::LightCyan},
			color_pair_type{"LightGoldenrodYellow",		graphics::utilities::color::LightGoldenrodYellow},
			color_pair_type{"LightGray",				graphics::utilities::color::LightGray},
			color_pair_type{"LightGrey",				graphics::utilities::color::LightGray},
			color_pair_type{"LightGreen",				graphics::utilities::color::LightGreen},
			color_pair_type{"LightPink",				graphics::utilities::color::LightPink},
			color_pair_type{"LightSalmon",				graphics::utilities::color::LightSalmon},
			color_pair_type{"LightSeaGreen",			graphics::utilities::color::LightSeaGreen},
			color_pair_type{"LightSkyBlue",				graphics::utilities::color::LightSkyBlue},
			color_pair_type{"LightSlateGray",			graphics::utilities::color::LightSlateGray},
			color_pair_type{"LightSlateGrey",			graphics::utilities::color::LightSlateGray},
			color_pair_type{"LightSteelBlue",			graphics::utilities::color::LightSteelBlue},
			color_pair_type{"LightYellow",				graphics::utilities::color::LightYellow},
			color_pair_type{"Lime",						graphics::utilities::color::Lime},
			color_pair_type{"LimeGreen",				graphics::utilities::color::LimeGreen},
			color_pair_type{"Linen",					graphics::utilities::color::Linen},
			color_pair_type{"Magenta",					graphics::utilities::color::Magenta},
			color_pair_type{"Maroon",					graphics::utilities::color::Maroon},
			color_pair_type{"MediumAquamarine",			graphics::utilities::color::MediumAquamarine},
			color_pair_type{"MediumBlue",				graphics::utilities::color::MediumBlue},
			color_pair_type{"MediumOrchid",				graphics::utilities::color::MediumOrchid},
			color_pair_type{"MediumPurple",				graphics::utilities::color::MediumPurple},
			color_pair_type{"MediumSeaGreen",			graphics::utilities::color::MediumSeaGreen},
			color_pair_type{"MediumSlateBlue",			graphics::utilities::color::MediumSlateBlue},
			color_pair_type{"MediumSpringGreen",		graphics::utilities::color::MediumSpringGreen},
			color_pair_type{"MediumTurquoise",			graphics::utilities::color::MediumTurquoise},
			color_pair_type{"MediumVioletRed",			graphics::utilities::color::MediumVioletRed},
			color_pair_type{"MidnightBlue",				graphics::utilities::color::MidnightBlue},
			color_pair_type{"MintCream",				graphics::utilities::color::MintCream},
			color_pair_type{"MistyRose",				graphics::utilities::color::MistyRose},
			color_pair_type{"Moccasin",					graphics::utilities::color::Moccasin},
			color_pair_type{"NavajoWhite",				graphics::utilities::color::NavajoWhite},
			color_pair_type{"Navy",						graphics::utilities::color::Navy},
			color_pair_type{"OldLace",					graphics::utilities::color::OldLace},
			color_pair_type{"Olive",					graphics::utilities::color::Olive},
			color_pair_type{"OliveDrab",				graphics::utilities::color::OliveDrab},
			color_pair_type{"Orange",					graphics::utilities::color::Orange},
			color_pair_type{"OrangeRed",				graphics::utilities::color::OrangeRed},
			color_pair_type{"Orchid",					graphics::utilities::color::Orchid},
			color_pair_type{"PaleGoldenrod",			graphics::utilities::color::PaleGoldenrod},
			color_pair_type{"PaleGreen",				graphics::utilities::color::PaleGreen},
			color_pair_type{"PaleTurquoise",			graphics::utilities::color::PaleTurquoise},
			color_pair_type{"PaleVioletRed",			graphics::utilities::color::PaleVioletRed},
			color_pair_type{"PapayaWhip",				graphics::utilities::color::PapayaWhip},
			color_pair_type{"PeachPuff",				graphics::utilities::color::PeachPuff},
			color_pair_type{"Peru",						graphics::utilities::color::Peru},
			color_pair_type{"Pink",						graphics::utilities::color::Pink},
			color_pair_type{"Plum",						graphics::utilities::color::Plum},
			color_pair_type{"PowderBlue",				graphics::utilities::color::PowderBlue},
			color_pair_type{"Purple",					graphics::utilities::color::Purple},
			color_pair_type{"RebeccaPurple",			graphics::utilities::color::RebeccaPurple},
			color_pair_type{"Red",						graphics::utilities::color::Red},
			color_pair_type{"RosyBrown",				graphics::utilities::color::RosyBrown},
			color_pair_type{"RoyalBlue",				graphics::utilities::color::RoyalBlue},
			color_pair_type{"SaddleBrown",				graphics::utilities::color::SaddleBrown},
			color_pair_type{"Salmon",					graphics::utilities::color::Salmon},
			color_pair_type{"SandyBrown",				graphics::utilities::color::SandyBrown},
			color_pair_type{"SeaGreen",					graphics::utilities::color::SeaGreen},
			color_pair_type{"SeaShell",					graphics::utilities::color::SeaShell},
			color_pair_type{"Sienna",					graphics::utilities::color::Sienna},
			color_pair_type{"Silver",					graphics::utilities::color::Silver},
			color_pair_type{"SkyBlue",					graphics::utilities::color::SkyBlue},
			color_pair_type{"SlateBlue",				graphics::utilities::color::SlateBlue},
			color_pair_type{"SlateGray",				graphics::utilities::color::SlateGray},
			color_pair_type{"SlateGrey",				graphics::utilities::color::SlateGray},
			color_pair_type{"Snow",						graphics::utilities::color::Snow},
			color_pair_type{"SpringGreen",				graphics::utilities::color::SpringGreen},
			color_pair_type{"SteelBlue",				graphics::utilities::color::SteelBlue},
			color_pair_type{"Tan",						graphics::utilities::color::Tan},
			color_pair_type{"Teal",						graphics::utilities::color::Teal},
			color_pair_type{"Thistle",					graphics::utilities::color::Thistle},
			color_pair_type{"Tomato",					graphics::utilities::color::Tomato},
			color_pair_type{"Turquoise",				graphics::utilities::color::Turquoise},
			color_pair_type{"Violet",					graphics::utilities::color::Violet},
			color_pair_type{"Wheat",					graphics::utilities::color::Wheat},
			color_pair_type{"White",					graphics::utilities::color::White},
			color_pair_type{"WhiteSmoke",				graphics::utilities::color::WhiteSmoke},
			color_pair_type{"Yellow",					graphics::utilities::color::Yellow},
			color_pair_type{"YellowGreen",				graphics::utilities::color::YellowGreen},

			//Clashing W3C color names
			color_pair_type{"WebGray",					graphics::utilities::color::web::Gray},
			color_pair_type{"WebGrey",					graphics::utilities::color::web::Gray},
			color_pair_type{"WebGreen",					graphics::utilities::color::web::Green},
			color_pair_type{"WebMaroon",				graphics::utilities::color::web::Maroon},
			color_pair_type{"WebPurple",				graphics::utilities::color::web::Purple}
		};


		constexpr auto is_control_character(char c) noexcept
		{
			return c >= '\0' && c < ' ' ||
				   c == '\x7F'; //Delete character (127)
		}

		constexpr auto parse_as_floating_point(std::string_view str) noexcept
		{
			for (auto c : str)
			{
				switch (c)
				{
					//Exponent
					case 'e':
					case 'E':

					//Decimal point
					case '.':
					return true;
				}
			}

			return false;
		}

		constexpr auto parse_as_integer(std::string_view str) noexcept
		{
			return !parse_as_floating_point(str);
		}


		std::optional<graphics::utilities::Color> hex_as_color(std::string_view str) noexcept;
		std::optional<graphics::utilities::Color> rgb_as_color(std::string str) noexcept;
		std::optional<graphics::utilities::Color> color_name_as_color(std::string_view str) noexcept;

		std::optional<bool> string_as_boolean(std::string_view str) noexcept;
		std::optional<graphics::utilities::Color> string_as_color(std::string str) noexcept;
		std::optional<graphics::utilities::Vector2> string_as_vector2(std::string str) noexcept;
		std::optional<std::string> string_literal_as_string(std::string_view str);
	} //detail


	/*
		String as boolean
	*/

	//Parses the given string as a boolean value
	//Supports 1 or 0, true or false, yes or no and on or off
	[[nodiscard]] std::optional<bool> AsBoolean(std::string_view str) noexcept;


	/*
		String as color
	*/

	//Parses the given string as a color value
	//Supports r,g,b,(a) values in range [0, 255] or percentages in range [0.0, 1.0]
	//Supports hexadecimal values (with alpha) prefixed with # or 0x (both short and long notation)
	//Supports X11 color names such as AliceBlue, Crimson etc.
	[[nodiscard]] std::optional<graphics::utilities::Color> AsColor(std::string str) noexcept;


	/*
		String literal as string
	*/

	//Parses the given string literal as a string value
	//Supports double " and single ' quoted literals
	//Supports escape characters
	[[nodiscard]] std::optional<std::string> AsString(std::string_view str);


	/*
		String as vector2
	*/

	//Parses the given string as a vector2 value
	//Supports both scalar and x,y values
	[[nodiscard]] std::optional<graphics::utilities::Vector2> AsVector2(std::string str) noexcept;

} //ion::script::utilities::parse

#endif