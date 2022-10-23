/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonColor.h
-------------------------------------------
*/

#ifndef ION_COLOR_H
#define ION_COLOR_H

#include <algorithm>
#include <cassert>
#include <tuple>

#include "types/IonTypes.h"
#include "utilities/IonMath.h"

#pragma warning(disable : 4201) //nameless struct/union

#undef RGB
#undef CMYK

namespace ion::graphics::utilities
{
	using namespace types::type_literals;

	namespace color::detail
	{
		constexpr auto max_hex_three_digit = 0xfff_ui32;
		constexpr auto max_hex_four_digit = 0xffff_ui32;
		constexpr auto max_hex_triplet = 0xffffff_ui32;
		constexpr auto max_hex_quartet = 0xffffffff_ui32;


		constexpr auto hex_to_red(uint32 value) noexcept
		{
			if (value > max_hex_triplet)
				return (value >> 24) / 255.0_r; //Hex quartet (rgba)
			else if (value > max_hex_four_digit)
				return (value >> 16) / 255.0_r; //Hex triplet (rgb)

			//Shorthand
			else if (value > max_hex_three_digit)
				return (((value >> 12) << 4) | (value >> 12)) / 255.0_r; //Four-digit (rgba)
			else
				return (((value >> 8) << 4) | (value >> 8)) / 255.0_r; //Three-digit (rgb)
		}

		constexpr auto hex_to_green(uint32 value) noexcept
		{
			if (value > max_hex_triplet)
				return (value >> 16 & 0xff) / 255.0_r; //Hex quartet (rgba)
			else if (value > max_hex_four_digit)
				return (value >> 8 & 0xff) / 255.0_r; //Hex triplet (rgb)

			//Shorthand
			else if (value > max_hex_three_digit)
				return (((value >> 8 & 0xf) << 4) | (value >> 8 & 0xf)) / 255.0_r; //Four-digit (rgba)
			else
				return (((value >> 4 & 0xf) << 4) | (value >> 4 & 0xf)) / 255.0_r; //Three-digit (rgb)
		}

		constexpr auto hex_to_blue(uint32 value) noexcept
		{
			if (value > max_hex_triplet)
				return (value >> 8 & 0xff) / 255.0_r; //Hex quartet (rgba)
			else if (value > max_hex_four_digit)
				return (value & 0xff) / 255.0_r; //Hex triplet (rgb)

			//Shorthand
			else if (value > max_hex_three_digit)
				return (((value >> 4 & 0xf) << 4) | (value >> 4 & 0xf)) / 255.0_r; //Four-digit (rgba)
			else
				return (((value & 0xf) << 4) | (value & 0xf)) / 255.0_r; //Three-digit (rgb)
		}

		constexpr auto hex_to_alpha(uint32 value) noexcept
		{
			if (value > max_hex_triplet)
				return (value & 0xff) / 255.0_r; //Hex quartet (rgba)
			else if (value > max_hex_four_digit)
				return 1.0_r; //Hex triplet (rgb)

			//Shorthand
			else if (value > max_hex_three_digit)
				return (((value & 0xf) << 4) | (value & 0xf)) / 255.0_r; //Four-digit (rgba)
			else
				return 1.0_r; //Three-digit (rgb)
		}


		constexpr auto rgb_to_hex24(uint8 red, uint8 green, uint8 blue) noexcept
		{
			return ((red & 0xff) << 16) + ((green & 0xff) << 8) + (blue & 0xff);
		}

		constexpr auto rgb_to_hex32(uint8 red, uint8 green, uint8 blue, uint8 alpha) noexcept
		{
			return ((red & 0xff) << 24) + ((green & 0xff) << 16) + ((blue & 0xff) << 8) + (alpha & 0xff);
		}


		std::tuple<real, real, real> hue_to_rgb(real hue, real c, real x) noexcept;
		real rgb_to_hue(real red, real green, real blue, real c_max, real delta) noexcept;

		std::tuple<real, real, real> cmyk_to_rgb(real cyan, real magenta, real yellow, real black) noexcept;
		std::tuple<real, real, real> hsl_to_rgb(real hue, real saturation, real lightness) noexcept;
		std::tuple<real, real, real> hsv_to_rgb(real hue, real saturation, real value) noexcept;
		std::tuple<real, real, real> hwb_to_rgb(real hue, real whiteness, real blackness) noexcept;
		std::tuple<real, real, real> ycbcr_to_rgb(real brightness, real u, real v) noexcept;

		std::tuple<real, real, real, real> rgb_to_cmyk(real red, real green, real blue) noexcept;
		std::tuple<real, real, real> rgb_to_hsl(real red, real green, real blue) noexcept;
		std::tuple<real, real, real> rgb_to_hsv(real red, real green, real blue) noexcept;
		std::tuple<real, real, real> rgb_to_hwb(real red, real green, real blue) noexcept;
		std::tuple<real, real, real> rgb_to_ycbcr(real red, real green, real blue) noexcept;
	} //color::detail


	//A class representing a four-channel RGBA32 color model
	class Color final
	{
		private:

			union
			{
				struct
				{
					real red_;
					real green_;
					real blue_;
					real alpha_;
				};

				real channels_[4]{};
			};

		public:

			Color() = default;

			//Constructs a new color from the given RGBA percentages in range [0.0, 1.0]
			Color(real red, real green, real blue, real alpha = 1.0_r) noexcept;


			/*
				Static color conversions
			*/

			//Returns a new color from the given CMYK values
			//The given values should be in range [0.0, 1.0]
			[[nodiscard]] static Color CMYK(real cyan, real magenta, real yellow, real black, real alpha = 1.0_r) noexcept;

			//Returns a new color from the given hex value
			//A hex triplet value contains only the RGB channels (the alpha channel is set to 1.0)
			//A hex quartet value contains all channels in order RGBA
			//A shorthand hex value contains three or four-digit, one for each channel (0xff0 -> 0xffff00)
			[[nodiscard]] static Color Hex(uint32 value) noexcept;

			//Returns a new color from the given hex and alpha values
			//Take only the RGB channels from the given hex value (triplet, quartet or shorthand)
			//The alpha channel is set to the given alpha instead
			[[nodiscard]] static Color Hex(uint32 value, real alpha) noexcept;

			//Returns a new color from the given HSL values
			//Hue should be degrees in range [0.0, 360.0)
			//Saturation, lightness and alpha should be in range [0.0, 1.0]
			[[nodiscard]] static Color HSL(real hue, real saturation, real lightness, real alpha = 1.0_r) noexcept;

			//Returns a new color from the given HSV/HSB values
			//Hue should be degrees in range [0.0, 360.0)
			//Saturation, value and alpha should be in range [0.0, 1.0]
			[[nodiscard]] static Color HSV(real hue, real saturation, real value, real alpha = 1.0_r) noexcept;

			//Returns a new color from the given HWB values
			//Hue should be degrees in range [0.0, 360.0)
			//Whiteness, blackness and alpha should be in range [0.0, 1.0]
			[[nodiscard]] static Color HWB(real hue, real whiteness, real blackness, real alpha = 1.0_r) noexcept;

			//Returns a new color from the given RGB values
			//Red, green and blue should be in range [0, 255]
			//Alpha should be in range [0.0, 1.0]
			[[nodiscard]] static Color RGB(uint8 red, uint8 green, uint8 blue, real alpha = 1.0_r) noexcept;

			//Returns a new color from the given YCbCr values
			//Luma, blue difference and red difference should be in range [0, 255]
			//Alpha should be in range [0.0, 1.0]
			[[nodiscard]] static Color YCbCr(uint8 luma, uint8 blue_difference, uint8 red_difference, real alpha = 1.0_r) noexcept;


			/*
				Operators
			*/

			//Checks if two colors are equal (all channels are equal)
			[[nodiscard]] inline auto operator==(const Color &rhs) const noexcept
			{
				return red_ == rhs.red_ &&
					   green_ == rhs.green_ &&
					   blue_ == rhs.blue_ &&
					   alpha_ == rhs.alpha_;
			}

			//Checks if two colors are different (one or more channels are different)
			[[nodiscard]] inline auto operator!=(const Color &rhs) const noexcept
			{
				return !(*this == rhs);
			}

			//Checks if one color is less than another one (channel wise)
			//Needed for sorting two colors (strict weak ordering)
			[[nodiscard]] inline auto operator<(const Color &rhs) const noexcept
			{
				return std::tuple{red_, green_, blue_, alpha_} < std::tuple{rhs.red_, rhs.green_, rhs.blue_, rhs.alpha_};
			}


			/*
				Adding
			*/

			//Adds two colors (adding each channel)
			[[nodiscard]] inline auto operator+(const Color &rhs) const noexcept
			{
				return Color{red_ + rhs.red_,
							 green_ + rhs.green_,
							 blue_ + rhs.blue_,
							 alpha_ + rhs.alpha_};
			}

			//Adds two colors (adding each channel)
			inline auto& operator+=(const Color &rhs) noexcept
			{
				return *this = *this + rhs;
			}


			/*
				Subtracting
			*/

			//Subtracts two colors (subtracting each channel)
			[[nodiscard]] inline auto operator-(const Color &rhs) const noexcept
			{
				return Color{red_ - rhs.red_,
							 green_ - rhs.green_,
							 blue_ - rhs.blue_,
							 alpha_ - rhs.alpha_};
			}

			//Subtracts two colors (subtracting each channel)
			inline auto& operator-=(const Color &rhs) noexcept
			{
				return *this = *this - rhs;
			}


			/*
				Multiplying
			*/

			//Muliplies all channels with the given scalar
			[[nodiscard]] inline auto operator*(real scalar) const noexcept
			{
				return Color{red_ * scalar,
							 green_ * scalar,
							 blue_ * scalar,
							 alpha_ * scalar};
			}

			//Muliplies all channels with the given scalar
			[[nodiscard]] friend inline auto operator*(real scalar, const Color &rhs) noexcept
			{
				return rhs * scalar; //Commutative
			}

			//Muliplies all channels with the given scalar
			inline auto& operator*=(real scalar) noexcept
			{
				return *this = *this * scalar;
			}


			/*
				Dividing
			*/

			//Divides all channels with the given scalar
			[[nodiscard]] inline auto operator/(real scalar) const noexcept
			{
				assert(scalar != 0.0_r);
				return *this * (1.0_r / scalar);
			}

			//Divides all channels with the given scalar
			inline auto& operator/=(real scalar) noexcept
			{
				return *this = *this / scalar;
			}


			/*
				Subscripting
			*/

			//Returns the channel at the given offset as percentage in range [0.0, 1.0]
			[[nodiscard]] inline auto operator[](int off) const noexcept
			{
				assert(off >= 0 && off < 4);
				return channels_[off];
			}


			/*
				Modifiers
			*/
			
			//Sets the red channel to the given percentage in range [0.0, 1.0]
			inline void R(real red) noexcept
			{
				red_ = ion::utilities::math::Saturate(red);
			}

			//Sets the green channel to the given percentage in range [0.0, 1.0]
			inline void G(real green) noexcept
			{
				green_ = ion::utilities::math::Saturate(green);
			}

			//Sets the blue channel to the given percentage in range [0.0, 1.0]
			inline void B(real blue) noexcept
			{
				blue_ = ion::utilities::math::Saturate(blue);
			}

			//Sets the alpha channel to the given percentage in range [0.0, 1.0]
			inline void A(real alpha) noexcept
			{
				alpha_ = ion::utilities::math::Saturate(alpha);
			}


			/*
				Observers
			*/

			//Returns the red channel as percentage in range [0.0, 1.0]
			[[nodiscard]] inline auto R() const noexcept
			{
				return red_;
			}

			//Returns the green channel as percentage in range [0.0, 1.0]
			[[nodiscard]] inline auto G() const noexcept
			{
				return green_;
			}

			//Returns the blue channel as percentage in range [0.0, 1.0]
			[[nodiscard]] inline auto B() const noexcept
			{
				return blue_;
			}

			//Returns the alpha channel as percentage in range [0.0, 1.0]
			[[nodiscard]] inline auto A() const noexcept
			{
				return alpha_;
			}	

			//Returns the red, green and blue channels as percentages in range [0.0, 1.0]
			[[nodiscard]] inline auto RGB() const noexcept
			{
				return std::tuple{red_, green_, blue_};
			}

			//Returns the red, green, blue and alpha channels as percentages in range [0.0, 1.0]
			[[nodiscard]] inline auto RGBA() const noexcept
			{
				return std::tuple{red_, green_, blue_, alpha_};
			}

			//Returns direct access to all color channels as percentages in range [0.0, 1.0]
			[[nodiscard]] inline auto Channels() const noexcept
			{
				return channels_;
			}


			/*
				Color conversions
			*/

			//Returns color converted to CMYK values
			//All values returned are in range [0.0, 1.0]
			[[nodiscard]] std::tuple<real, real, real, real> ToCMYK() const noexcept;

			//Returns color converted to a 24 bit hex value (triplet)
			//The returned hex value contains only the RGB channels
			[[nodiscard]] uint32 ToHex24() const noexcept;

			//Returns color converted to a 32 bit hex value (quartet)
			//The returned hex value contains all channels in order RGBA
			[[nodiscard]] uint32 ToHex32() const noexcept;

			//Returns color converted to HSL values
			//Returns hue in range [0.0, 360.0)
			//Returns saturation and lightness in range [0.0, 1.0]
			[[nodiscard]] std::tuple<real, real, real> ToHSL() const noexcept;

			//Returns color converted to HSV/HSB values
			//Returns hue in range [0.0, 360.0)
			//Returns saturation and value in range [0.0, 1.0]
			[[nodiscard]] std::tuple<real, real, real> ToHSV() const noexcept;

			//Returns color converted to HWB values
			//Returns hue in range [0.0, 360.0)
			//Returns whiteness and blackness in range [0.0, 1.0]
			[[nodiscard]] std::tuple<real, real, real> ToHWB() const noexcept;

			//Returns color converted to RGB values
			//All values returned are in range [0, 255]
			[[nodiscard]] std::tuple<int, int, int> ToRGB() const noexcept;

			//Returns color converted to YCbCr values
			//All values returned are in range [0, 255]
			[[nodiscard]] std::tuple<int, int, int> ToYCbCr() const noexcept;


			/*
				Inversing
			*/
			
			//Inverse the color by inversing the RGB channels
			//The alpha channel is not inversed
			Color& Inverse() noexcept;

			//Inverse the color by inversing the RGB channels
			//The alpha channel is not inversed
			//Returns the result as a copy
			[[nodiscard]] Color InverseCopy() const noexcept;


			/*
				Mixing
			*/

			//Mix the color with another color by the given amount in range [0.0, 1.0]
			//Default amount is 0.5 (50%)
			Color& Mix(const Color &with_color, real amount = 0.5_r) noexcept;

			//Mix the color with another color by the given amount in range [0.0, 1.0]
			//Default amount is 0.5 (50%)
			//Returns the result as a copy
			[[nodiscard]] Color MixCopy(const Color &with_color, real amount = 0.5_r) const noexcept;
	};


	namespace color
	{
		/*
			Predefined constant colors
		*/

		inline const auto Transparent = Color::RGB(0, 0, 0, 0.0_r);


		/*
			X11 color names standardized by SVG 1.1 (second edition)
			Names are also supported in CSS by all major browsers (W3C colors)

			Note: Gray, green, maroon and purple have clashing definitions with W3C
			Therefore the clashing W3C colors are prefixed with web

			Note: RebeccaPurple was added 21st of June, 2014
		*/

		inline const auto AliceBlue = Color::RGB(240, 248, 255);
		inline const auto AntiqueWhite = Color::RGB(250, 235, 215);
		inline const auto Aqua = Color::RGB(0, 255, 255);
		inline const auto Aquamarine = Color::RGB(127, 255, 212);
		inline const auto Azure = Color::RGB(240, 255, 255);
		inline const auto Beige = Color::RGB(245, 245, 220);
		inline const auto Bisque = Color::RGB(255, 228, 196);
		inline const auto Black = Color::RGB(0, 0, 0);
		inline const auto BlanchedAlmond = Color::RGB(255, 235, 205);
		inline const auto Blue = Color::RGB(0, 0, 255);
		inline const auto BlueViolet = Color::RGB(138, 43, 226);
		inline const auto Brown = Color::RGB(165, 42, 42);
		inline const auto BurlyWood = Color::RGB(222, 184, 135);
		inline const auto CadetBlue = Color::RGB(95, 158, 160);
		inline const auto Chartreuse = Color::RGB(127, 255, 0);
		inline const auto Chocolate = Color::RGB(210, 105, 30);
		inline const auto Coral = Color::RGB(255, 127, 80);
		inline const auto CornflowerBlue = Color::RGB(100, 149, 237);
		inline const auto Cornsilk = Color::RGB(255, 248, 220);
		inline const auto Crimson = Color::RGB(220, 20, 60);
		inline const auto Cyan = Color::RGB(0, 255, 255);
		inline const auto DarkBlue = Color::RGB(0, 0, 139);
		inline const auto DarkCyan = Color::RGB(0, 139, 139);
		inline const auto DarkGoldenrod = Color::RGB(184, 134, 11);
		inline const auto DarkGray = Color::RGB(169, 169, 169);
		inline const auto DarkGreen = Color::RGB(0, 100, 0);
		inline const auto DarkKhaki = Color::RGB(189, 183, 107);
		inline const auto DarkMagenta = Color::RGB(139, 0, 139);
		inline const auto DarkOliveGreen = Color::RGB(85, 107, 47);
		inline const auto DarkOrange = Color::RGB(255, 140, 0);
		inline const auto DarkOrchid = Color::RGB(153, 50, 204);
		inline const auto DarkRed = Color::RGB(139, 0, 0);
		inline const auto DarkSalmon = Color::RGB(233, 150, 122);
		inline const auto DarkSeaGreen = Color::RGB(143, 188, 142);
		inline const auto DarkSlateBlue = Color::RGB(72, 61, 139);
		inline const auto DarkSlateGray = Color::RGB(47, 79, 79);
		inline const auto DarkTurquoise = Color::RGB(0, 206, 209);
		inline const auto DarkViolet = Color::RGB(148, 0, 211);
		inline const auto DeepPink = Color::RGB(255, 20, 147);
		inline const auto DeepSkyBlue = Color::RGB(0, 191, 255);
		inline const auto DimGray = Color::RGB(105, 105, 105);
		inline const auto DodgerBlue = Color::RGB(30, 144, 255);
		inline const auto Firebrick = Color::RGB(178, 34, 34);
		inline const auto FloralWhite = Color::RGB(255, 250, 240);
		inline const auto ForestGreen = Color::RGB(34, 139, 34);
		inline const auto Fuchsia = Color::RGB(255, 0, 255);
		inline const auto Gainsboro = Color::RGB(220, 220, 220);
		inline const auto GhostWhite = Color::RGB(248, 248, 248);
		inline const auto Gold = Color::RGB(255, 215, 0);
		inline const auto Goldenrod = Color::RGB(218, 165, 32);
		inline const auto Gray = Color::RGB(190, 190, 190);
		inline const auto Green = Color::RGB(0, 255, 0);
		inline const auto GreenYellow = Color::RGB(173, 255, 47);
		inline const auto Honeydew = Color::RGB(240, 255, 240);
		inline const auto HotPink = Color::RGB(255, 105, 180);
		inline const auto IndianRed = Color::RGB(205, 92, 92);
		inline const auto Indigo = Color::RGB(75, 0, 130);
		inline const auto Ivory = Color::RGB(255, 255, 240);
		inline const auto Khaki = Color::RGB(240, 230, 140);
		inline const auto Lavender = Color::RGB(230, 230, 250);
		inline const auto LavenderBlush = Color::RGB(255, 240, 245);
		inline const auto LawnGreen = Color::RGB(124, 252, 0);
		inline const auto LemonChiffon = Color::RGB(255, 250, 205);
		inline const auto LightBlue = Color::RGB(173, 216, 230);
		inline const auto LightCoral = Color::RGB(240, 128, 128);
		inline const auto LightCyan = Color::RGB(224, 255, 255);
		inline const auto LightGoldenrodYellow = Color::RGB(250, 250, 210);
		inline const auto LightGray = Color::RGB(211, 211, 211);
		inline const auto LightGreen = Color::RGB(144, 238, 144);
		inline const auto LightPink = Color::RGB(255, 182, 193);
		inline const auto LightSalmon = Color::RGB(255, 160, 122);
		inline const auto LightSeaGreen = Color::RGB(32, 178, 170);
		inline const auto LightSkyBlue = Color::RGB(135, 206, 250);
		inline const auto LightSlateGray = Color::RGB(119, 136, 153);
		inline const auto LightSteelBlue = Color::RGB(176, 196, 222);
		inline const auto LightYellow = Color::RGB(255, 255, 224);
		inline const auto Lime = Color::RGB(0, 255, 0);
		inline const auto LimeGreen = Color::RGB(50, 205, 50);
		inline const auto Linen = Color::RGB(250, 240, 230);
		inline const auto Magenta = Color::RGB(255, 0, 255);
		inline const auto Maroon = Color::RGB(176, 48, 96);
		inline const auto MediumAquamarine = Color::RGB(102, 205, 170);
		inline const auto MediumBlue = Color::RGB(0, 0, 205);
		inline const auto MediumOrchid = Color::RGB(186, 85, 211);
		inline const auto MediumPurple = Color::RGB(147, 112, 219);
		inline const auto MediumSeaGreen = Color::RGB(60, 179, 113);
		inline const auto MediumSlateBlue = Color::RGB(123, 104, 238);
		inline const auto MediumSpringGreen = Color::RGB(0, 250, 154);
		inline const auto MediumTurquoise = Color::RGB(72, 209, 204);
		inline const auto MediumVioletRed = Color::RGB(199, 21, 133);
		inline const auto MidnightBlue = Color::RGB(25, 25, 112);
		inline const auto MintCream = Color::RGB(245, 255, 250);
		inline const auto MistyRose = Color::RGB(255, 228, 225);
		inline const auto Moccasin = Color::RGB(255, 228, 181);
		inline const auto NavajoWhite = Color::RGB(255, 222, 173);
		inline const auto Navy = Color::RGB(0, 0, 128);
		inline const auto OldLace = Color::RGB(253, 245, 230);
		inline const auto Olive = Color::RGB(128, 128, 0);
		inline const auto OliveDrab = Color::RGB(107, 142, 35);
		inline const auto Orange = Color::RGB(255, 165, 0);
		inline const auto OrangeRed = Color::RGB(255, 69, 0);
		inline const auto Orchid = Color::RGB(218, 112, 214);
		inline const auto PaleGoldenrod = Color::RGB(238, 232, 170);
		inline const auto PaleGreen = Color::RGB(152, 251, 152);
		inline const auto PaleTurquoise = Color::RGB(175, 238, 238);
		inline const auto PaleVioletRed = Color::RGB(219, 112, 147);
		inline const auto PapayaWhip = Color::RGB(255, 239, 213);
		inline const auto PeachPuff = Color::RGB(255, 218, 185);
		inline const auto Peru = Color::RGB(205, 133, 63);
		inline const auto Pink = Color::RGB(255, 192, 203);
		inline const auto Plum = Color::RGB(221, 160, 221);
		inline const auto PowderBlue = Color::RGB(176, 224, 230);
		inline const auto Purple = Color::RGB(160, 32, 240);
		inline const auto RebeccaPurple = Color::RGB(102, 51, 153);
		inline const auto Red = Color::RGB(255, 0, 0);
		inline const auto RosyBrown = Color::RGB(188, 143, 143);
		inline const auto RoyalBlue = Color::RGB(65, 105, 225);
		inline const auto SaddleBrown = Color::RGB(139, 69, 19);
		inline const auto Salmon = Color::RGB(250, 128, 114);
		inline const auto SandyBrown = Color::RGB(244, 164, 96);
		inline const auto SeaGreen = Color::RGB(46, 139, 87);
		inline const auto SeaShell = Color::RGB(255, 245, 238);
		inline const auto Sienna = Color::RGB(160, 82, 45);
		inline const auto Silver = Color::RGB(192, 192, 192);
		inline const auto SkyBlue = Color::RGB(135, 206, 235);
		inline const auto SlateBlue = Color::RGB(106, 90, 205);
		inline const auto SlateGray = Color::RGB(112, 128, 144);
		inline const auto Snow = Color::RGB(255, 250, 250);
		inline const auto SpringGreen = Color::RGB(0, 255, 127);
		inline const auto SteelBlue = Color::RGB(70, 130, 180);
		inline const auto Tan = Color::RGB(210, 180, 140);
		inline const auto Teal = Color::RGB(0, 128, 128);
		inline const auto Thistle = Color::RGB(216, 191, 216);
		inline const auto Tomato = Color::RGB(255, 99, 71);
		inline const auto Turquoise = Color::RGB(64, 244, 208);
		inline const auto Violet = Color::RGB(238, 130, 238);
		inline const auto Wheat = Color::RGB(245, 222, 179);
		inline const auto White = Color::RGB(255, 255, 255);
		inline const auto WhiteSmoke = Color::RGB(245, 245, 245);
		inline const auto Yellow = Color::RGB(255, 255, 0);
		inline const auto YellowGreen = Color::RGB(154, 205, 50);


		namespace web
		{
			/*
				W3C color names
				Clashing definitions with X11 color names
			*/

			inline const auto Gray = Color::RGB(128, 128, 128);
			inline const auto Green = Color::RGB(0, 128, 0);
			inline const auto Maroon = Color::RGB(128, 0, 0);
			inline const auto Purple = Color::RGB(128, 0, 128);
		} //web


		inline namespace literals
		{
			/*
				User defined literals (UDLs)
				For hex triplet (rgb) and quartet (rgba) value to color
				For shorthand hex three-digit (rgb) and four-digit (rgba) value to color
			*/

			inline auto operator""_rgb(unsigned long long value) noexcept
			{
				assert((value > detail::max_hex_four_digit && value <= detail::max_hex_triplet) ||
					   value <= detail::max_hex_three_digit); //Shorthand
				return Color::Hex(static_cast<uint32>(value));
			}

			inline auto operator""_rgba(unsigned long long value) noexcept
			{
				assert((value > detail::max_hex_triplet && value <= detail::max_hex_quartet) ||
					   (value > detail::max_hex_three_digit && value <= detail::max_hex_four_digit)); //Shorthand
				return Color::Hex(static_cast<uint32>(value));
			}
		} //literals
	} //color
} //ion::graphics::utilities

#endif