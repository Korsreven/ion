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

	class TextureAtlas final : public Texture
	{
		private:
			
			int rows_ = 0;
			int columns_ = 0;
			int sub_textures_ = 0;
			texture_atlas::AtlasSubTextureOrder sub_texture_order_ = texture_atlas::AtlasSubTextureOrder::RowMajor;

		public:

			//Construct a new texture atlas with the given name, asset name, number of rows, columns, sub textures and sub texture order
			TextureAtlas(std::string name, std::string asset_name,
				int rows, int columns, int sub_textures, texture_atlas::AtlasSubTextureOrder sub_texture_order = texture_atlas::AtlasSubTextureOrder::RowMajor);


			/*
				Modifiers
			*/




			/*
				Observers
			*/

			//Returns the number of rows in this texture atlas
			[[nodiscard]] inline auto Rows() const noexcept
			{
				return rows_;
			}

			//Returns the number of columns in this texture atlas
			[[nodiscard]] inline auto Columns() const noexcept
			{
				return columns_;
			}

			//Returns the number of sub textures in this texture atlas
			[[nodiscard]] inline auto SubTextures() const noexcept
			{
				return sub_textures_;
			}

			//Returns the sub texture order for this texture atlas
			[[nodiscard]] inline auto SubTextureOrder() const noexcept
			{
				return sub_texture_order_;
			}
	};
} //ion::graphics::textures

#endif