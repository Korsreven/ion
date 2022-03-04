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
#include <string_view>
#include <tuple>
#include <utility>

#include "IonFont.h"
#include "assets/repositories/IonFontRepository.h"
#include "memory/IonNonOwningPtr.h"
#include "resources/IonFileResourceManager.h"
#include "types/IonTypes.h"

#undef CreateFont

namespace ion::graphics::fonts
{
	namespace font_manager
	{
		namespace detail
		{
			std::optional<std::tuple<font::GlyphBitmapData, font::GlyphMetrices, int>> prepare_font(
				const std::string &file_data, int size, int face_index,
				int character_spacing, font::FontCharacterSet character_set);

			std::optional<font::GlyphTextureHandles> load_font(
				const font::GlyphBitmapData &glyph_data,
				const font::GlyphMetrices &glyph_metrics,
				font::FontGlyphFilter min_filter, font::FontGlyphFilter mag_filter) noexcept;
			void unload_font(const font::GlyphTextureHandles &glyph_handles) noexcept;
		} //detail
	} //font_manager


	class FontManager final :
		public resources::FileResourceManager<Font, FontManager, assets::repositories::FontRepository>
	{
		private:
			

		protected:

			/*
				Events
			*/

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
				return Resources();
			}

			//Returns an immutable range of all fonts in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Fonts() const noexcept
			{
				return Resources();
			}


			/*
				Fonts
				Creating
			*/

			//Create a font with the given name, asset name, size, face index, character spacing, character set and glyph min/mag filter
			NonOwningPtr<Font> CreateFont(std::string name, std::string asset_name, int size, int face_index,
				int character_spacing, font::FontCharacterSet character_set, font::FontGlyphFilter min_filter, font::FontGlyphFilter mag_filter);

			//Create a font with the given name, asset name, size, character spacing, character set and glyph min/mag filter
			NonOwningPtr<Font> CreateFont(std::string name, std::string asset_name, int size,
				int character_spacing, font::FontCharacterSet character_set, font::FontGlyphFilter min_filter, font::FontGlyphFilter mag_filter);

			//Create a font with the given name, asset name, size, character spacing, character set and glyph filter
			NonOwningPtr<Font> CreateFont(std::string name, std::string asset_name, int size,
				int character_spacing, font::FontCharacterSet character_set, font::FontGlyphFilter filter);

			//Create a font with the given name, asset name, size, character spacing and character set
			NonOwningPtr<Font> CreateFont(std::string name, std::string asset_name, int size,
				int character_spacing, font::FontCharacterSet character_set);

			//Create a font with the given name, asset name, size and character spacing
			NonOwningPtr<Font> CreateFont(std::string name, std::string asset_name, int size,
				int character_spacing = 0);


			//Create a font as a copy of the given font
			NonOwningPtr<Font> CreateFont(const Font &font);

			//Create a font by moving the given font
			NonOwningPtr<Font> CreateFont(Font &&font);


			/*
				Fonts
				Retrieving
			*/

			//Gets a pointer to a mutable font with the given name
			//Returns nullptr if font could not be found
			[[nodiscard]] NonOwningPtr<Font> GetFont(std::string_view name) noexcept;

			//Gets a pointer to an immutable font with the given name
			//Returns nullptr if font could not be found
			[[nodiscard]] NonOwningPtr<const Font> GetFont(std::string_view name) const noexcept;


			/*
				Fonts
				Removing
			*/

			//Clear all removable fonts from this manager
			void ClearFonts() noexcept;

			//Remove a removable font from this manager
			bool RemoveFont(Font &font) noexcept;

			//Remove a removable font with the given name from this manager
			bool RemoveFont(std::string_view name) noexcept;
	};
} //ion::graphics::fonts

#endif