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

#ifndef ION_RANDOM_H
#define ION_RANDOM_H

#include <algorithm>
#include <array>
#include <random>
#include <type_traits>

#include "types/IonTypes.h"

///@brief Namespace containing commonly used functions for generating random numbers in different ranges
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


		///@brief Random number generator class with a 32 and 64 bit engine
		template <typename Engine32_t = std::mt19937, typename Engine64_t = std::mt19937_64>
		class random_number_generator final
		{
			private:

				Engine32_t engine32_;
				Engine64_t engine64_;

				static auto GetSeededEngine32() noexcept
				{
					std::array<Engine32_t::result_type, Engine32_t::state_size> data;

					std::random_device device;
					std::generate(std::begin(data), std::end(data),
						[&]() noexcept
						{
							return device();
						}
					);

					std::seed_seq seed(std::cbegin(data), std::cend(data));
					return Engine32_t{seed};
				}

				static auto GetSeededEngine64() noexcept
				{
					std::array<Engine64_t::result_type, Engine64_t::state_size> data;

					std::random_device device;
					std::generate(std::begin(data), std::end(data),
						[&]() noexcept
						{
							return (static_cast<uint64>(device()) << 32) | device();
						}
					);

					std::seed_seq seed(std::cbegin(data), std::cend(data));
					return Engine64_t{seed};
				}

			public:

				random_number_generator() noexcept :
					engine32_{GetSeededEngine32()},
					engine64_{GetSeededEngine64()}
				{
					//Empty
				}

				random_number_generator(std::random_device::result_type seed) :
					engine32_{seed},
					engine64_{static_cast<uint64>(seed)}
				{
					//Empty
				}

				random_number_generator(Engine32_t::result_type seed32, Engine64_t::result_type seed64) :
					engine32_{seed32},
					engine64_{seed64}
				{
					//Empty
				}


				inline auto &Engine32() noexcept
				{
					return engine32_;
				}

				inline auto &Engine64() noexcept
				{
					return engine64_;
				}

				template <typename T>
				auto Number32(T min, T max) noexcept
				{
					return random_distribution_t<T>{min, max}(Engine32());
				}

				template <typename T>
				auto Number64(T min, T max) noexcept
				{
					return random_distribution_t<T>{min, max}(Engine64());
				}
		};

		inline random_number_generator<> default_rng;


		///@brief Returns a random number in range [min, max]
		template <typename T>
		inline auto get_number(T min, T max, random_number_generator<> &rng) noexcept
		{
			static_assert(std::is_arithmetic_v<T>);

			//Use 32 bit engine
			if constexpr (sizeof(T) <= 4)
				return rng.Number32(min, max);
			//Use 64 bit engine
			else
				return rng.Number64(min, max);
		}
	} //detail


	/**
		@name Random integer or floating point in specified range
		@{
	*/

	///@brief Returns a random number in range [min, max]
	template <typename T>
	[[nodiscard]] inline auto Number(T min, T max) noexcept
	{
		return detail::get_number(min, max, detail::default_rng);
	}

	///@brief Returns a random number in range [min, max]
	template <typename T>
	[[nodiscard]] inline auto Number(T min, T max, detail::random_number_generator<> &rng) noexcept
	{
		return detail::get_number(min, max, rng);
	}

	///@brief Returns a random number in range [0, max]
	template <typename T>
	[[nodiscard]] inline auto Number(T max) noexcept
	{
		return Number(T{0}, max);
	}

	///@brief Returns a random number in range [0, max]
	template <typename T>
	[[nodiscard]] inline auto Number(T max, detail::random_number_generator<> &rng) noexcept
	{
		return Number(T{0}, max, rng);
	}

	///@}

	/**
		@name Random floating point between 0.0 and 1.0
		@{
	*/

	///@brief Returns a random floating point number in range [0.0, 1.0]
	template <typename T>
	[[nodiscard]] inline auto Number() noexcept
	{
		static_assert(std::is_floating_point_v<T>);
		return Number(T{1});
	}

	///@brief Returns a random floating point number in range [0.0, 1.0]
	template <typename T>
	[[nodiscard]] inline auto Number(detail::random_number_generator<> &rng) noexcept
	{
		static_assert(std::is_floating_point_v<T>);
		return Number(T{1}, rng);
	}

	///@brief Returns a random real number in range [0.0, 1.0]
	[[nodiscard]] inline auto Number() noexcept
	{
		return Number<real>();
	}

	///@brief Returns a random real number in range [0.0, 1.0]
	[[nodiscard]] inline auto Number(detail::random_number_generator<> &rng) noexcept
	{
		return Number<real>(rng);
	}

	///@}
} //ion::utilities::random

#endif