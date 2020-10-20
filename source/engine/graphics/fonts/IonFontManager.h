/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonFontManager.h
-------------------------------------------
*/

#ifndef ION_FONT_MANAGER_H
#define ION_FONT_MANAGER_H

#include <filesystem>
#include <string>
#include <tuple>
#include <utility>

#include "IonFont.h"
#include "resources/files/IonFileResourceManager.h"
#include "resources/files/repositories/IonFontRepository.h"
#include "types/IonTypes.h"

#undef CreateFont

namespace ion::graphics::fonts
{
	namespace font_manager
	{
		namespace detail
		{
			inline auto is_fonts_equivalent(const Font &lhs, const Font &rhs) noexcept
			{
				return lhs.Name() == rhs.Name() && lhs.Size() == rhs.Size() && lhs.FaceIndex() == rhs.FaceIndex() &&
					lhs.CharacterEncoding() == rhs.CharacterEncoding() && lhs.GlyphFilter() == rhs.GlyphFilter();
			}

			std::optional<std::tuple<font::detail::container_type<std::string>, font::detail::container_type<font::GlyphExtents>, int>> prepare_font(
				const std::string &file_data, int size, int face_index, font::CharacterEncoding encoding);

			std::pair<std::optional<int>, std::optional<font::detail::container_type<int>>> load_font(
				const font::detail::container_type<std::string> &glyph_data,
				const font::detail::container_type<font::GlyphExtents> &glyph_extents,
				font::GlyphFilter min_filter, font::GlyphFilter mag_filter) noexcept;
			void unload_font(int font_handle, const font::detail::container_type<int> &glyph_handles) noexcept;
		} //detail
	} //font_manager


	class FontManager final :
		public resources::files::FileResourceManager<Font, FontManager, resources::files::repositories::FontRepository>
	{
		private:
			

		protected:

			/*
				Events
			*/

			bool IsResourcesEquivalent(const Font &lhs, const Font &rhs) noexcept override;

			bool PrepareResource(Font &font) noexcept override;
			bool LoadResource(Font &font) noexcept override;
			bool UnloadResource(Font &font) noexcept override;


			//See FileResourceManager::ResourceLoaded for more details
			void ResourceLoaded(Font &font) noexcept override;

			//See FileResourceManager::ResourceFailed for more details
			void ResourceFailed(Font &font) noexcept override;

		public:

			//Default constructor
			FontManager() = default;

			//Deleted copy constructor
			FontManager(const FontManager&) = delete;

			//Default move constructor
			FontManager(FontManager&&) = default;

			//Destructor
			~FontManager() noexcept;


			/*
				Operators
			*/

			//Deleted copy assignment
			FontManager& operator=(const FontManager&) = delete;

			//Move assignment
			FontManager& operator=(FontManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all fonts in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Fonts() noexcept
			{
				return this->Resources();
			}

			//Returns an immutable range of all fonts in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Fonts() const noexcept
			{
				return this->Resources();
			}


			/*
				Fonts
				Creating
			*/

			//Create a font with the given name, size, face index, character encoding and glyph min/mag filter
			Font& CreateFont(std::string name, int size, int face_index,
				font::CharacterEncoding encoding, font::GlyphFilter min_filter, font::GlyphFilter mag_filter);

			//Create a font with the given name, size, character encoding and glyph min/mag filter
			Font& CreateFont(std::string name, int size, font::CharacterEncoding encoding,
				font::GlyphFilter min_filter, font::GlyphFilter mag_filter);

			//Create a font with the given name, size, character encoding and glyph filter
			Font& CreateFont(std::string name, int size, font::CharacterEncoding encoding,
				font::GlyphFilter filter);

			//Create a font with the given name, size and character encoding
			Font& CreateFont(std::string name, int size, font::CharacterEncoding encoding);

			//Create a font with the given name and size
			Font& CreateFont(std::string name, int size);


			/*
				Fonts
				Removing
			*/

			//Clear all removable fonts from this manager
			void ClearFonts() noexcept;

			//Remove a removable font from this manager
			bool RemoveFont(Font &font) noexcept;
	};
} //ion::graphics::fonts

#endif