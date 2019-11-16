/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonCrypto.h
-------------------------------------------
*/

#ifndef ION_CRYPTO_H
#define ION_CRYPTO_H

#include <array>
#include <string>
#include <string_view>

#include "types/IonTypes.h"

namespace ion::utilities::crypto
{
	namespace detail
	{
		class keccak final
		{
			private:

				std::array<uint8, 200> state_{};


				static inline auto ROL64(uint64 a, uint32 offset) noexcept
				{
					return (a << offset) ^ (a >> (64 - offset));
				}

				static inline auto I(uint32 x, uint32 y) noexcept
				{
					return x + 5 * y;
				}

				inline auto ReadLane(uint32 x, uint32 y) const noexcept
				{
					return reinterpret_cast<const uint64*>(std::data(state_))[I(x, y)];
				}

				inline void WriteLane(uint32 x, uint32 y, uint64 lane) noexcept
				{
					reinterpret_cast<uint64*>(std::data(state_))[I(x, y)] = lane;
				}

				inline void XORLane(uint32 x, uint32 y, uint64 lane) noexcept
				{
					reinterpret_cast<uint64*>(std::data(state_))[I(x, y)] ^= lane;
				}

				//Function that computes the linear feedback shift register (LFSR) used to
				//define the round constants (see [Keccak Reference, Section 1.2]).
				static bool LFSR86540(uint8 &lfsr) noexcept;
				
				//Function that computes the Keccak-f[1600] permutation on the given state.
				void F1600_StatePermute() noexcept;

			public:

				//A readable and compact implementation of the Keccak sponge functions
				//that use the Keccak-f[1600] permutation.
				std::string operator()(int rate, int capacity, std::string_view input, char delimited_suffix, int output_length);
		};
	} //detail


	/*
		Hashing
		Keccak variants (non-standard)
	*/

	//Function to compute Keccak-224 on the input message. The output length is fixed to 28 bytes
	[[nodiscard]] std::string Keccak_224(std::string_view input);

	//Function to compute Keccak-256 on the input message. The output length is fixed to 32 bytes
	[[nodiscard]] std::string Keccak_256(std::string_view input);

	//Function to compute Keccak-384 on the input message. The output length is fixed to 48 bytes
	[[nodiscard]] std::string Keccak_384(std::string_view input);

	//Function to compute Keccak-512 on the input message. The output length is fixed to 64 bytes
	[[nodiscard]] std::string Keccak_512(std::string_view input);


	/*
		Hashing
		FIPS 202 (standard)
	*/

	//Function to compute SHA3-224 on the input message. The output length is fixed to 28 bytes
	[[nodiscard]] std::string SHA3_224(std::string_view input);

	//Function to compute SHA3-256 on the input message. The output length is fixed to 32 bytes
	[[nodiscard]] std::string SHA3_256(std::string_view input);

	//Function to compute SHA3-384 on the input message. The output length is fixed to 48 bytes
	[[nodiscard]] std::string SHA3_384(std::string_view input);

	//Function to compute SHA3-512 on the input message. The output length is fixed to 64 bytes
	[[nodiscard]] std::string SHA3_512(std::string_view input);

	//Function to compute SHAKE128 on the input message with any output length
	[[nodiscard]] std::string SHAKE_128(std::string_view input, int output_length);

	//Function to compute SHAKE256 on the input message with any output length
	[[nodiscard]] std::string SHAKE_256(std::string_view input, int output_length);
} //ion::utilities::crypto

#endif