/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonNoise.cpp
-------------------------------------------
*/

#include "IonNoise.h"

#include <cmath>
#include "IonMath.h"

namespace ion::utilities::noise
{

namespace detail
{

real fade(real t) noexcept
{
	//6t^5 - 15t^4 + 10t^3
	return t * t * t * (t * (t * 6 - 15) + 10);
}


real grad(int hash, real x) noexcept
{
	return (hash & 1) == 0 ? x : -x;
}

real grad(int hash, real x, real y) noexcept
{
	auto h = hash & 7;
	auto u = h < 4 ? x : y;
	auto v = h < 4 ? y : x;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

real grad(int hash, real x, real y, real z) noexcept
{
	auto h = hash & 15;
    auto u = h < 8 ? x : y;
    auto v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

} //detail


/* 
	Perlin functions
*/

real Perlin(real x) noexcept
{
	//Determine the unit segment containing the coordinate
	auto i = static_cast<int>(std::floor(x));

	//Relative coordinate within segment
	auto x0 = x - i;

	//Wrap coordinate for permutation table
	auto ii = i & 255;

	//Compute fade curves for coordinate
	auto u = detail::fade(x0);

	//Permute coordinate for gradient lookup
	auto a = detail::perm[ii];

	//Gradient contributions from segment endpoints
	auto v0 = detail::grad(detail::perm[a], x0);
	auto v1 = detail::grad(detail::perm[a + 1], x0 - 1.0_r);

	//Interpolate along x
	return math::Lerp(v0, v1, u);
}

real Perlin(real x, real y) noexcept
{
	//Determine the unit square containing the coordinate
	auto i = static_cast<int>(std::floor(x));
	auto j = static_cast<int>(std::floor(y));

	//Relative coordinates within square
	auto x0 = x - i;
	auto y0 = y - j;

	//Wrap coordinates for permutation table
	auto ii = i & 255;
	auto jj = j & 255;

	//Compute fade curves for each coordinate
	auto u = detail::fade(x0);
	auto v = detail::fade(y0);

	//Permute coordinates for gradient lookup
	auto a = detail::perm[ii] + jj;
	auto b = detail::perm[ii + 1] + jj;

	//Gradient contributions from each corners
	auto v00 = detail::grad(detail::perm[a], x0, y0); //Bottom-left
	auto v10 = detail::grad(detail::perm[b], x0 - 1.0_r, y0); //Bottom-right
	auto v01 = detail::grad(detail::perm[a + 1], x0, y0 - 1.0_r); //Top-left
	auto v11 = detail::grad(detail::perm[b + 1], x0 - 1.0_r, y0 - 1.0_r); //Top-right

	//Interpolate along x
	auto ix0 = math::Lerp(v00, v10, u);
	auto ix1 = math::Lerp(v01, v11, u);

	//Interpolate along y
	return math::Lerp(ix0, ix1, v);
}

real Perlin(real x, real y, real z) noexcept
{
	//Determine the unit cube containing the coordinate
	auto i = static_cast<int>(std::floor(x));
	auto j = static_cast<int>(std::floor(y));
	auto k = static_cast<int>(std::floor(z));

	//Relative coordinates within cube
	auto x0 = x - i;
	auto y0 = y - j;
	auto z0 = z - k;

	//Wrap coordinates for permutation table
	auto ii = i & 255;
	auto jj = j & 255;
	auto kk = k & 255;

	//Compute fade curves for each coordinate
	auto u = detail::fade(x0);
	auto v = detail::fade(y0);
	auto w = detail::fade(z0);

	//Permute coordinates for gradient lookup
	auto a = detail::perm[ii] + jj;
	auto aa = detail::perm[a] + kk;
	auto ab = detail::perm[a + 1] + kk;
	auto b = detail::perm[ii + 1] + jj;
	auto ba = detail::perm[b] + kk;
	auto bb = detail::perm[b + 1] + kk;

	//Gradient contributions from each corners
	//Front face
	auto v000 = detail::grad(detail::perm[aa], x0, y0, z0); //Bottom-left
	auto v100 = detail::grad(detail::perm[ba], x0 - 1.0_r, y0, z0); //Bottom-right
	auto v010 = detail::grad(detail::perm[ab], x0, y0 - 1.0_r, z0); //Top-left
	auto v110 = detail::grad(detail::perm[bb], x0 - 1.0_r, y0 - 1.0_r, z0); //Top-right
	//Back face
	auto v001 = detail::grad(detail::perm[aa + 1], x0, y0, z0 - 1.0_r); //Bottom-left
	auto v101 = detail::grad(detail::perm[ba + 1], x0 - 1.0_r, y0, z0 - 1.0_r); //Bottom-right
	auto v011 = detail::grad(detail::perm[ab + 1], x0, y0 - 1.0_r, z0 - 1.0_r); //Top-left
	auto v111 = detail::grad(detail::perm[bb + 1], x0 - 1.0_r, y0 - 1.0_r, z0 - 1.0_r); //Top-right

	//Interpolate along x
	//Front face
	auto ix0 = math::Lerp(v000, v100, u);
	auto ix1 = math::Lerp(v010, v110, u);
	//Back face
	auto ix2 = math::Lerp(v001, v101, u);
	auto ix3 = math::Lerp(v011, v111, u);

	//Interpolate along y
	auto iy0 = math::Lerp(ix0, ix1, v);
	auto iy1 = math::Lerp(ix2, ix3, v);

	//Interpolate along z
	return math::Lerp(iy0, iy1, w);
}


/*
	Simplex functions
*/

real Simplex(real x) noexcept
{
	//Determine simplex cell
	auto i = static_cast<int>(std::floor(x));

	//Distance from cell origin
	auto x0 = x - i;

	//Endpoint offset
	auto x1 = x0 - 1.0_r;

	//Wrap coordinate for permutation table
	auto ii = i & 255;

	//Compute contribution from each endpoint
	auto contrib =
		[](real t, real x, int hash)
		{
			if (t < 0.0_r)
				return 0.0_r;

			t *= t;
			return t * t * detail::grad(hash, x);
		};

	auto n0 = contrib(1.0_r - x0 * x0, x0, detail::perm[ii]);
	auto n1 = contrib(1.0_r - x1 * x1, x1, detail::perm[ii + 1]);

	//Scale to roughly [-1.0, 1.0]
	return 0.395_r * (n0 + n1);
}

real Simplex(real x, real y) noexcept
{
	//Skew the input space to determine simplex cell
	auto s = (x + y) * detail::f2;
	auto i = static_cast<int>(std::floor(x + s));
	auto j = static_cast<int>(std::floor(y + s));

	//Unskew the cell origin back to (x, y) space
	auto t = (i + j) * detail::g2;

	//Distance from cell origin
	auto x0 = x - (i - t);
	auto y0 = y - (j - t);

	//Determine which simplex triangle we are in
	auto i1 = x0 > y0 ? 1 : 0; //Lower triangle
	auto j1 = x0 > y0 ? 0 : 1; //Upper triangle

	//Corner offsets
	auto x1 = x0 - i1 + detail::g2;
	auto y1 = y0 - j1 + detail::g2;
	auto x2 = x0 - 1.0_r + 2.0_r * detail::g2;
	auto y2 = y0 - 1.0_r + 2.0_r * detail::g2;

	//Wrap coordinates for permutation table
	auto ii = i & 255;
	auto jj = j & 255;

	//Compute contribution from each corner
	auto contrib =
		[](real t, real x, real y, int hash)
		{
			if (t < 0.0_r)
				return 0.0_r;
			
			t *= t;
			return t * t * detail::grad(hash, x, y);
		};

	auto n0 = contrib(0.5_r - x0 * x0 - y0 * y0, x0, y0,
		detail::perm[ii + detail::perm[jj]]);
	auto n1 = contrib(0.5_r - x1 * x1 - y1 * y1, x1, y1,
		detail::perm[ii + i1 + detail::perm[jj + j1]]);
	auto n2 = contrib(0.5_r - x2 * x2 - y2 * y2, x2, y2,
		detail::perm[ii + 1 + detail::perm[jj + 1]]);

	//Scale to roughly [-1.0, 1.0]
	return 70.0_r * (n0 + n1 + n2);
}

real Simplex(real x, real y, real z) noexcept
{
	//Skew the input space to determine simplex cell
	auto s = (x + y + z) * detail::f3;
	auto i = static_cast<int>(std::floor(x + s));
	auto j = static_cast<int>(std::floor(y + s));
	auto k = static_cast<int>(std::floor(z + s));

	//Unskew the cell origin back to (x, y, z) space
	auto t = (i + j + k) * detail::g3;

	//Distance from cell origin
	auto x0 = x - (i - t);
	auto y0 = y - (j - t);
	auto z0 = z - (k - t);

	//Determine simplex corner order
	auto i1 = 0;
	auto j1 = 0;
	auto k1 = 0;
	auto i2 = 0;
	auto j2 = 0;
	auto k2 = 0;

	if (x0 >= y0)
	{
		i2 = 1;

		if (y0 >= z0)
			i1 = j2 = 1;
		else if (x0 >= z0)
			i1 = k2 = 1;
		else
			k1 = k2 = 1;
	}
	else
	{
		j2 = 1;

		if (y0 < z0)
			k1 = k2 = 1;
		else if (x0 < z0)
			j1 = k2 = 1;
		else
			j1 = i2 = 1;
	}

	//Corner offsets
	auto x1 = x0 - i1 + detail::g3;
	auto y1 = y0 - j1 + detail::g3;
	auto z1 = z0 - k1 + detail::g3;
	auto x2 = x0 - i2 + 2.0_r * detail::g3;
	auto y2 = y0 - j2 + 2.0_r * detail::g3;
	auto z2 = z0 - k2 + 2.0_r * detail::g3;
	auto x3 = x0 - 1.0_r + 3.0_r * detail::g3;
	auto y3 = y0 - 1.0_r + 3.0_r * detail::g3;
	auto z3 = z0 - 1.0_r + 3.0_r * detail::g3;

	//Wrap coordinates for permutation table
	auto ii = i & 255;
	auto jj = j & 255;
	auto kk = k & 255;

	//Compute contribution from each corner
	auto contrib =
		[](real t, real x, real y, real z, int hash)
		{
			if (t < 0.0_r)
				return 0.0_r;

			t *= t;
			return t * t * detail::grad(hash, x, y, z);
		};

	auto n0 = contrib(0.6_r - x0 * x0 - y0 * y0 - z0 * z0, x0, y0, z0,
		detail::perm[ii + detail::perm[jj + detail::perm[kk]]]);
	auto n1 = contrib(0.6_r - x1 * x1 - y1 * y1 - z1 * z1, x1, y1, z1,
		detail::perm[ii + i1 + detail::perm[jj + j1 + detail::perm[kk + k1]]]);
	auto n2 = contrib(0.6_r - x2 * x2 - y2 * y2 - z2 * z2, x2, y2, z2,
		detail::perm[ii + i2 + detail::perm[jj + j2 + detail::perm[kk + k2]]]);
	auto n3 = contrib(0.6_r - x3 * x3 - y3 * y3 - z3 * z3, x3, y3, z3,
		detail::perm[ii + 1 + detail::perm[jj + 1 + detail::perm[kk + 1]]]);

	//Scale to roughly [-1.0, 1.0]
	return 32.0_r * (n0 + n1 + n2 + n3);
}

} //ion::utilities::noise