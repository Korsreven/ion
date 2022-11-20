/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonCrypto.cpp
-------------------------------------------
*/

#include "IonCrypto.h"
#include <algorithm>

namespace ion::utilities::crypto
{

namespace detail
{

bool keccak::LFSR86540(uint8 &lfsr) noexcept
{
	auto result = ((lfsr & 0x01) != 0);

	if ((lfsr & 0x80) != 0)
		//Primitive polynomial over GF(2): x^8+x^6+x^5+x^4+1
		lfsr = (lfsr << 1) ^ 0x71;
	else
		lfsr <<= 1;

	return result;
}

void keccak::F1600_StatePermute() noexcept
{
	uint8 lfsr_state = 0x01;
					
	for (auto round = 0; round < 24; ++round)
	{
		//(theta) step (see [Keccak Reference, Section 2.3.2])
		{
			const auto size = 5;
			std::array<uint64, size> c;

			//Compute the parity of the columns
			for (auto x = 0; x < size; ++x)
				c[x] = ReadLane(x, 0) ^ ReadLane(x, 1) ^ ReadLane(x, 2) ^ ReadLane(x, 3) ^ ReadLane(x, 4);

			for (auto x = 0; x < size; ++x)
			{
				//Compute the (theta) effect for a given column
				auto d = c[(x + 4) % size] ^ ROL64(c[(x + 1) % size], 1);

				//Add the (theta) effect to the whole column
				for (auto y = 0; y < size; ++y)
					XORLane(x, y, d);
			}
		}

		//(roh) and (pi) steps (see [Keccak Reference, Sections 2.3.3 and 2.3.4])
		{
			//Start at coordinates (1 0)
			auto x = 1;
			auto y = 0;
			auto current = ReadLane(x, y);

			//Iterate over ((0 1)(2 3))^t * (1 0) for 0 <= t <= 23
			for (auto t = 0; t < 24; ++t)
			{
				//Compute the rotation constant r = (t+1)(t+2)/2
				auto r = ((t + 1) * (t + 2) / 2) % 64;

				//Compute ((0 1)(2 3)) * (x y)
				auto Y = (2 * x + 3 * y) % 5;

				x = y;
				y = Y;

				//Swap current and state(x,y), and rotate
				auto temp = ReadLane(x, y);
				WriteLane(x, y, ROL64(current, r));
				current = temp;
			}
		}

		//(chi) step (see [Keccak Reference, Section 2.3.1])
		{
			const auto size = 5;
			std::array<uint64, size> temp;

			for (auto y = 0; y < size; ++y)
			{
				//Take a copy of the plane
				for (auto x = 0; x < size; ++x)
					temp[x] = ReadLane(x, y);

				//Compute (chi) on the plane
				for (auto x = 0; x < size; ++x)
					WriteLane(x, y, temp[x] ^ ((~temp[(x + 1) % size]) & temp[(x + 2) % size]));
			}
		}

		//(iota) step (see [Keccak Reference, Section 2.3.5])
		{
			for (auto j = 0; j < 7; ++j)
			{
				auto bit_position = (1 << j) - 1U; //2^j-1

				if (LFSR86540(lfsr_state))
					XORLane(0, 0, 1ULL << bit_position);
			}
		}
	}
}

std::string keccak::operator()(int rate, int capacity, std::string_view input, char delimited_suffix, int output_length)
{
	if (((rate + capacity) != 1600) || ((rate % 8) != 0))
		return "";

	auto input_size = std::ssize(input);
	auto rate_in_bytes = rate / 8;
	auto block_size = 0;

	//Absorb all the input blocks
	while (input_size > 0)
	{
		block_size = std::min(input_size, rate_in_bytes);

		for (auto i = 0; i < block_size; ++i)
			state_[i] ^= static_cast<unsigned char>(input[i]);

		input.remove_prefix(block_size);
		input_size -= block_size;

		if (block_size == rate_in_bytes)
		{
			F1600_StatePermute();
			block_size = 0;
		}
	}

	//Do the padding and switch to the squeezing phase
	//Absorb the last few bits and add the first bit of padding (which coincides with the delimiter in delimited_suffix)
	state_[block_size] ^= static_cast<unsigned char>(delimited_suffix);

	//If the first bit of padding is at position rate-1, we need a whole new block for the second bit of padding
	if (((static_cast<unsigned char>(delimited_suffix) & 0x80) != 0) &&
		(block_size == (rate_in_bytes - 1)))
			F1600_StatePermute();

	//Add the second bit of padding
	state_[rate_in_bytes - 1] ^= 0x80;

	//Switch to the squeezing phase
	F1600_StatePermute();

	std::string output;
	output.reserve(output_length);

	//Squeeze out all the output blocks
	while (output_length > 0)
	{
		block_size = std::min(output_length, rate_in_bytes);
		output.append(reinterpret_cast<const char*>(std::data(state_)), block_size);
		output_length -= block_size;

		if (output_length > 0)
			F1600_StatePermute();
	}

	return output;
}

} //detail


/*
	Hashing - Keccak variants (non-standard)
*/

std::string Keccak_224(std::string_view input)
{
	return detail::keccak()(1152, 448, input, 0x01, 28);
}

std::string Keccak_256(std::string_view input)
{
	return detail::keccak()(1088, 512, input, 0x01, 32);
}

std::string Keccak_384(std::string_view input)
{
	return detail::keccak()(832, 768, input, 0x01, 48);
}

std::string Keccak_512(std::string_view input)
{
	return detail::keccak()(576, 1024, input, 0x01, 64);
}


/*
	Hashing - FIPS 202 (standard)
*/

std::string SHA3_224(std::string_view input)
{
	return detail::keccak()(1152, 448, input, 0x06, 28);
}

std::string SHA3_256(std::string_view input)
{
	return detail::keccak()(1088, 512, input, 0x06, 32);
}

std::string SHA3_384(std::string_view input)
{
	return detail::keccak()(832, 768, input, 0x06, 48);
}

std::string SHA3_512(std::string_view input)
{
	return detail::keccak()(576, 1024, input, 0x06, 64);
}

std::string SHAKE_128(std::string_view input, int output_length)
{
	return detail::keccak()(1344, 256, input, 0x1F, output_length);
}

std::string SHAKE_256(std::string_view input, int output_length)
{
	return detail::keccak()(1088, 512, input, 0x1F, output_length);
}

} //ion::utilities::crypto