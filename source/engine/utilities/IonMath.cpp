/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonMath.cpp
-------------------------------------------
*/

#include <cmath>

#include "IonMath.h"

namespace ion::utilities::math
{

namespace detail
{

std::array<real, trigonometric_tables::size> trigonometric_tables::GenerateSineTable() noexcept
{
	std::array<real, size> table;
	std::generate(std::begin(table), std::end(table),
		[off = 0]() mutable noexcept
		{
			return std::sin(two_pi * off++ / size);
		});

	return table;
}

std::array<real, trigonometric_tables::size> trigonometric_tables::GenerateTangentTable() noexcept
{
	std::array<real, size> table;
	std::generate(std::begin(table), std::end(table),
		[off = 0]() mutable noexcept
		{
			return std::tan(two_pi * off++ / size);
		});

	return table;
}

int trigonometric_tables::TableIndex(real radian) noexcept
{
	auto index = static_cast<int>(Abs(radian) * factor) % size;

	if (radian < 0.0_r)
		index = size - index - 1;

	return index;
}

void trigonometric_tables::Initialize() noexcept
{
	SineTable();
	TangentTable();
}

real trigonometric_tables::Sine(real radian) noexcept
{
	return SineTable()[TableIndex(radian)];
}

real trigonometric_tables::Cosine(real radian) noexcept
{
	return SineTable()[TableIndex(radian + half_pi)];
}

real trigonometric_tables::Tangent(real radian) noexcept
{
	return TangentTable()[TableIndex(radian)];
}

} //detail


/*
	Fractions and rounding
*/

real Fraction(real x) noexcept
{
	return x - (x < 0.0_r ? std::ceil(x) : std::floor(x));
}

real Round(real x) noexcept
{
	return Round(x, 0);
}

real Round(real x, int precision) noexcept
{
	real factor = std::pow(10.0_r, static_cast<real>(precision));
	x *= factor;

	return	Abs(Fraction(x)) < 0.5_r ?
			//Round down
			x < 0.0_r ?
				std::ceil(x) / factor : std::floor(x) / factor :
			//Round up
			x < 0.0_r ?
				std::floor(x) / factor : std::ceil(x) / factor;
}

real Truncate(real x) noexcept
{
	return Truncate(x, 0);
}

real Truncate(real x, int precision) noexcept
{
	real factor = std::pow(10.0_r, static_cast<real>(precision));
	x *= factor;

	return (x < 0.0_r ?
			//Round up
			std::ceil(x) / factor :
			//Round down
			std::floor(x) / factor);
}


/*
	Interpolation
*/

real Slerp(real x, real y, real omega, real amount) noexcept
{
	return x * Sin(omega * (1.0_r - amount)) +
			y * Sin(omega * amount) / Sin(omega);
}


/*
	Trigonometry
*/

real Sin(real radian) noexcept
{
	return detail::trigonometric_tables::Sine(radian);
}

real Cos(real radian) noexcept
{
	return detail::trigonometric_tables::Cosine(radian);
}

real Tan(real radian) noexcept
{
	return detail::trigonometric_tables::Tangent(radian);
}

} //ion::utilities::math