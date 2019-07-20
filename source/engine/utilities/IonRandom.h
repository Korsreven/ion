/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonRandom.h
-------------------------------------------
*/

#ifndef _ION_RANDOM_
#define _ION_RANDOM_

#include <algorithm>
#include <array>
#include <random>
#include <type_traits>

#include "types/IonTypes.h"

namespace ion::utilities::random
{
	namespace detail
	{
		//Type traits for deciding which uniform distribution to use

		template <typename T>
		struct random_distribution_impl
		{
			using type = std::uniform_int_distribution<T>;
		};

		template <>
		struct random_distribution_impl<float>
		{
			using type = std::uniform_real_distribution<float>;
		};

		template <>
		struct random_distribution_impl<double>
		{
			using type = std::uniform_real_distribution<double>;
		};

		template <>
		struct random_distribution_impl<long double>
		{
			using type = std::uniform_real_distribution<long double>;
		};

		template <typename T>
		struct random_distribution :
			random_distribution_impl<std::remove_cv_t<T>>
		{
		};

		template <typename T>
		using random_distribution_t = typename random_distribution<T>::type;


		//Random generator class for seeding a 32 and 64 bit mersenne twister engine
		class random_generator final
		{
			private:

				template <typename T, decltype(T::state_size) N = T::state_size>
				static auto SeedEngine32() noexcept
				{
					std::array<T::result_type, N> data;

					std::random_device device;
					std::generate(std::begin(data), std::end(data),
						[&]() noexcept
						{
							return device();
						}
					);

					std::seed_seq seed(std::cbegin(data), std::cend(data));
					return T{seed};
				}

				template <typename T, decltype(T::state_size) N = T::state_size>
				static auto SeedEngine64() noexcept
				{
					std::array<T::result_type, N> data;

					std::random_device device;
					std::generate(std::begin(data), std::end(data),
						[&]() noexcept
						{
							return (static_cast<uint64>(device()) << 32) | device();
						}
					);

					std::seed_seq seed(std::cbegin(data), std::cend(data));
					return T{seed};
				}

			public:

				static void Initialize() noexcept;

				static inline auto &Engine32() noexcept
				{
					static auto engine32{SeedEngine32<std::mt19937>()};
					return engine32;
				}

				static inline auto &Engine64() noexcept
				{
					static auto engine64{SeedEngine64<std::mt19937_64>()};
					return engine64;
				}

				template <typename T>
				static auto Number32(T min, T max) noexcept
				{
					return random_distribution_t<T>{min, max}(Engine32());
				}

				template <typename T>
				static auto Number64(T min, T max) noexcept
				{
					return random_distribution_t<T>{min, max}(Engine64());
				}
		};
	} //detail


	/*
		Random integer or floating point in specified range
	*/

	//Returns a random number in range [min, max]
	template <typename T>
	[[nodiscard]] inline auto Number(T min, T max) noexcept
	{
		static_assert(std::is_arithmetic_v<T>);

		//Use 32 bit engine
		if constexpr (sizeof(T) <= 4)
			return detail::random_generator::Number32(min, max);
		//Use 64 bit engine
		else
			return detail::random_generator::Number64(min, max);
	}

	//Returns a random number in range [0, max]
	template <typename T>
	[[nodiscard]] inline auto Number(T max) noexcept
	{
		return Number(T{0}, max);
	}


	/*
		Random floating point between 0.0 and 1.0
	*/

	//Returns a random floating point number in range [0.0, 1.0]
	template <typename T>
	[[nodiscard]] inline auto Number() noexcept
	{
		static_assert(std::is_floating_point_v<T>);
		return Number(T{1});
	}

	//Returns a random real number in range [0.0, 1.0]
	[[nodiscard]] inline auto Number() noexcept
	{
		return Number<real>();
	}
} //ion::utilities::random

#endif