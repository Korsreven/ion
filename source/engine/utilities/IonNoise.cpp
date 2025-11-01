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
	return t * t * t * (t * (t * 6.0_r - 15.0_r) + 10.0_r);
}


real grad(int hash, real x) noexcept
{
	return (hash & 1) ? x : -x;
}

std::pair<real, real> grad2(int hash, real x) noexcept
{
    switch (hash & 7)
    {
        case 0: return {x, x};
        case 1: return {-x, x};
        case 2: return {x, -x};
        case 3: return {-x, -x};
        case 4: return {x, 0.0_r};
        case 5: return {-x, 0.0_r};
        case 6: return {0.0_r, x};
        default: return {0.0_r, -x};
    }
}

real value(int hash, real x) noexcept
{
	//Map 32 bits of hash into a deterministic floating point
    return ((hash & 0x7fffffff) / 2147483647.0_r * 2.0_r - 1.0_r) * x;
}


int hash(int x) noexcept
{
	x = (x << 13) ^ x;
	return (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff;
}

int hash(int x, int y) noexcept
{
    return hash(x + hash(y));
}

} //detail


/* 
	Perlin functions
*/

real Perlin1D(real x) noexcept
{
	auto x0 = static_cast<int>(std::floor(x)); //Left coordinate
	auto x1 = x0 + 1; //Right coordinate

	auto sx = x - x0; //Distance from x0
	auto u = detail::fade(sx); //Smooth interpolation weight

	//Get gradient directions based on hashed coordinates
	auto g0 = detail::grad(detail::hash(x0));
	auto g1 = detail::grad(detail::hash(x1));

	//Distance vectors (from lattice point to input coordinate)
	auto d0 = sx;
	auto d1 = sx - 1.0_r;

	//Dot product between gradient and distance
	auto v0 = g0 * d0;
	auto v1 = g1 * d1;

	//Interpolate between the two values
	return math::Lerp(v0, v1, u);
}

real Perlin2D(real x, real y) noexcept
{
	//Determine grid cell coordinates
	auto x0 = static_cast<int>(std::floor(x));
	auto y0 = static_cast<int>(std::floor(y));
	auto x1 = x0 + 1;
	auto y1 = y0 + 1;

	//Relative coordinates within cell
	auto sx = x - x0;
	auto sy = y - y0;

	//Smooth interpolation weights
	auto u = detail::fade(sx);
	auto v = detail::fade(sy);

	//Hash each corner to get gradient directions
	auto [gx00, gy00] = detail::grad2(detail::hash(x0, y0));
	auto [gx10, gy10] = detail::grad2(detail::hash(x1, y0));
	auto [gx01, gy01] = detail::grad2(detail::hash(x0, y1));
	auto [gx11, gy11] = detail::grad2(detail::hash(x1, y1));

	//Distance vectors from each corner to point
	auto dx0 = sx;
	auto dy0 = sy;
    auto dx1 = sx - 1.0_r;
	auto dy1 = sy;
    
	//Dot products between gradients and distance vectors
	auto v00 = gx00 * dx0 + gy00 * dy0;
	auto v10 = gx10 * dx1 + gy10 * dy0;
	auto v01 = gx01 * dx0 + gy01 * dy1;
	auto v11 = gx11 * dx1 + gy11 * dy1;

	//Interpolate along x
	auto ix0 = math::Lerp(v00, v10, u);
	auto ix1 = math::Lerp(v01, v11, u);

	//Interpolate along y
	return math::Lerp(ix0, ix1, v);
}


/*
	Simplex functions
*/

real Simplex2D(real x, real y) noexcept
{
	//Skew the input space to determine which simplex cell we are in
	auto s = (x + y) * detail::simplex_f2;
	auto i = static_cast<int>(std::floor(x + s));
	auto j = static_cast<int>(std::floor(y + s));

	//Unskew the cell origin back to (x, y) space
	auto t = (i + j) * detail::simplex_g2;
	auto X0 = i - t;
	auto Y0 = j - t;

	//Distance from cell origin
	auto x0 = x - X0;
	auto y0 = y - Y0;

	//Determine which simplex triangle we are in
	auto i1 = x0 > y0 ? 1 : 0; //Lower triangle
	auto j1 = x0 > y0 ? 0 : 1; //Upper triangle

	//Offsets for the other corners
	auto x1 = x0 - i1 + detail::simplex_g2;
	auto y1 = y0 - j1 + detail::simplex_g2;
	auto x2 = x0 - 1.0_r + 2.0_r * detail::simplex_g2;
	auto y2 = y0 - 1.0_r + 2.0_r * detail::simplex_g2;

	//Hash lattice corners
	auto h0 = detail::hash(i, j);
	auto h1 = detail::hash(i + i1, j + j1);
	auto h2 = detail::hash(i + 1, j + 1);

	//Gradients from your existing grad2()
	auto [gx0, gy0] = detail::grad2(h0);
	auto [gx1, gy1] = detail::grad2(h1);
	auto [gx2, gy2] = detail::grad2(h2);

	//Compute contribution from each corner
	auto contrib =
		[](real t, real gx, real gy, real x, real y) noexcept
		{
			if (t < 0.0_r)
				return 0.0_r;
			
			t *= t;
			return t * t * (gx * x + gy * y);
		};

	auto t0 = 0.5_r - x0 * x0 - y0 * y0;
	auto n0 = contrib(t0, gx0, gy0, x0, y0);

	auto t1 = 0.5_r - x1 * x1 - y1 * y1;
	auto n1 = contrib(t1, gx1, gy1, x1, y1);

	auto t2 = 0.5_r - x2 * x2 - y2 * y2;
	auto n2 = contrib(t2, gx2, gy2, x2, y2);

	//Scale to roughly [-1.0, 1.0]
	return 70.0_r * (n0 + n1 + n2);
}


/*
	Value functions
*/

real Value1D(real x) noexcept
{
	auto x0 = static_cast<int>(std::floor(x)); //Left coordinate
	auto x1 = x0 + 1; //Right coordinate

	auto sx = x - x0; //Distance from x0
	auto u = detail::fade(sx); //Smooth interpolation weight

	//Get random scalar values at lattice points
	auto v0 = detail::value(detail::hash(x0));
	auto v1 = detail::value(detail::hash(x1));

	//Interpolate between the two values
	return math::Lerp(v0, v1, u);
}

real Value2D(real x, real y) noexcept
{
	//Determine grid cell coordinates
	auto x0 = static_cast<int>(std::floor(x));
	auto y0 = static_cast<int>(std::floor(y));
	auto x1 = x0 + 1;
	auto y1 = y0 + 1;

	//Relative coordinates within cell
	auto sx = x - x0;
	auto sy = y - y0;

	//Smooth interpolation weights
	auto u = detail::fade(sx);
	auto v = detail::fade(sy);

	//Get random scalar values at the four lattice corners
	auto v00 = detail::value(detail::hash(x0, y0));
	auto v10 = detail::value(detail::hash(x1, y0));
	auto v01 = detail::value(detail::hash(x0, y1));
	auto v11 = detail::value(detail::hash(x1, y1));

	//Interpolate along x
	auto ix0 = math::Lerp(v00, v10, u);
	auto ix1 = math::Lerp(v01, v11, u);

	//Interpolate along y
	return math::Lerp(ix0, ix1, v);
}

} //ion::utilities::noise