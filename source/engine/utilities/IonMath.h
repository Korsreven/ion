/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonMath.h
-------------------------------------------
*/

#ifndef _ION_MATH_
#define _ION_MATH_

#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <optional>
#include <type_traits>
#include <vector>

#include "adaptors/IonFlatMap.h"
#include "types/IonTypes.h"

namespace ion::utilities::math
{
	using namespace types::type_literals;

	namespace detail
	{
		//Constants are defined with quadruple precision (128 bit)

		constexpr auto pi = 3.141592653589793238462643383279502884_r;
		constexpr auto two_pi = 6.283185307179586476925286766559005768_r;
		constexpr auto half_pi = 1.570796326794896619231321691639751442_r;
		constexpr auto quarter_pi = 0.785398163397448309615660845819875721_r;
		
		constexpr auto degree_to_radian_factor = pi / 180.0_r;
		constexpr auto radian_to_degree_factor = 180.0_r / pi;


		class trigonometric_tables final
		{
			private:

				static constexpr auto size = 4'096;
				static constexpr auto factor = size / two_pi;


				static std::array<real, size> GenerateSineTable() noexcept;
				static std::array<real, size> GenerateTangentTable() noexcept;

				static int TableIndex(real radian) noexcept;

			public:

				static void Initialize() noexcept;

				static inline auto &SineTable() noexcept
				{
					static auto sine_table{GenerateSineTable()};
					return sine_table;
				}

				static inline auto &TangentTable() noexcept
				{
					static auto tangent_table{GenerateTangentTable()};
					return tangent_table;
				}

				static real Sine(real radian) noexcept;
				static real Cosine(real radian) noexcept;
				static real Tangent(real radian) noexcept;
		};


		template <typename Iterator>
		constexpr auto sum(Iterator first, Iterator last) noexcept
		{
			return std::accumulate(first, last, typename Iterator::value_type{});
		}

		template <typename Iterator>
		constexpr auto mean(Iterator first, Iterator last) noexcept
		{
			return sum(first, last) / static_cast<typename Iterator::value_type>(std::distance(first, last));
		}

		template <typename Iterator>
		/*constexpr*/ auto median(Iterator first, Iterator last) noexcept
		{
			auto count = std::distance(first, last);
			auto middle = first + count / 2;

			std::nth_element(first, middle, last);
			auto middle_value = *middle;

			//Even
			if (count % 2 == 0)
			{
				std::nth_element(first, middle - 1, last);
				return (*(middle - 1) + middle_value) / static_cast<typename Iterator::value_type>(2);
			}
			//Odd
			else
				return middle_value;
		}

		template <typename Iterator>
		/*constexpr*/ auto mode(Iterator first, Iterator last)
		{
			adaptors::FlatMap<typename Iterator::value_type, int> histogram;
			std::for_each(first, last,
				[&](auto &&value) mutable noexcept
				{
					++histogram[value]; //Create histogram
				});

			std::vector dataset(std::begin(histogram), std::end(histogram));
			std::stable_sort(std::begin(dataset), std::end(dataset),
				[](auto &&x, auto &&y) noexcept
				{
					return x.second > y.second; //Sort descending
				});

			auto last_in_set =
				std::find_if_not(std::begin(dataset) + 1, std::end(dataset),
					[&](auto &&value) noexcept
					{
						return value.second == dataset.front().second; //Find all modes (multimodal)
					});

			std::vector<typename Iterator::value_type> result;
			result.reserve(last_in_set - std::begin(dataset));
			std::for_each(std::begin(dataset), last_in_set,
				[&](auto &&value) mutable noexcept
				{
					result.push_back(value.first);
				});

			return result;
		}

		template <typename Iterator>
		constexpr auto range(Iterator first, Iterator last) noexcept
		{
			auto [min, max] = std::minmax_element(first, last);
			return *max - *min;
		}
	} //detail


	/*
		Floating point limits
	*/

	//Gives an upper bound on the relative error due to rounding in floating point arithmetic
	constexpr auto Epsilon = std::numeric_limits<real>::epsilon();


	/*
		Mathematical constants
	*/

	//Archimedes' constant pi, 3.14...
	//With the highest precision supported
	constexpr auto Pi = detail::pi;

	//Two pi, 6.28...
	//With the highest precision supported
	constexpr auto TwoPi = detail::two_pi;

	//Half pi, 1.57...
	//With the highest precision supported
	constexpr auto HalfPi = detail::half_pi;

	//Quarter pi, 0.78...
	//With the highest precision supported
	constexpr auto QuarterPi = detail::quarter_pi;


	/*
		Common functions implemented as constexpr
	*/

	//Returns the absolute value of a real number
	//Handles -0.0 properly
	[[nodiscard]] constexpr auto Abs(real x) noexcept
	{
		return x < 0.0_r ? -x :
				x > 0.0_r ? x :
				0.0_r;
	}

	//Check if two real numbers are equal within the given epsilon margin
	[[nodiscard]] constexpr auto Equal(real x, real y, real epsilon = Epsilon) noexcept
	{
		return Abs(x - y) < epsilon;
	}

	//Return the binary logarithm of x for any integral type
	template <typename T>
	[[nodiscard]] constexpr auto Log2(T x) noexcept
	{
		static_assert(std::is_integral_v<T>);
		auto result = T{0};

		for (; x > T{1}; ++result)
			x /= T{2};

		return result;
	}

	//Returns x normalized to range [0.0, 1.0]
	[[nodiscard]] constexpr auto Normalize(real x, real min, real max) noexcept
	{
		return (x - min) / (max - min);
	}

	//Return x raised to the y-th power for any integral type
	template <typename T>
	[[nodiscard]] constexpr auto Power(T x, T y) noexcept
	{
		static_assert(std::is_integral_v<T>);
		auto result = T{1};

		while (y)
		{
			if (y & T{1})
				result *= x;

			y >>= T{1};
			x *= x;
		}

		return result;
	}

	//Returns x clamped inside range [0.0, 1.0]
	[[nodiscard]] constexpr auto Saturate(real x) noexcept
	{
		return std::clamp(x, 0.0_r, 1.0_r);
	}

	//Returns the sign of x
	//-1 if negative, 1 if positive else 0
	[[nodiscard]] constexpr auto Sign(real x) noexcept
	{
		return (0.0_r < x) - (x < 0.0_r);
	}


	/*
		Fractions and rounding
	*/

	//Returns the fractional part of x
	//x - floor x	(for x >= 0)
	//x - ceil x	(for x < 0)
	[[nodiscard]] real Fraction(real x) noexcept;

	//Return x rounded to the nearest whole number
	//Half rounds away from zero
	[[nodiscard]] real Round(real x) noexcept;

	//Return x rounded to the nearest number with the given precision
	//Half rounds away from zero
	[[nodiscard]] real Round(real x, int precision) noexcept;

	//Return x truncated toward zero
	[[nodiscard]] real Truncate(real x) noexcept;

	//Return x truncated toward zero with the given precision
	[[nodiscard]] real Truncate(real x, int precision) noexcept;


	/*
		Interpolation
	*/

	//Linear interpolation
	//Interpolate between x and y with the given amount
	[[nodiscard]] constexpr auto Lerp(real x, real y, real amount) noexcept
	{
		return x + (y - x) * amount;
	}

	//Spherical linear interpolation
	//Interpolate between x and y with the given omega and amount
	[[nodiscard]] real Slerp(real x, real y, real omega, real amount) noexcept;


	/*
		Probability and statistics
	*/

	//Calculate n! for any integral type
	template <typename T>
	[[nodiscard]] constexpr auto Factorial(T n) noexcept
	{
		static_assert(std::is_integral_v<T>);
		auto result = T{1};

		while (n > T{1})
			result *= n--;

		return result;
	}

	//Calculate nCr for any integral type
	template <typename T>
	[[nodiscard]] constexpr auto Choose(T n, T r) noexcept
	{
		static_assert(std::is_integral_v<T>);

		if (r > n)
			return T{0};

		return Factorial(n) / (Factorial(r) * Factorial(n - r));
	}

	//Calculate nPr for any integral type
	template <typename T>
	[[nodiscard]] constexpr auto Permute(T n, T r) noexcept
	{
		static_assert(std::is_integral_v<T>);

		if (r > n)
			return T{0};

		return Factorial(n) / Factorial(n - r);
	}


	/*
		Sum and averages
	*/

	//Returns the sum of all numbers in the given range [first, last)
	//If range is empty, it returns nullopt
	template <typename Iterator>
	[[nodiscard]] constexpr auto Sum(Iterator first, Iterator last) noexcept
		-> std::optional<decltype(detail::sum(first, last))>
	{
		static_assert(std::is_base_of_v<std::input_iterator_tag,
					  typename std::iterator_traits<Iterator>::iterator_category>);
		
		if (first != last)
			return detail::sum(first, last);
		else
			return {};
	}

	//Returns the sum of all numbers in the given container
	//If container is empty, it returns nullopt
	template <typename Container>
	[[nodiscard]] constexpr auto Sum(const Container &container) noexcept
	{
		return Sum(std::begin(container), std::end(container));
	}


	//Returns the mean of all numbers in the given range [first, last)
	//If range is empty, it returns nullopt
	template <typename Iterator>
	[[nodiscard]] constexpr auto Mean(Iterator first, Iterator last) noexcept
		-> std::optional<decltype(detail::mean(first, last))>
	{
		static_assert(std::is_base_of_v<std::input_iterator_tag,
					  typename std::iterator_traits<Iterator>::iterator_category>);
		
		if (first != last)
			return detail::mean(first, last);
		else
			return {};
	}

	//Returns the mean of all numbers in the given container
	//If container is empty, it returns nullopt
	template <typename Container>
	[[nodiscard]] constexpr auto Mean(const Container &container) noexcept
	{
		return Mean(std::begin(container), std::end(container));
	}


	//Returns the median of all numbers in the given range [first, last)
	//If range is empty, it returns nullopt
	template <typename Iterator>
	[[nodiscard]] /*constexpr*/ auto Median(Iterator first, Iterator last) noexcept
		-> std::optional<decltype(detail::median(first, last))>
	{
		static_assert(std::is_base_of_v<std::random_access_iterator_tag,
					  typename std::iterator_traits<Iterator>::iterator_category>);

		if (first != last)
			return detail::median(first, last);
		else
			return {};
	}

	//Returns the median of all numbers in the given container
	//If container is empty, it returns nullopt
	template <typename Container>
	[[nodiscard]] /*constexpr*/ auto Median(Container &container) noexcept
	{
		return Median(std::begin(container), std::end(container));
	}


	//Returns the mode (multimodal) of all numbers in the given range [first, last)
	//If range is empty, it returns nullopt
	template <typename Iterator>
	[[nodiscard]] /*constexpr*/ auto Mode(Iterator first, Iterator last)
		-> std::optional<decltype(detail::mode(first, last))>
	{
		static_assert(std::is_base_of_v<std::forward_iterator_tag,
					  typename std::iterator_traits<Iterator>::iterator_category>);

		if (first != last)
			return detail::mode(first, last);
		else
			return {};
	}

	//Returns the mode (multimodal) of all numbers in the given container
	//If container is empty, it returns nullopt
	template <typename Container>
	[[nodiscard]] /*constexpr*/ auto Mode(const Container &container)
	{
		return Mode(std::begin(container), std::end(container));
	}


	//Returns the range of all numbers in the given range [first, last)
	//If range is empty, it returns nullopt
	template <typename Iterator>
	[[nodiscard]] constexpr auto Range(Iterator first, Iterator last) noexcept
		-> std::optional<decltype(detail::range(first, last))>
	{
		static_assert(std::is_base_of_v<std::forward_iterator_tag,
					  typename std::iterator_traits<Iterator>::iterator_category>);

		if (first != last)
			return detail::range(first, last);
		else
			return {};
	}

	//Returns the range of all numbers in the given container
	//If container is empty, it returns nullopt
	template <typename Container>
	[[nodiscard]] constexpr auto Range(const Container &container) noexcept
	{
		return Range(std::begin(container), std::end(container));
	}


	/*
		Trigonometry
	*/

	//Returns the sine in radians
	//This function uses a look-up table and is not as precise as std::sin
	[[nodiscard]] real Sin(real radian) noexcept;

	//Returns the cosine in radians
	//This function uses a look-up table and is not as precise as std::cos
	[[nodiscard]] real Cos(real radian) noexcept;

	//Returns the tangent in radians
	//This function uses a look-up table and is not as precise as std::tan
	[[nodiscard]] real Tan(real radian) noexcept;

	//Converts angle value in degrees to radians
	[[nodiscard]] constexpr auto Degree(real degree) noexcept
	{
		return degree * detail::degree_to_radian_factor;
	}

	//Converts angle value in radians to degrees
	[[nodiscard]] constexpr auto Radian(real radian) noexcept
	{
		return radian * detail::radian_to_degree_factor;
	}

	inline namespace literals
	{
		/*
			User defined literals (UDLs)
			For trigonometric conversions 
		*/

		constexpr auto operator""_deg(long double degree) noexcept
		{
			return Degree(static_cast<real>(degree));
		}

		constexpr auto operator""_rad(long double radian) noexcept
		{
			return Radian(static_cast<real>(radian));
		}
	} //literals
} //ion::utilities::math

#endif