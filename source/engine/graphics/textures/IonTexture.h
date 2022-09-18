/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonTexture.h
-------------------------------------------
*/

#ifndef ION_TEXTURE_H
#define ION_TEXTURE_H

#include <optional>
#include <string>
#include <tuple>
#include <utility>

#include "graphics/utilities/IonVector2.h"
#include "memory/IonNonOwningPtr.h"
#include "resources/IonFileResource.h"

namespace ion::graphics::textures
{
	class TextureAtlas; //Forward declaration
	class TextureManager; //Forward declaration

	using graphics::utilities::Vector2;

	namespace texture
	{
		enum class TextureType
		{
			Texture1D,
			Texture2D,
			ArrayTexture1D,
			ArrayTexture2D
		};

		enum class TextureFilter : bool
		{
			NearestNeighbor,	//Nearest
			Bilinear			//Linear
		};

		enum class MipmapFilter : bool
		{
			ClosestMatch,		//Nearest
			WeightedAverage		//Linear
		};

		enum class TextureWrapMode : bool
		{
			Clamp,
			Repeat
		};


		struct TextureHandle final
		{
			int Id = 0;
			TextureType Type = TextureType::Texture2D;


			/*
				Operators
			*/

			//Checks if two texture handles are equal (all members are equal)
			[[nodiscard]] inline auto operator==(const TextureHandle &rhs) const noexcept
			{
				return Id == rhs.Id &&
					   Type == rhs.Type;
			}

			//Checks if two texture handles are different (one or more members are different)
			[[nodiscard]] inline auto operator!=(const TextureHandle &rhs) const noexcept
			{
				return !(*this == rhs);
			}
		};

		struct TextureExtents final
		{
			int Width = 0, Height = 0;
			int ActualWidth = 0, ActualHeight = 0;
			int BitDepth = 0;
		};

		struct TextureAtlasRegion final
		{
			NonOwningPtr<TextureAtlas> Atlas;
			std::pair<int, int> Position; //Row and column
		};


		namespace detail
		{
			int texture_type_to_gl_texture_type(TextureType texture_type) noexcept;
		} //detail
	} //texture


	//A class representing a texture with min, mag and mip filters that can be clamped or repeated
	//A texture can be a single file, or part of an texture atlas
	class Texture : public resources::FileResource<TextureManager>
	{
		private:

			texture::TextureFilter min_filter_ = texture::TextureFilter::Bilinear;
			texture::TextureFilter mag_filter_ = texture::TextureFilter::Bilinear;		
			std::optional<texture::MipmapFilter> mip_filter_;

			texture::TextureWrapMode s_wrap_mode_ = texture::TextureWrapMode::Clamp;
			texture::TextureWrapMode t_wrap_mode_ = texture::TextureWrapMode::Clamp;

			std::optional<texture::TextureHandle> handle_;

			std::optional<std::string> pixel_data_;
			std::optional<texture::TextureExtents> extents_;
			std::optional<texture::TextureAtlasRegion> atlas_region_;

		public:

			using resources::FileResource<TextureManager>::FileResource;

			//Construct a new texture with the given name, asset name, texture filter for min/mag, mip filter and texture wrap mode for s/t
			Texture(std::string name, std::string asset_name,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
				texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode);

			//Construct a new texture with the given name, asset name, texture filter, mip filter and texture wrap mode
			Texture(std::string name, std::string asset_name,
				texture::TextureFilter filter, texture::MipmapFilter mip_filter, texture::TextureWrapMode wrap_mode);

			//Construct a new texture with the given name, asset name, texture filter and texture wrap mode (no mipmap)
			Texture(std::string name, std::string asset_name,
				texture::TextureFilter filter, texture::TextureWrapMode wrap_mode);


			//Construct a new sub texture with the given name, asset name and atlas region
			Texture(std::string name, std::string asset_name, const texture::TextureAtlasRegion &atlas_region);

			//Construct a new sub texture with the given name, asset name, atlas region, texture filter for min/mag, mip filter and texture wrap mode for s/t
			Texture(std::string name, std::string asset_name, const texture::TextureAtlasRegion &atlas_region,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
				texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode);

			//Construct a new sub texture with the given name, asset name, atlas region, texture filter, mip filter and texture wrap mode
			Texture(std::string name, std::string asset_name, const texture::TextureAtlasRegion &atlas_region,
				texture::TextureFilter filter, texture::MipmapFilter mip_filter, texture::TextureWrapMode wrap_mode);

			//Construct a new sub texture with the given name, asset name, atlas region, texture filter and texture wrap mode (no mipmap)
			Texture(std::string name, std::string asset_name, const texture::TextureAtlasRegion &atlas_region,
				texture::TextureFilter filter, texture::TextureWrapMode wrap_mode);


			//Default copy constructor
			Texture(const Texture&) = default;

			//Default move constructor
			Texture(Texture&&) = default;

			//Virtual destructor
			virtual ~Texture() = default;


			/*
				Operators
			*/

			//Default copy assignment
			Texture& operator=(const Texture&) = default;

			//Default move assignment
			Texture& operator=(Texture&&) = default;


			/*
				Modifiers
			*/

			//Sets the handle for the texture to the given handle
			inline void Handle(std::optional<texture::TextureHandle> handle) noexcept
			{
				handle_ = handle;
			}

			//Sets the pixel data of the texture to the given data
			inline void PixelData(std::string data, const texture::TextureExtents &extents)
			{
				pixel_data_ = std::move(data);
				extents_ = extents;
			}

			//Resets the pixel data to save some memory (if not needed anymore)
			inline void ResetPixelData() noexcept
			{
				pixel_data_.reset();
			}


			/*
				Observers
			*/

			//Returns the handle for the texture
			//Returns nullopt if the texture is not loaded
			[[nodiscard]] inline auto Handle() const noexcept
			{
				return handle_;
			}

			//Returns the pixel data of the texture
			//Returns nullopt if the texture has not been prepared yet, or is no longer needed (fully loaded or has failed)
			[[nodiscard]] inline auto& PixelData() const noexcept
			{
				return pixel_data_;
			}

			//Returns the extents of the texture
			//Returns nullopt if the texture has not been prepared yet
			[[nodiscard]] inline auto& Extents() const noexcept
			{
				return extents_;
			}

			//Returns the atlas region of the texture
			//Returns nullopt if the texture is not a sub texture
			[[nodiscard]] inline auto& AtlasRegion() const noexcept
			{
				return atlas_region_;
			}


			//Returns the min, mag and mip texture filters
			[[nodiscard]] inline auto Filter() const noexcept
			{
				return std::tuple{min_filter_, mag_filter_, mip_filter_};
			}

			//Returns the min (minifying) texture filter
			[[nodiscard]] inline auto MinFilter() const noexcept
			{
				return min_filter_;
			}

			//Returns the mag (magnifying) texture filter
			[[nodiscard]] inline auto MagFilter() const noexcept
			{
				return mag_filter_;
			}

			//Returns the mip (mipmap) filter
			//Returns nullopt if mipmapping is turned off
			[[nodiscard]] inline auto MipFilter() const noexcept
			{
				return mip_filter_;
			}


			//Returns both the s and t texture wrap mode
			[[nodiscard]] inline auto WrapMode() const noexcept
			{
				return std::pair{s_wrap_mode_, t_wrap_mode_};
			}

			//Returns the s texture wrap mode
			[[nodiscard]] inline auto SWrapMode() const noexcept
			{
				return s_wrap_mode_;
			}

			//Returns the t texture wrap mode
			[[nodiscard]] inline auto TWrapMode() const noexcept
			{
				return t_wrap_mode_;
			}


			/*
				Texture coordinates
			*/

			//Returns the lower left and upper right texture coordinates for the texture
			//This is important if one or both of the texture sides are NPOT
			//Returns nullopt if the texture is missing extents (not loaded)
			[[nodiscard]] std::optional<std::pair<Vector2, Vector2>> TexCoords() const noexcept;

			//Returns a pair of true/false to indicate which texture axis is repeatable
			//Returns nullopt if the texture is missing extents (not loaded)
			[[nodiscard]] std::optional<std::pair<bool, bool>> IsRepeatable() const noexcept;
	};
} //ion::graphics::textures

#endif