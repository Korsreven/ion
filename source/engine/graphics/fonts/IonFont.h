/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonFont.h
-------------------------------------------
*/

#ifndef ION_FONT_H
#define ION_FONT_H

#include <cassert>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "graphics/textures/IonTexture.h"
#include "resources/IonFileResource.h"

namespace ion::graphics::fonts
{
	class FontManager; //Forward declaration

	namespace font
	{
		enum class FontCharacterSet : int
		{
			ASCII = 128,
			ExtendedASCII = 256
		};

		enum class FontGlyphFilter : bool
		{
			NearestNeighbor,	//Nearest
			Bilinear			//Linear
		};


		struct GlyphTextureHandle final
		{
			std::vector<int> Ids;
			textures::texture::TextureType Type = textures::texture::TextureType::Texture2D;


			inline auto operator[](int glyph_index) const noexcept
				-> textures::texture::TextureHandle
			{
				assert(!std::empty(Ids));
				return {Ids[glyph_index < std::ssize(Ids) ? glyph_index : 0], Type};
			}
		};

		struct GlyphMetric final
		{
			int Left = 0, Top = 0;
			int Width = 0, Height = 0;
			int ActualWidth = 0, ActualHeight = 0;
			int Advance = 0;
		};

		struct GlyphMaxMetric final
		{
			int Width = 0, Height = 0;
			int ActualWidth = 0, ActualHeight = 0;
		};

		using GlyphBitmapData = std::vector<std::string>;
		using GlyphMetrices = std::vector<GlyphMetric>;
	} //font


	//A class representing a font (with a fixed size) that contains multiple glyphs
	//Only ASCII and ExtendedASCII character sets are supported for now
	class Font final : public resources::FileResource<FontManager>
	{
		private:

			int size_ = 0;
			int face_index_ = 0;
			int character_spacing_ = 0;
			font::FontCharacterSet character_set_ = font::FontCharacterSet::ASCII;

			font::FontGlyphFilter glyph_min_filter_ = font::FontGlyphFilter::Bilinear;
			font::FontGlyphFilter glyph_mag_filter_ = font::FontGlyphFilter::Bilinear;

			std::optional<font::GlyphTextureHandle> glyph_handle_;

			std::optional<font::GlyphBitmapData> glyph_data_;
			std::optional<font::GlyphMetrices> glyph_metrics_;
			std::optional<font::GlyphMaxMetric> glyph_max_metrics_;

		public:

			//Construct a new font with the given name, asset name, size, face index, character spacing, character set and glyph filter for min/mag
			Font(std::string name, std::string asset_name, int size, int face_index,
				int character_spacing, font::FontCharacterSet character_set, font::FontGlyphFilter min_filter, font::FontGlyphFilter mag_filter) noexcept;

			//Construct a new font with the given name, asset name, size, character spacing, character set and glyph filter for min/mag
			Font(std::string name, std::string asset_name, int size,
				int character_spacing, font::FontCharacterSet character_set, font::FontGlyphFilter min_filter, font::FontGlyphFilter mag_filter) noexcept;

			//Construct a new font with the given name, asset name, size, character spacing, character set and glyph filter
			Font(std::string name, std::string asset_name, int size,
				int character_spacing, font::FontCharacterSet character_set, font::FontGlyphFilter filter) noexcept;

			//Construct a new font with the given name, asset name, size, character spacing and character set
			Font(std::string name, std::string asset_name, int size,
				int character_spacing, font::FontCharacterSet character_set) noexcept;

			//Construct a new font with the given name, asset name, size and character spacing
			Font(std::string name, std::string asset_name, int size,
				int character_spacing = 0) noexcept;


			/*
				Modifiers
			*/

			//Sets the handle for the glyphs to the given handle
			inline void GlyphHandle(std::optional<font::GlyphTextureHandle> handle) noexcept
			{
				glyph_handle_ = std::move(handle);
			}


			//Sets the glyph data of the font to the given data
			inline void GlyphData(font::GlyphBitmapData data, font::GlyphMetrices glyph_metrics, const font::GlyphMaxMetric &glyph_max_metrics) noexcept
			{
				glyph_data_ = std::move(data);
				glyph_metrics_ = std::move(glyph_metrics);
				glyph_max_metrics_ = glyph_max_metrics;
			}

			//Sets the glyph metrics of the font to the given metrics
			inline void GlyphMetrics(font::GlyphMetrices glyph_metrics) noexcept
			{
				glyph_metrics_ = std::move(glyph_metrics);
			}

			//Resets the glyph data to save some memory (if not needed anymore)
			inline void ResetGlyphData() noexcept
			{
				glyph_data_.reset();
			}


			/*
				Observers
			*/

			//Returns the handle for each of the glyphs in the font
			//Returns nullopt if the font is not loaded
			[[nodiscard]] inline auto& GlyphHandle() const noexcept
			{
				return glyph_handle_;
			}


			//Returns the glyph data for the font
			//Returns nullopt if the font has not been prepared yet, or is no longer needed (fully loaded or has failed)
			[[nodiscard]] inline auto& GlyphData() const noexcept
			{
				return glyph_data_;
			}

			//Returns the glyph metrics for the font
			//Returns nullopt if the font has not been prepared yet
			[[nodiscard]] inline auto& GlyphMetrics() const noexcept
			{
				return glyph_metrics_;
			}

			//Returns the glyph max metrics for the font
			//Returns nullopt if the font has not been prepared yet
			[[nodiscard]] inline auto& GlyphMaxMetrics() const noexcept
			{
				return glyph_max_metrics_;
			}


			//Returns the size of the font
			[[nodiscard]] inline auto Size() const noexcept
			{
				return size_;
			}

			//Returns the face index of the font (usually 0)
			[[nodiscard]] inline auto FaceIndex() const noexcept
			{
				return face_index_;
			}

			//Returns the character spacing for the font
			[[nodiscard]] inline auto CharacterSpacing() const noexcept
			{
				return character_spacing_;
			}

			//Returns the character set for the font
			[[nodiscard]] inline auto CharacterSet() const noexcept
			{
				return character_set_;
			}


			//Returns the min, mag and mip glyph filters for the font
			[[nodiscard]] inline auto GlyphFilter() const noexcept
			{
				return std::pair{glyph_min_filter_, glyph_mag_filter_};
			}

			//Returns the min (minifying) glyph filter for the font
			[[nodiscard]] inline auto GlyphMinFilter() const noexcept
			{
				return glyph_min_filter_;
			}

			//Returns the mag (magnifying) glyph filter for the font
			[[nodiscard]] inline auto GlyphMagFilter() const noexcept
			{
				return glyph_mag_filter_;
			}
	};
} //ion::graphics::fonts

#endif