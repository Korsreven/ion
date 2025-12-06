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

#include <array>

#include "types/IonTypes.h"

///@brief Namespace containing commonly used noise functions for terrain and pattern generation
namespace ion::utilities::noise
{
	using namespace types::type_literals;

	namespace detail
	{
		//Standard permutation table (Ken Perlin’s original)
		constexpr auto perm = std::array
		{
			151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
			140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
			247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
			 57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
			 74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
			 60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
			 65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
			200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
			 52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
			207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
			119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
			129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
			218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
			 81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
			184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
			222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180,

			151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
			140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
			247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
			 57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
			 74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
			 60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
			 65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
			200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
			 52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
			207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
			119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
			129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
			218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
			 81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
			184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
			222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180
		};


		//Constants are defined with quadruple precision (128 bit)

		constexpr auto f2 = 0.366025403784438646763723170752936183_r; //(sqrt(3) - 1) / 2
		constexpr auto g2 = 0.211324865405187117745425609749021272_r; //(3 - sqrt(3)) / 6
		constexpr auto f3 = 1.0_r / 3.0_r;
		constexpr auto g3 = 1.0_r / 6.0_r;


		real fade(real t) noexcept;

		real grad(int hash, real x) noexcept;
		real grad(int hash, real x, real y) noexcept;
		real grad(int hash, real x, real y, real z) noexcept;
	} //detail


	/**
		@name Perlin functions
		@{
	*/

	///@brief Returns the classic perlin noise for the given x coordinate
	///@details The result is in range [-1.0, 1.0]
	[[nodiscard]] real Perlin(real x) noexcept;

	///@brief Returns the classic perlin noise for the given x and y coordinates
	///@details The result is in range [-1.0, 1.0]
	[[nodiscard]] real Perlin(real x, real y) noexcept;

	///@brief Returns the classic perlin noise for the given x, y and z coordinates
	///@details The result is in range [-1.0, 1.0]
	[[nodiscard]] real Perlin(real x, real y, real z) noexcept;

	///@}

	/**
		@name Simplex functions
		@{
	*/

	///@brief Returns the simplex noise for the given x coordinate
	///@details The result is in range [-1.0, 1.0]
	[[nodiscard]] real Simplex(real x) noexcept;

	///@brief Returns the simplex noise for the given x and y coordinates
	///@details The result is in range [-1.0, 1.0]
	[[nodiscard]] real Simplex(real x, real y) noexcept;

	///@brief Returns the simplex noise for the given x, y and z coordinates
	///@details The result is in range [-1.0, 1.0]
	[[nodiscard]] real Simplex(real x, real y, real z) noexcept;

	///@}
} //ion::utilities::noise

#endif