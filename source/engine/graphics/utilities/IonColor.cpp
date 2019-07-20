/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/utilities
File:	IonColor.cpp
-------------------------------------------
*/

#include "IonColor.h"
#include <cmath>

namespace ion::graphics::utilities
{

using namespace color;
using namespace ion::utilities;

namespace color::detail
{

std::tuple<real, real, real> hue_to_rgb(real hue, real c, real x) noexcept
{
	if (hue < 60.0_r)
		return std::tuple{c, x, 0.0_r};
	else if (hue < 120.0_r)
		return std::tuple{x, c, 0.0_r};

	else if (hue < 180.0_r)
		return std::tuple{0.0_r, c, x};
	else if (hue < 240.0_r)
		return std::tuple{0.0_r, x, c};

	else if (hue < 300.0_r)
		return std::tuple{x, 0.0_r, c};
	else //less than 360.0
		return std::tuple{c, 0.0_r, x};
}

real rgb_to_hue(real red, real green, real blue, real c_max, real delta) noexcept
{
	if (delta == 0.0_r)
		return 0.0_r;

	real hue = [=]() noexcept
		{
			if (c_max == red)
				return (green - blue) / delta;
			else if (c_max == green)
				return (blue - red) / delta + 2.0_r;
			else //blue is max
				return (red - green) / delta + 4.0_r;
		}() * 60.0_r;

	return hue < 0.0_r ?
		360.0_r + hue :
		hue;
}

std::tuple<real, real, real> cmyk_to_rgb(real cyan, real magenta, real yellow, real black) noexcept
{
	auto k_inv = 1.0_r - black;
	return std::tuple{(1.0_r - cyan) * k_inv,
					  (1.0_r - magenta) * k_inv,
					  (1.0_r - yellow) * k_inv};
}

std::tuple<real, real, real> hsl_to_rgb(real hue, real saturation, real lightness) noexcept
{
	auto c = (1.0_r - math::Abs(2.0_r * lightness - 1.0_r)) * saturation;
	auto x = c * (1.0_r - math::Abs(std::fmod(hue / 60.0_r, 2.0_r) - 1.0_r));
	auto m = lightness - c / 2.0_r;

	auto [r, g, b] = hue_to_rgb(hue, c, x);
	return std::tuple{r + m, g + m, b + m};
}

std::tuple<real, real, real> hsv_to_rgb(real hue, real saturation, real value) noexcept
{
	auto c = value * saturation;
	auto x = c * (1.0_r - math::Abs(std::fmod(hue / 60.0_r, 2.0_r) - 1.0_r));
	auto m = value - c;

	auto [r, g, b] = hue_to_rgb(hue, c, x);
	return std::tuple{r + m, g + m, b + m};
}

std::tuple<real, real, real> hwb_to_rgb(real hue, real whiteness, real blackness) noexcept
{
	return hsv_to_rgb(hue, 1.0_r - whiteness / (1.0_r - blackness), 1.0_r - blackness);
}

std::tuple<real, real, real> ycbcr_to_rgb(real luma, real blue_difference, real red_difference) noexcept
{
	return std::tuple{luma + 1.402_r * (red_difference - 128.0_r),
					  luma - 0.344136_r * (blue_difference - 128.0_r) - 0.714136_r * (red_difference - 128.0_r),
					  luma + 1.772_r * (blue_difference - 128.0_r)};
}


std::tuple<real, real, real, real> rgb_to_cmyk(real red, real green, real blue) noexcept
{
	auto k = 1.0_r - std::max({red, green, blue});
	auto k_inv = 1.0_r - k;
	return std::tuple{(1.0_r - red - k) / k_inv,
					  (1.0_r - green - k) / k_inv,
					  (1.0_r - blue - k) / k_inv,
					  k};
}

std::tuple<real, real, real> rgb_to_hsl(real red, real green, real blue) noexcept
{
	auto [c_min, c_max] = std::minmax({red, green, blue});
	auto delta = c_max - c_min;

	auto h = rgb_to_hue(red, green, blue, c_max, delta);
	auto l = (c_min + c_max) / 2.0_r;
	auto s = (delta == 0.0_r) ?
		0.0_r :
		delta / (1.0_r - math::Abs(2.0_r * l - 1.0_r));

	return std::tuple{h, s, l};
}

std::tuple<real, real, real> rgb_to_hsv(real red, real green, real blue) noexcept
{
	auto [c_min, c_max] = std::minmax({red, green, blue});
	auto delta = c_max - c_min;

	auto h = rgb_to_hue(red, green, blue, c_max, delta);
	auto s = (c_max == 0.0_r) ?
		0.0_r :
		delta / c_max;

	return std::tuple{h, s, c_max};
}

std::tuple<real, real, real> rgb_to_hwb(real red, real green, real blue) noexcept
{
	auto [h, s, v] = rgb_to_hsv(red, green, blue);
	return std::tuple{h, (h - s) * v, 1.0_r - v};
}

std::tuple<real, real, real> rgb_to_ycbcr(real red, real green, real blue) noexcept
{
	return std::tuple{0.299_r *red + 0.587_r * green + 0.114_r * blue,
					  -0.168736_r * red - 0.331264_r * green + 0.5_r * blue + 128.0_r,
					  0.5_r * red - 0.418688_r * green - 0.081312_r * blue  + 128.0_r};
}

} //color::detail


//Color

Color::Color(real red, real green, real blue, real alpha) noexcept :
	red_{math::Saturate(red)},
	green_{math::Saturate(green)},
	blue_{math::Saturate(blue)},
	alpha_{math::Saturate(alpha)}
{
	//Empty
}


/*
	Static color conversions
*/

Color Color::CMYK(real cyan, real magenta, real yellow, real black, real alpha) noexcept
{
	cyan = math::Saturate(cyan);
	magenta = math::Saturate(magenta);
	yellow = math::Saturate(yellow);
	black = math::Saturate(black);

	auto [r, g, b] = detail::cmyk_to_rgb(cyan, magenta, yellow, black);
	return {r, g, b, alpha};
}

Color Color::Hex(uint32 value) noexcept
{
	return {detail::hex_to_red(value),
			detail::hex_to_green(value),
			detail::hex_to_blue(value),
			detail::hex_to_alpha(value)};
}

Color Color::Hex(uint32 value, real alpha) noexcept
{
	return {detail::hex_to_red(value),
			detail::hex_to_green(value),
			detail::hex_to_blue(value),
			alpha};
}

Color Color::HSL(real hue, real saturation, real lightness, real alpha) noexcept
{
	//Make sure hue is inside range [0.0, 360.0)
	hue = std::fmod(hue, 360.0_r);
	if (hue < 0.0_r)
		hue = 360.0_r - hue;

	saturation = math::Saturate(saturation);
	lightness = math::Saturate(lightness);

	auto [r, g, b] = detail::hsl_to_rgb(hue, saturation, lightness);
	return {r, g, b, alpha};
}

Color Color::HSV(real hue, real saturation, real value, real alpha) noexcept
{
	//Make sure hue is inside range [0.0, 360.0)
	hue = std::fmod(hue, 360.0_r);
	if (hue < 0.0_r)
		hue = 360.0_r - hue;

	saturation = math::Saturate(saturation);
	value = math::Saturate(value);

	auto [r, g, b] = detail::hsv_to_rgb(hue, saturation, value);
	return {r, g, b, alpha};
}

Color Color::HWB(real hue, real whiteness, real blackness, real alpha) noexcept
{
	//Make sure hue is inside range [0.0, 360.0)
	hue = std::fmod(hue, 360.0_r);
	if (hue < 0.0_r)
		hue = 360.0_r - hue;
	
	//If sum is greater than 1.0, scale back whiteness and blackness proportionally
	if (auto sum = whiteness + blackness;
		sum > 1.0_r)
	{
		whiteness /= sum;
		blackness /= sum;
	}
	
	auto [r, g, b] = detail::hwb_to_rgb(hue, whiteness, blackness);
	return {r, g, b, alpha};
}

Color Color::RGB(uint8 red, uint8 green, uint8 blue, real alpha) noexcept
{
	return {red / 255.0_r,
			green / 255.0_r,
			blue / 255.0_r,
			alpha};
}

Color Color::YCbCr(uint8 luma, uint8 blue_difference, uint8 red_difference, real alpha) noexcept
{
	auto [r, g, b] = detail::ycbcr_to_rgb(luma, blue_difference, red_difference);
	return {r / 255.0_r,
			g / 255.0_r,
			b / 255.0_r,
			alpha};
}


/*
	Color conversions
*/

std::tuple<real, real, real, real> Color::ToCMYK() const noexcept
{
	return detail::rgb_to_cmyk(red_, green_, blue_);
}

uint32 Color::ToHex24() const noexcept
{
	return detail::rgb_to_hex24(
		static_cast<uint8>(math::Round(red_ * 255.0_r)),
		static_cast<uint8>(math::Round(green_ * 255.0_r)),
		static_cast<uint8>(math::Round(blue_ * 255.0_r)));
}

uint32 Color::ToHex32() const noexcept
{
	return detail::rgb_to_hex32(
		static_cast<uint8>(math::Round(red_ * 255.0_r)),
		static_cast<uint8>(math::Round(green_ * 255.0_r)),
		static_cast<uint8>(math::Round(blue_ * 255.0_r)),
		static_cast<uint8>(math::Round(alpha_ * 255.0_r)));
}

std::tuple<real, real, real> Color::ToHSL() const noexcept
{
	return detail::rgb_to_hsl(red_, green_, blue_);
}

std::tuple<real, real, real> Color::ToHSV() const noexcept
{
	return detail::rgb_to_hsv(red_, green_, blue_);
}

std::tuple<real, real, real> Color::ToHWB() const noexcept
{
	return detail::rgb_to_hwb(red_, green_, blue_);
}

std::tuple<int, int, int> Color::ToRGB() const noexcept
{
	return {static_cast<int>(math::Round(red_ * 255.0_r)),
			static_cast<int>(math::Round(green_ * 255.0_r)),
			static_cast<int>(math::Round(blue_ * 255.0_r))};
}

std::tuple<int, int, int> Color::ToYCbCr() const noexcept
{
	auto [y, cb, cr] = detail::rgb_to_ycbcr(red_ * 255.0_r, green_ * 255.0_r, blue_ * 255.0_r);
	return {static_cast<int>(math::Round(y)),
			static_cast<int>(math::Round(cb)),
			static_cast<int>(math::Round(cr))};
}


/*
	Inversing
*/
			
Color& Color::Inverse() noexcept
{
	return *this = InverseCopy();
}

Color Color::InverseCopy() const noexcept
{
	return {1.0_r - red_,
			1.0_r - green_,
			1.0_r - blue_,
			alpha_};
}


/*
	Mixing
*/

Color& Color::Mix(const Color &with_color, real amount) noexcept
{
	return *this = MixCopy(with_color, amount);
}

Color Color::MixCopy(const Color &with_color, real amount) const noexcept
{
	amount = math::Saturate(amount);
	return {math::Lerp(red_, with_color.red_, amount),
			math::Lerp(green_, with_color.green_, amount),
			math::Lerp(blue_, with_color.blue_, amount),
			math::Lerp(alpha_, with_color.alpha_, amount)};
}

} //ion::graphics::utilities