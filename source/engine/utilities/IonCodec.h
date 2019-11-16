/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	utilities
File:	IonCodec.h
-------------------------------------------
*/

#ifndef ION_CODEC_H
#define ION_CODEC_H

#include <array>
#include <cassert>
#include <limits>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

#include "IonMath.h"
#include "IonStringUtility.h"

namespace ion::utilities::codec
{
	namespace detail
	{
		using namespace std::string_view_literals;

		//Constants used for encoding to different bases

		inline constexpr auto base36_map = "0123456789abcdefghijklmnopqrstuvwxyz"sv;
		inline constexpr auto base32_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"sv;
		inline constexpr auto base64_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"sv;
		inline constexpr auto base64url_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"sv;


		constexpr auto is_power_of_two(int x)
		{
			if (x < 2)
				return false;

			return (x & (x - 1)) == 0;
		}

		template <const std::string_view &Map,
				  auto Base = static_cast<int>(std::size(Map))>
		class base_index_table
		{
			private:

				static constexpr auto GenerateUnmap() noexcept
				{
					auto has_lower =
						[]() noexcept
						{
							for (auto c : Map)
							{
								if (string::detail::is_lower(c))
									return true;
							}
							return false;
						}();

					auto has_upper =
						[]() noexcept
						{
							for (auto c : Map)
							{
								if (string::detail::is_upper(c))
									return true;
							}
							return false;
						}();

					std::array<unsigned char,
						std::numeric_limits<unsigned char>::max()> unmap{};
							//Need to zero-initalize for constexpr

					//std::array::fill() is not constexpr yet
					for (auto &c : unmap)
						c = std::numeric_limits<unsigned char>::max();

					//Reverse map, keep case
					for (auto i = 0; i < Base; ++i)
						unmap[static_cast<unsigned char>(Map[i])] =
							static_cast<unsigned char>(i);
					
					//Reverse map, generate for the opposite case
					if (has_lower != has_upper)
					{
						if (has_lower)
						{
							for (auto i = 0; i < Base; ++i)
								unmap[static_cast<unsigned char>(string::detail::to_upper(Map[i]))] =
									static_cast<unsigned char>(i);
						}
						else
						{
							for (auto i = 0; i < Base; ++i)
								unmap[static_cast<unsigned char>(string::detail::to_lower(Map[i]))] =
									static_cast<unsigned char>(i);
						}
					}

					return unmap;
				}

			public:

				static constexpr auto Unmap = GenerateUnmap();
		};

		template <const std::string_view &Map,
				  int Base = static_cast<int>(std::size(Map))>
		class rfc_base final :
			base_index_table<Map, Base>
		{
			static_assert(Base >= 2 && Base <= 256 &&
						  is_power_of_two(Base));

			public:

				using base_index_table<Map, Base>::Unmap;

			private:

				static constexpr auto BitsInDigit() noexcept
				{
					return math::Log2(Base);
				}

				static constexpr auto BytesInBlock() noexcept
				{
					auto bits = BitsInDigit();
					auto divisor = std::gcd(bits, 8);
					return std::make_pair(bits / divisor, 8 / divisor);
				}

				static constexpr auto bits_in_digit_ = BitsInDigit();
				static constexpr auto bytes_in_block_ = BytesInBlock();


				static void EncodeBlock(std::string_view in, std::string &out, int pad_length) noexcept
				{
					//Pack bytes
					auto buffer = 0ULL;

					for (unsigned char c : in)
					{
						if (buffer != 0)
							buffer <<= 8;

						buffer |= c;
					}

					if (pad_length > 0)
						buffer <<= 8 *
							(bytes_in_block_.first - static_cast<int>(std::size(in)));

					//Output bytes
					for (auto i = bytes_in_block_.second; i > pad_length; --i)
						out.push_back(Map[static_cast<char>(
							(buffer >> (i - 1) * bits_in_digit_) & (Base - 1))]);
				}

				static auto DecodeBlock(std::string_view in, std::string &out, int pad_length) noexcept
				{
					//Pack bytes
					auto buffer = 0ULL;

					for (auto c : in)
					{
						auto value = Unmap[static_cast<unsigned char>(c)];

						//Input contains invalid character
						if (value >= Base)
							return false;

						if (buffer != 0)
							buffer <<= bits_in_digit_;

						buffer |= value;
					}

					if (pad_length > 0)
						buffer <<= bits_in_digit_ *
							(bytes_in_block_.second - static_cast<int>(std::size(in)));

					//Output bytes
					for (auto i = bytes_in_block_.first; i > pad_length; --i)
						out.push_back(static_cast<char>(buffer >> (i - 1) * 8));

					return true;
				}

			public:

				static constexpr auto EncodeLength(int bytes) noexcept
				{
					auto bits = bytes * 8;
					auto length = bits / bits_in_digit_;

					if ((bits % bits_in_digit_) > 0)
						++length;

					return length;
				}

				static constexpr auto DecodeLength(int bytes) noexcept
				{
					return bytes * bits_in_digit_ / 8;
				}

				static auto Encode(std::string_view str, std::optional<char> padding_character)
				{
					if (std::empty(str))
						return std::string{};

					auto size = static_cast<int>(std::size(str));
					auto r = size % bytes_in_block_.first;

					auto length = EncodeLength(size);
					auto pad_length = r > 0 ?
						bytes_in_block_.second - EncodeLength(r) :
						0;

					std::string result;
					result.reserve(length +
						(padding_character ? pad_length : 0));

					//Process bytes in groups
					for (auto d = size / bytes_in_block_.first; d > 0; --d)
					{
						EncodeBlock(str.substr(0, bytes_in_block_.first), result, 0);
						str.remove_prefix(bytes_in_block_.first);
					}

					//Process padding
					if (r > 0)
					{
						EncodeBlock(str, result, pad_length);

						if (padding_character)
							result.append(pad_length, *padding_character);
					}

					return result;
				}

				static auto Decode(std::string_view str)
				{
					if (std::empty(str))
						return std::optional<std::string>{};

					auto size = static_cast<int>(std::size(str));
					auto r = size % bytes_in_block_.second;

					auto length = DecodeLength(size);
					auto pad_length = r > 0 ?
						bytes_in_block_.first - DecodeLength(r) :
						0;

					std::string result;
					result.reserve(length);

					//Process bytes in groups
					for (auto d = size / bytes_in_block_.second; d > 0; --d)
					{
						if (!DecodeBlock(str.substr(0, bytes_in_block_.second), result, 0))
							return std::optional<std::string>{};

						str.remove_prefix(bytes_in_block_.second);
					}

					//Process padding
					if (r > 0)
					{
						if (!DecodeBlock(str, result, pad_length))
							return std::optional<std::string>{};
					}

					return std::make_optional(std::move(result));
				}
		};


		bool is_base(std::string_view str, int base) noexcept;

		template <typename T>
		inline auto number_to_base(T value, int base)
		{
			if (base < 2 || base > 36)
				return std::optional<std::string>();

			//Negative
			if (value < T{0})
				return number_to_base(static_cast<std::make_unsigned_t<T>>(value), base);

			std::string result;

			do
			{
				result.insert(std::begin(result), base36_map[value % base]);
			} while ((value /= static_cast<T>(base)) > T{0});

			return std::make_optional(std::move(result));
		}

		template <typename T>
		inline auto base_to_number(std::string_view str, int base)
		{
			if (base < 2 || base > 36 ||
				!is_base(str, base))
					return std::optional<T>();

			//Negative
			if (!std::empty(str) && str.front() == '-')
			{
				str.remove_prefix(1);
				auto result = base_to_number<T>(str, base);

				if constexpr (std::is_signed_v<T>)
					*result = -*result;

				return result;
			}

			auto result = T{0};
			{
				auto exponent = static_cast<int>(std::size(str));
				for (auto c : str)
				{
					result +=
						(c <= '9' ?
						//0-9
						c - '0' :
						//a-z
						string::detail::to_lower(c) - 'a' + 10) *
						math::Power(static_cast<T>(base),
									static_cast<T>(--exponent));
				}
			}

			return std::make_optional(std::move(result));
		}

		std::string string_to_hex(std::string_view str);
		std::string string_to_base32(std::string_view str, std::optional<char> padding_character);
		std::string string_to_base64(std::string_view str, std::optional<char> padding_character);
		std::string string_to_base64_url(std::string_view str, std::optional<char> padding_character);

		std::optional<std::string> hex_to_string(std::string_view str);
		std::optional<std::string> base32_to_string(std::string_view str);
		std::optional<std::string> base64_to_string(std::string_view str);
		std::optional<std::string> base64_url_to_string(std::string_view str);
	} //detail

	
	/*
		Encoding
		Base 2-36 (radix)
	*/

	//Encode a number (base 10) to a given base in range [2, 36]
	template <typename T>
	[[nodiscard]] inline auto EncodeTo(T value, int base)
	{
		static_assert(std::is_integral_v<T>);
		assert(base >= 2 && base <= 36);
		return detail::number_to_base(value, base);
	}


	/*
		Decoding
		Base 2-36 (radix)
	*/

	//Decode a string from a given base in range [2, 36] to a number (base 10)
	template <typename T>
	[[nodiscard]] inline auto DecodeFrom(std::string_view str, int base)
	{
		static_assert(std::is_integral_v<T>);
		assert(base >= 2 && base <= 36);
		return detail::base_to_number<T>(str, base);
	}


	/*
		Encoding (RFC 4648)
		Base 16/32/64
	*/

	//Encode a string to base16 (hexadecimal)
	//Letters a-f, are returned in lower case
	[[nodiscard]] std::string EncodeToHex(std::string_view str);


	//Encode a string to base32
	//Letters A-Z, are returned in upper case
	//Result is not padded
	[[nodiscard]] std::string EncodeToBase32(std::string_view str);

	//Encode a string to base32
	//Letters A-Z, are returned in upper case
	//Result is padded with the given padding character
	[[nodiscard]] std::string EncodeToBase32(std::string_view str, char padding_character);


	//Encode a string to base64
	//Result is not padded
	[[nodiscard]] std::string EncodeToBase64(std::string_view str);

	//Encode a string to base64
	//Result is padded with the given padding character
	[[nodiscard]] std::string EncodeToBase64(std::string_view str, char padding_character);

	//Encode a string to base64 URL
	//Safe variant for URLs and filenames
	//Result is not padded
	[[nodiscard]] std::string EncodeToBase64_URL(std::string_view str);

	//Encode a string to base64 URL
	//Safe variant for URLs and filenames
	//Result is padded with the given padding character
	[[nodiscard]] std::string EncodeToBase64_URL(std::string_view str, char padding_character);


	/*
		Decoding (RFC 4648)
		Base 16/32/64
	*/

	//Decode a string from base16 (hexadecimal)
	//Note that letters are case insensitive
	[[nodiscard]] std::optional<std::string> DecodeFromHex(std::string_view str);

	//Decode a string from base32
	//Note that letters are case insensitive
	[[nodiscard]] std::optional<std::string> DecodeFromBase32(std::string_view str);

	//Decode a string from base64
	//Note that letters are case sensitive
	[[nodiscard]] std::optional<std::string> DecodeFromBase64(std::string_view str);

	//Decode a string from base64 URL
	//Safe variant for URLs and filenames
	//Note that letters are case sensitive
	[[nodiscard]] std::optional<std::string> DecodeFromBase64_URL(std::string_view str);
} //ion::utilities::codec

#endif