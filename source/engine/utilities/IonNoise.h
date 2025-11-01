/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonNoise.h
-------------------------------------------
*/

#ifndef ION_NOISE_H
#define ION_NOISE_H

#include <utility>

#include "types/IonTypes.h"

///@brief Namespace containing commonly used noise functions for terrain and pattern generation
namespace ion::utilities::noise
{
	using namespace types::type_literals;

	namespace detail
	{
		//Constants are defined with quadruple precision (128 bit)

		constexpr auto simplex_f2 = 0.366025403784438646763723170752936183_r; //(sqrt(3) - 1) / 2
		constexpr auto simplex_g2 = 0.211324865405187117745425609749021272_r; //(3 - sqrt(3)) / 6


		real fade(real t) noexcept;

		real grad(int hash, real x = 1.0_r) noexcept;
		std::pair<real, real> grad2(int hash, real x = 1.0_r) noexcept;
		real value(int hash, real x = 1.0_r) noexcept;

		int hash(int x) noexcept;
		int hash(int x, int y) noexcept;	
	} //detail


	/**
		@name Perlin functions
		@{
	*/

	///@brief Returns the perlin noise for the given x coordinate
	///@details The result is in range [-1.0, 1.0]
	[[nodiscard]] real Perlin1D(real x) noexcept;

	///@brief Returns the perlin noise for the given x and y coordinates
	///@details The result is in range [-1.0, 1.0]
	[[nodiscard]] real Perlin2D(real x, real y) noexcept;

	///@}

	/**
		@name Simplex functions
		@{
	*/

	///@brief Returns the simplex noise for the given x and y coordinates
	///@details The result is in range [-1.0, 1.0]
	[[nodiscard]] real Simplex2D(real x, real y) noexcept;

	///@}

	/**
		@name Value functions
		@{
	*/

	///@brief Returns the value noise for the given x coordinate
	///@details The result is in range [-1.0, 1.0]
	[[nodiscard]] real Value1D(real x) noexcept;

	///@brief Returns the value noise for the given x and y coordinates
	///@details The result is in range [-1.0, 1.0]
	[[nodiscard]] real Value2D(real x, real y) noexcept;

	///@}
} //ion::utilities::noise

#endif