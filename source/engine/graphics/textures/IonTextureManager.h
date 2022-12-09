/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/textures
File:	IonTextureManager.h
-------------------------------------------
*/

#ifndef ION_TEXTURE_MANAGER_H
#define ION_TEXTURE_MANAGER_H

#include <cmath>
#include <filesystem>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include "IonTexture.h"
#include "IonTextureAtlas.h"
#include "assets/repositories/IonImageRepository.h"
#include "memory/IonNonOwningPtr.h"
#include "resources/IonFileResourceManager.h"
#include "types/IonTypes.h"
#include "utilities/IonStringUtility.h"

#undef max

namespace ion::graphics::textures
{
	namespace texture_manager
	{
		enum class NpotResizing : bool
		{
			ResizeCanvas,
			ResampleImage
		};

		enum class NpotSampling
		{
			Nearest,
			Up,
			Down
		};

		enum class NpotResampleFit : bool
		{
			Horizontally,
			Vertically
		};

		enum class NpotResampleFilter
		{
			Box,
			Bicubic,
			Bilinear,
			BSpline,
			CatmullRom,
			Lanczos3
		};

		namespace detail
		{
			bool has_support_for_non_power_of_two_textures() noexcept;
			bool has_support_for_array_texture() noexcept;

			int max_texture_size() noexcept;
			int max_array_texture_layers() noexcept;


			constexpr auto is_power_of_two(uint32 x) noexcept
			{
				return x && !(x & (x - 1));
			}

			constexpr auto lower_power_of_two(uint32 x) noexcept
			{
				x |= x >> 1;
				x |= x >> 2;
				x |= x >> 4;
				x |= x >> 8;
				x |= x >> 16;
				return x - (x >> 1);
			}

			constexpr auto upper_power_of_two(uint32 x) noexcept
			{
				--x;
				x |= x >> 1;
				x |= x >> 2;
				x |= x >> 4;
				x |= x >> 8;
				x |= x >> 16;
				++x;
				return x;
			}

			constexpr auto nearest_power_of_two(uint32 x) noexcept
			{
				auto lower = lower_power_of_two(x);
				auto upper = upper_power_of_two(x);
				return x - lower < upper - x ? lower : upper;
			}

			constexpr auto make_power_of_two(uint32 x, NpotSampling npot_sampling) noexcept
			{
				switch (npot_sampling)
				{
					case NpotSampling::Up:
					return upper_power_of_two(x);

					case NpotSampling::Down:
					return lower_power_of_two(x);

					case NpotSampling::Nearest:
					default:
					return nearest_power_of_two(x);
				}
			}

			inline auto number_of_mipmap_levels(int width, int height) noexcept
			{
				return static_cast<int>(std::log2(std::max(width, height)));
			}


			std::pair<int, int> power_of_two_adjusted_size(int width, int height,
				NpotSampling npot_sampling, std::optional<NpotResampleFit> npot_resample_fit) noexcept;
			
			std::tuple<int, int, int, int> padding(int width, int height, int actual_width, int actual_height) noexcept;
			std::tuple<int, int, int, int> power_of_two_padding(int width, int height) noexcept;

			void enlarge_canvas(std::string &pixel_data, int left, int bottom, const texture::TextureExtents &extents) noexcept;


			std::optional<std::pair<std::string, texture::TextureExtents>> prepare_texture(
				const std::string &file_data, const std::filesystem::path &file_path,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter,
				std::optional<NpotResizing> npot_resizing, NpotSampling npot_sampling,
				std::optional<NpotResampleFit> npot_resample_fit, NpotResampleFilter npot_resample_filter);

			std::optional<texture::TextureHandle> load_texture(const std::string &pixel_data, const texture::TextureExtents &extents,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
				texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode) noexcept;
			void unload_texture(texture::TextureHandle texture_handle) noexcept;


			/**
				@name Sub textures
				@{
			*/

			void next_sub_texture_position(std::pair<int, int> &position, int rows, int columns,
				texture_atlas::AtlasSubTextureOrder sub_texture_order) noexcept;

			std::optional<std::pair<std::string, texture::TextureExtents>> prepare_sub_texture(
				const TextureAtlas &texture_atlas, const std::pair<int, int> &position,
				std::optional<NpotResizing> npot_resizing) noexcept;

			///@}
		} //detail
	} //texture_manager


	///@brief A class that manages and stores textures and texture atlases
	class TextureManager final :
		public resources::FileResourceManager<Texture, TextureManager, assets::repositories::ImageRepository>
	{
		private:

			std::optional<texture_manager::NpotResizing> texture_npot_resizing_ = texture_manager::NpotResizing::ResizeCanvas;
			texture_manager::NpotSampling texture_npot_sampling_ = texture_manager::NpotSampling::Nearest;
			std::optional<texture_manager::NpotResampleFit> texture_npot_resample_fit_;
			texture_manager::NpotResampleFilter texture_npot_resample_filter_ = texture_manager::NpotResampleFilter::Bilinear;


			template <typename... Args>
			void CreateSubTextures(NonOwningPtr<TextureAtlas> texture_atlas, Args &&...args)
			{
				using namespace ion::utilities;

				if (texture_atlas)
				{
					auto &name = *texture_atlas->Name();

					texture::TextureAtlasRegion atlas_region;
					atlas_region.Atlas = texture_atlas;
					atlas_region.Position = {1, 1};

					for (auto i = 0; i < texture_atlas->SubTextures(); ++i)
					{
						CreateResource(string::Concat(name, "_", i + 1), texture_atlas->AssetName(), atlas_region,
							std::forward<Args>(args)...);
						texture_manager::detail::next_sub_texture_position(atlas_region.Position,
							texture_atlas->Rows(), texture_atlas->Columns(), texture_atlas->SubTextureOrder());
					}
				}
			}		

		protected:

			/**
				@name Events
				@{
			*/

			bool PrepareResource(Texture &texture) override;
			bool LoadResource(Texture &texture) override;
			bool UnloadResource(Texture &texture) noexcept override;


			///@brief See FileResourceManager::ResourceLoaded for more details
			void ResourceLoaded(Texture &texture) noexcept override;

			///@brief See FileResourceManager::ResourceFailed for more details
			void ResourceFailed(Texture &texture) noexcept override;

			///@}

		public:

			///@brief Default constructor
			TextureManager() noexcept;

			///@brief Deleted copy constructor
			TextureManager(const TextureManager&) = delete;

			///@brief Default move constructor
			TextureManager(TextureManager&&) = default;

			///@brief Destructor
			~TextureManager() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			TextureManager& operator=(const TextureManager&) = delete;

			///@brief Default move assignment
			TextureManager& operator=(TextureManager&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all textures in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Textures() noexcept
			{
				return Resources();
			}

			///@brief Returns an immutable range of all textures in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Textures() const noexcept
			{
				return Resources();
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the texture resizing used by this manager to the given value
			inline void TextureNpotResizing(std::optional<texture_manager::NpotResizing> npot_resizing) noexcept
			{
				texture_npot_resizing_ = npot_resizing;
			}	

			///@brief Sets the texture sampling used by this manager to the given value
			inline void TextureNpotSampling(texture_manager::NpotSampling npot_sampling) noexcept
			{
				texture_npot_sampling_ = npot_sampling;
			}	

			///@brief Sets the texture resample fit used by this manager to the given value
			inline void TextureNpotResampleFit(std::optional<texture_manager::NpotResampleFit> npot_resample_fit) noexcept
			{
				texture_npot_resample_fit_ = npot_resample_fit;
			}

			///@brief Sets the texture resample filter used by this manager to the given value
			inline void TextureNpotResampleFilter(texture_manager::NpotResampleFilter npot_resample_filter) noexcept
			{
				texture_npot_resample_filter_ = npot_resample_filter;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the texture resizing used by this manager when resizing from NPOT to POT
			///@details Returns nullopt if NPOT textures should not be resized to POT
			[[nodiscard]] inline auto& TextureNpotResizing() const noexcept
			{
				return texture_npot_resizing_;
			}

			///@brief Returns the texture sampling used by this manager when resampling from NPOT to POT
			[[nodiscard]] inline auto& TextureNpotSampling() const noexcept
			{
				return texture_npot_sampling_;
			}

			///@brief Returns the texture resample fit used by this manager when resampling from NPOT to POT
			///@details Returns nullopt if best fit is automatically calculated
			[[nodiscard]] inline auto& TextureNpotResampleFit() const noexcept
			{
				return texture_npot_resample_fit_;
			}

			///@brief Returns the texture resample filter used by this manager when resampling from NPOT to POT
			[[nodiscard]] inline auto TextureNpotResampleFilter() const noexcept
			{
				return texture_npot_resample_filter_;
			}		

			///@}

			/**
				@name Textures - Creating
				@{
			*/

			///@brief Creates a texture with the given name and asset name
			NonOwningPtr<Texture> CreateTexture(std::string name, std::string asset_name);

			///@brief Creates a texture with the given name, asset name, texture filter for min/mag, mip filter and texture wrap for s/t
			NonOwningPtr<Texture> CreateTexture(std::string name, std::string asset_name,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
				texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode);

			///@brief Creates a texture with the given name, asset name, texture filter, mip filter and texture wrap
			NonOwningPtr<Texture> CreateTexture(std::string name, std::string asset_name,
				texture::TextureFilter filter, texture::MipmapFilter mip_filter, texture::TextureWrapMode wrap_mode);

			///@brief Creates a texture with the given name, asset name, texture filter and texture wrap (no mipmap)
			NonOwningPtr<Texture> CreateTexture(std::string name, std::string asset_name,
				texture::TextureFilter filter, texture::TextureWrapMode wrap_mode);


			///@brief Creates a texture as a copy of the given texture
			NonOwningPtr<Texture> CreateTexture(const Texture &texture);

			///@brief Creates a texture by moving the given texture
			NonOwningPtr<Texture> CreateTexture(Texture &&texture);

			///@}

			/**
				@name Texture atlases - Creating
				@{
			*/

			///@brief Creates a texture atlas with the given name, asset name, number of rows, columns, sub textures and sub texture order
			///@details Creates textures with names <name>_1 to <name>_N, where N is the number of sub textures in the atlas
			NonOwningPtr<TextureAtlas> CreateTextureAtlas(std::string name, std::string asset_name,
				int rows, int columns, std::optional<int> sub_textures = {}, texture_atlas::AtlasSubTextureOrder sub_texture_order = texture_atlas::AtlasSubTextureOrder::RowMajor);

			///@brief Creates a texture atlas with the given name, asset name, texture filter for min/mag, mip filter, texture wrap for s/t,
			///number of rows, columns, sub textures and sub texture order
			///@details Creates textures with names <name>_1 to <name>_N, where N is the number of sub textures in the atlas
			NonOwningPtr<TextureAtlas> CreateTextureAtlas(std::string name, std::string asset_name,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
				texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode,
				int rows, int columns, std::optional<int> sub_textures = {}, texture_atlas::AtlasSubTextureOrder sub_texture_order = texture_atlas::AtlasSubTextureOrder::RowMajor);

			///@brief Creates a texture atlas with the given name, asset name, texture filter, mip filter, texture wrap,
			///number of rows, columns, sub textures and sub texture order
			///@details Creates textures with names <name>_1 to <name>_N, where N is the number of sub textures in the atlas
			NonOwningPtr<TextureAtlas> CreateTextureAtlas(std::string name, std::string asset_name,
				texture::TextureFilter filter, texture::MipmapFilter mip_filter, texture::TextureWrapMode wrap_mode,
				int rows, int columns, std::optional<int> sub_textures = {}, texture_atlas::AtlasSubTextureOrder sub_texture_order = texture_atlas::AtlasSubTextureOrder::RowMajor);

			///@brief Creates a texture atlas with the given name, asset name, texture filter, texture wrap (no mipmap),
			///number of rows, columns, sub textures and sub texture order
			///@details Creates textures with names <name>_1 to <name>_N, where N is the number of sub textures in the atlas
			NonOwningPtr<TextureAtlas> CreateTextureAtlas(std::string name, std::string asset_name,
				texture::TextureFilter filter, texture::TextureWrapMode wrap_mode,
				int rows, int columns, std::optional<int> sub_textures = {}, texture_atlas::AtlasSubTextureOrder sub_texture_order = texture_atlas::AtlasSubTextureOrder::RowMajor);

			///@}

			/**
				@name Textures - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable texture with the given name
			///@details Returns nullptr if texture could not be found
			[[nodiscard]] NonOwningPtr<Texture> GetTexture(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable texture with the given name
			///@details Returns nullptr if texture could not be found
			[[nodiscard]] NonOwningPtr<const Texture> GetTexture(std::string_view name) const noexcept;

			///@}

			/**
				@name Textures - Removing
				@{
			*/

			///@brief Clears all removable textures from this manager
			void ClearTextures() noexcept;

			///@brief Removes a removable texture from this manager
			bool RemoveTexture(Texture &texture) noexcept;

			///@brief Removes a removable texture with the given name from this manager
			bool RemoveTexture(std::string_view name) noexcept;

			///@}
	};
} //ion::graphics::textures

#endif