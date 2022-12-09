/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonTexture.cpp
-------------------------------------------
*/

#include "IonTexture.h"

#include "IonTextureManager.h"
#include "graphics/IonGraphicsAPI.h"
#include "types/IonTypes.h"

namespace ion::graphics::textures
{

using namespace texture;
using namespace types::type_literals;

namespace texture::detail
{

int texture_type_to_gl_texture_type(TextureType texture_type) noexcept
{
	switch (texture_type)
	{
		case TextureType::Texture1D:
		return GL_TEXTURE_1D;

		case TextureType::ArrayTexture1D:
		return GL_TEXTURE_1D_ARRAY;

		case TextureType::ArrayTexture2D:
		return GL_TEXTURE_2D_ARRAY;

		case TextureType::Texture2D:
		default:
		return GL_TEXTURE_2D;
	}
}

} //texture::detail


//Public

Texture::Texture(std::string name, std::string asset_name,
	TextureFilter min_filter, TextureFilter mag_filter, std::optional<MipmapFilter> mip_filter,
	TextureWrapMode s_wrap_mode, TextureWrapMode t_wrap_mode) noexcept :

	FileResource{std::move(name), std::move(asset_name)},

	min_filter_{min_filter},
	mag_filter_{mag_filter},
	mip_filter_{mip_filter},

	s_wrap_mode_{s_wrap_mode},
	t_wrap_mode_{t_wrap_mode}
{
	//Empty
}

Texture::Texture(std::string name, std::string asset_name,
	TextureFilter filter, MipmapFilter mip_filter, TextureWrapMode wrap_mode) noexcept :

	Texture{std::move(name), std::move(asset_name), filter, filter, mip_filter, wrap_mode, wrap_mode}
{
	//Empty
}

Texture::Texture(std::string name, std::string asset_name,
	TextureFilter filter, TextureWrapMode wrap_mode) noexcept :

	Texture{std::move(name), std::move(asset_name), filter, filter, {}, wrap_mode, wrap_mode}
{
	//Empty
}


Texture::Texture(std::string name, std::string asset_name, const TextureAtlasRegion &atlas_region) noexcept :

	FileResource{std::move(name), std::move(asset_name)},
	atlas_region_{atlas_region}
{
	//Empty
}

Texture::Texture(std::string name, std::string asset_name, const TextureAtlasRegion &atlas_region,
	TextureFilter min_filter, TextureFilter mag_filter, std::optional<MipmapFilter> mip_filter,
	TextureWrapMode s_wrap_mode, TextureWrapMode t_wrap_mode) noexcept :

	FileResource{std::move(name), std::move(asset_name)},

	min_filter_{min_filter},
	mag_filter_{mag_filter},
	mip_filter_{mip_filter},

	s_wrap_mode_{s_wrap_mode},
	t_wrap_mode_{t_wrap_mode},

	atlas_region_{atlas_region}
{
	//Empty
}

Texture::Texture(std::string name, std::string asset_name, const TextureAtlasRegion &atlas_region,
	TextureFilter filter, MipmapFilter mip_filter, TextureWrapMode wrap_mode) noexcept :

	Texture{std::move(name), std::move(asset_name), atlas_region, filter, filter, mip_filter, wrap_mode, wrap_mode}
{
	//Empty
}

Texture::Texture(std::string name, std::string asset_name, const TextureAtlasRegion &atlas_region,
	TextureFilter filter, TextureWrapMode wrap_mode) noexcept :

	Texture{std::move(name), std::move(asset_name), atlas_region, filter, filter, {}, wrap_mode, wrap_mode}
{
	//Empty
}


/*
	Texture coordinates
*/

std::optional<std::pair<Vector2, Vector2>> Texture::TexCoords() const noexcept
{
	if (extents_)
	{
		auto [left, top, right, bottom] =
			texture_manager::detail::padding(extents_->Width, extents_->Height, extents_->ActualWidth, extents_->ActualHeight);

		auto w = static_cast<real>(extents_->ActualWidth);
		auto h = static_cast<real>(extents_->ActualHeight);
		return std::pair{Vector2{left / w, bottom / h}, Vector2{1.0_r - right / w, 1.0_r - top / h}};
	}
	else
		return {};
}

std::optional<std::pair<bool, bool>> Texture::IsRepeatable() const noexcept
{
	if (extents_)
		return std::pair{s_wrap_mode_ == TextureWrapMode::Repeat &&
							extents_->Width == extents_->ActualWidth,
							t_wrap_mode_ == TextureWrapMode::Repeat &&
							extents_->Height == extents_->ActualHeight};
	else
		return {};
}

} //ion::graphics::textures