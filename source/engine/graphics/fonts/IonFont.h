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

#include <optional>
#include <string>
#include <utility>
#include <vector>

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


		struct GlyphMetric
		{
			int Left = 0, Top = 0;
			int Width = 0, Height = 0;
			int ActualWidth = 0, ActualHeight = 0;
			int Advance = 0;
		};
		
		using GlyphTextureHandles = std::vector<int>;
		using GlyphBitmapData = std::vector<std::string>;
		using GlyphMetrices = std::vector<GlyphMetric>;
	} //font

	class Font final : public resources::FileResource<FontManager>
	{
		private:

			int size_ = 0;
			int face_index_ = 0;
			font::FontCharacterSet character_set_ = font::FontCharacterSet::ASCII;

			font::FontGlyphFilter glyph_min_filter_ = font::FontGlyphFilter::Bilinear;
			font::FontGlyphFilter glyph_mag_filter_ = font::FontGlyphFilter::Bilinear;

			std::optional<font::GlyphTextureHandles> glyph_handles_;

			std::optional<font::GlyphBitmapData> glyph_data_;
			std::optional<font::GlyphMetrices> glyph_metrics_;
			std::optional<int> glyph_max_height_;

		public:

			//Construct a new font with the given name, asset name, size, face index, character set and glyph filter for min/mag
			Font(std::string name, std::string asset_name, int size, int face_index, font::FontCharacterSet character_set,
				font::FontGlyphFilter min_filter, font::FontGlyphFilter mag_filter);

			//Construct a new font with the given name, asset name, size, character set and glyph filter for min/mag
			Font(std::string name, std::string asset_name, int size, font::FontCharacterSet character_set,
				font::FontGlyphFilter min_filter, font::FontGlyphFilter mag_filter);

			//Construct a new font with the given name, asset name, size, character set and glyph filter
			Font(std::string name, std::string asset_name, int size, font::FontCharacterSet character_set,
				font::FontGlyphFilter filter);

			//Construct a new font with the given name, asset name, size and character set
			Font(std::string name, std::string asset_name, int size, font::FontCharacterSet character_set);

			//Construct a new font with the given name, asset name and size
			Font(std::string name, std::string asset_name, int size);


			/*
				Modifiers
			*/

			//Sets the handle for the font to the given value
			inline void GlyphHandles(std::optional<font::GlyphTextureHandles> handles)
			{
				glyph_handles_ = std::move(handles);
			}


			//Sets the glyph data of the font to the given data
			inline void GlyphData(font::GlyphBitmapData data, font::GlyphMetrices glyph_metrics, int max_height)
			{
				glyph_data_ = std::move(data);
				glyph_metrics_ = std::move(glyph_metrics);
				glyph_max_height_ = max_height;
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
			[[nodiscard]] inline auto& GlyphHandles() const noexcept
			{
				return glyph_handles_;
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

			//Returns the max glyph height for the font
			//Returns nullopt if the font has not been prepared yet
			[[nodiscard]] inline auto GlyphMaxHeight() const noexcept
			{
				return glyph_max_height_;
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