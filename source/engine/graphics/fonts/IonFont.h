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

#include "resources/files/IonFileResource.h"

namespace ion::graphics::fonts
{
	class FontManager; //Forward declaration

	namespace font
	{
		enum class CharacterEncoding : int
		{
			ASCII = 128,
			ExtendedASCII = 256
		};

		enum class GlyphFilter : bool
		{
			NearestNeighbor,	//Nearest
			Bilinear			//Linear
		};


		struct GlyphExtents
		{
			int Left = 0, Top = 0;
			int Width = 0, Height = 0;
			int ActualWidth = 0, ActualHeight = 0;
			int Advance = 0;
		};

		namespace detail
		{
			template <typename T>
			using container_type = std::vector<T>;
		} //detail
	} //font

	class Font : public resources::files::FileResource<FontManager>
	{
		private:

			std::optional<int> handle_;	
			std::optional<font::detail::container_type<int>> glyph_handles_;

			std::optional<font::detail::container_type<std::string>> glyph_data_;	
			std::optional<font::detail::container_type<font::GlyphExtents>> glyph_extents_;
			std::optional<int> glyph_max_height_;

			int size_ = 0;
			int face_index_ = 0;
			font::CharacterEncoding character_encoding_ = font::CharacterEncoding::ASCII;

			font::GlyphFilter glyph_min_filter_ = font::GlyphFilter::Bilinear;
			font::GlyphFilter glyph_mag_filter_ = font::GlyphFilter::Bilinear;		

		public:

			//Construct a new font with the given name, size, face index, character encoding and glyph filter for min/mag
			Font(std::string name, int size, int face_index, font::CharacterEncoding encoding,
				font::GlyphFilter min_filter, font::GlyphFilter mag_filter) :

				FileResource{std::move(name)},

				size_{size},
				face_index_{face_index},
				character_encoding_{encoding},

				glyph_min_filter_{min_filter},
				glyph_mag_filter_{mag_filter}
			{
				//Empty
			}

			//Construct a new font with the given name, size, character encoding and glyph filter for min/mag
			Font(std::string name, int size, font::CharacterEncoding encoding,
				font::GlyphFilter min_filter, font::GlyphFilter mag_filter) :

				Font{std::move(name), size, 0, encoding, min_filter, mag_filter}
			{
				//Empty
			}

			//Construct a new font with the given name, size, character encoding and glyph filter
			Font(std::string name, int size, font::CharacterEncoding encoding,
				font::GlyphFilter filter) :

				Font{std::move(name), size, encoding, filter, filter}
			{
				//Empty
			}

			//Construct a new font with the given name, size and character encoding
			Font(std::string name, int size, font::CharacterEncoding encoding) :

				FileResource{std::move(name)},

				size_{size},
				character_encoding_{encoding}
			{
				//Empty
			}

			//Construct a new font with the given name and size
			Font(std::string name, int size) :

				FileResource{std::move(name)},
				size_{size}
			{
				//Empty
			}


			/*
				Modifiers
			*/

			//Sets the handle for the font to the given value
			inline void Handle(std::optional<int> handle)
			{
				handle_ = handle;
			}

			//Sets the handle for the font to the given value
			inline void GlyphHandles(std::optional<font::detail::container_type<int>> handles)
			{
				glyph_handles_ = std::move(handles);
			}


			//Sets the glyph data of the font to the given data
			inline void GlyphData(font::detail::container_type<std::string> data,
				font::detail::container_type<font::GlyphExtents> extents, int max_height)
			{
				glyph_data_ = std::move(data);
				glyph_extents_ = std::move(extents);
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

			//Returns the handle for the font
			//Returns nullopt if the font is not loaded
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}

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

			//Returns the glyph extents for the font
			//Returns nullopt if the font has not been prepared yet
			[[nodiscard]] inline auto& GlyphExtents() const noexcept
			{
				return glyph_extents_;
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

			//Returns the character encoding for the font
			[[nodiscard]] inline auto CharacterEncoding() const noexcept
			{
				return character_encoding_;
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