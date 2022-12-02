/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonTextureAtlas.h
-------------------------------------------
*/

#ifndef ION_TEXTURE_ATLAS_H
#define ION_TEXTURE_ATLAS_H

#include <optional>
#include "IonTexture.h"

namespace ion::graphics::textures
{
	namespace texture_atlas
	{
		enum class AtlasSubTextureOrder : bool
		{
			RowMajor,
			ColumnMajor
		};

		namespace detail
		{
		} //detail
	} //texture_atlas


	///@brief A class representing a texture atlas with rows and columns
	///@details A texture atlas is itself a texture that contains multiple (sub) textures
	class TextureAtlas final : public Texture
	{
		private:
			
			int rows_ = 0;
			int columns_ = 0;
			int sub_textures_ = 0;
			texture_atlas::AtlasSubTextureOrder sub_texture_order_ = texture_atlas::AtlasSubTextureOrder::RowMajor;

		public:

			///@brief Constructs a new texture atlas with the given name, asset name, number of rows, columns, sub textures and sub texture order
			TextureAtlas(std::string name, std::string asset_name,
				int rows, int columns, std::optional<int> sub_textures = {}, texture_atlas::AtlasSubTextureOrder sub_texture_order = texture_atlas::AtlasSubTextureOrder::RowMajor) noexcept;

			///@brief Constructs a new texture atlas with the given name, asset name, texture filter for min/mag, number of rows, columns, sub textures and sub texture order
			TextureAtlas(std::string name, std::string asset_name,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter,
				int rows, int columns, std::optional<int> sub_textures = {}, texture_atlas::AtlasSubTextureOrder sub_texture_order = texture_atlas::AtlasSubTextureOrder::RowMajor) noexcept;

			///@brief Constructs a new texture atlas with the given name, asset name, texture filter, number of rows, columns, sub textures and sub texture order
			TextureAtlas(std::string name, std::string asset_name, texture::TextureFilter filter,
				int rows, int columns, std::optional<int> sub_textures = {}, texture_atlas::AtlasSubTextureOrder sub_texture_order = texture_atlas::AtlasSubTextureOrder::RowMajor) noexcept;


			/**
				@name Observers
				@{
			*/

			///@brief Returns the number of rows in this texture atlas
			[[nodiscard]] inline auto Rows() const noexcept
			{
				return rows_;
			}

			///@brief Returns the number of columns in this texture atlas
			[[nodiscard]] inline auto Columns() const noexcept
			{
				return columns_;
			}

			///@brief Returns the number of sub textures in this texture atlas
			[[nodiscard]] inline auto SubTextures() const noexcept
			{
				return sub_textures_;
			}

			///@brief Returns the sub texture order for this texture atlas
			[[nodiscard]] inline auto SubTextureOrder() const noexcept
			{
				return sub_texture_order_;
			}

			///@}
	};
} //ion::graphics::textures

#endif