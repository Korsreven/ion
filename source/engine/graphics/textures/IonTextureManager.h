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
		enum class NpotScale
		{
			ToNearest,
			ToLarger,
			ToSmaller
		};

		enum class NpotScaleFit : bool
		{
			Horizontally,
			Vertically
		};

		enum class NpotScaleResampling
		{
			Bilinear,
			Bicubic,
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

			constexpr auto make_power_of_two(uint32 x, NpotScale npot_scale) noexcept
			{
				switch (npot_scale)
				{
					case NpotScale::ToLarger:
					return upper_power_of_two(x);

					case NpotScale::ToSmaller:
					return lower_power_of_two(x);

					case NpotScale::ToNearest:
					default:
					return nearest_power_of_two(x);
				}
			}

			inline auto number_of_mipmap_levels(int width, int height) noexcept
			{
				return static_cast<int>(std::log2(std::max(width, height)));
			}


			inline auto power_of_two_adjusted_size(int width, int height,
				NpotScale npot_scale, std::optional<NpotScaleFit> npot_scale_fit) noexcept
			{
				auto aspect_ratio = static_cast<real>(width) / height;

				//Max texture limit reached
				if (width > max_texture_size() || height > max_texture_size())
				{
					if (width > height)
					{
						width = max_texture_size();
						height = static_cast<int>(width / aspect_ratio);
					}
					else
					{
						height = max_texture_size();
						width = static_cast<int>(height * aspect_ratio);
					}
				}
				else
				{
					auto pot_width = static_cast<int>(make_power_of_two(width, npot_scale));
					auto pot_height = static_cast<int>(make_power_of_two(height, npot_scale));

					if (!npot_scale_fit)
						npot_scale_fit =
							[&]() noexcept
							{
								//Choose minimum npot->pot difference
								if (std::abs(width - pot_width) < std::abs(height - pot_height))
									return NpotScaleFit::Horizontally;
								else
									return NpotScaleFit::Vertically;
							}();

					switch (*npot_scale_fit)
					{
						case NpotScaleFit::Horizontally:
						{
							width = pot_width;
							height = static_cast<int>(width / aspect_ratio);
							break;
						}

						case NpotScaleFit::Vertically:
						{
							height = pot_height;
							width = static_cast<int>(height * aspect_ratio);
							break;
						}
					}
				}

				return std::pair{width, height};
			}

			inline auto power_of_two_padding(int width, int height) noexcept
			{
				if (auto is_width_pot = is_power_of_two(width),
						 is_height_pot = is_power_of_two(height); !is_width_pot || !is_height_pot)
				{
					//Pad left and right
					if (!is_width_pot)
					{
						auto padding_width = static_cast<int>(upper_power_of_two(width)) - width;
						auto padding_half_width = padding_width / 2;
						return std::tuple{padding_half_width, 0, padding_half_width + padding_width % 2, 0};
					}
					//Pad top and bottom
					else
					{
						auto padding_height = static_cast<int>(upper_power_of_two(height)) - height;
						auto padding_half_height = padding_height / 2;
						return std::tuple{0, padding_half_height, 0, padding_half_height + padding_height % 2};
					}
				}
				else
					return std::tuple{0, 0, 0, 0};
			}


			std::optional<std::pair<std::string, texture::TextureExtents>> prepare_texture(
				const std::string &file_data, const std::filesystem::path &file_path,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter,
				std::optional<NpotScale> npot_scale, std::optional<NpotScaleFit> npot_scale_fit,
				NpotScaleResampling npot_scale_resampling);

			std::optional<texture::TextureHandle> load_texture(const std::string &pixel_data, const texture::TextureExtents &extents,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
				texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode) noexcept;
			void unload_texture(texture::TextureHandle texture_handle) noexcept;


			/*
				Sub textures
			*/

			void next_sub_texture_position(std::pair<int, int> &position, int rows, int columns,
				texture_atlas::AtlasSubTextureOrder sub_texture_order) noexcept;

			std::optional<std::pair<std::string, texture::TextureExtents>> prepare_sub_texture(
				const TextureAtlas &texture_atlas, const std::pair<int, int> &position,
				std::optional<NpotScale> npot_scale) noexcept;
		} //detail
	} //texture_manager


	class TextureManager final :
		public resources::FileResourceManager<Texture, TextureManager, assets::repositories::ImageRepository>
	{
		private:

			std::optional<texture_manager::NpotScale> texture_npot_scale_ = texture_manager::NpotScale::ToNearest;
			std::optional<texture_manager::NpotScaleFit> texture_npot_scale_fit_;
			texture_manager::NpotScaleResampling texture_npot_scale_resampling_ = texture_manager::NpotScaleResampling::Bilinear;


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

			/*
				Events
			*/

			bool PrepareResource(Texture &texture) noexcept override;
			bool LoadResource(Texture &texture) noexcept override;
			bool UnloadResource(Texture &texture) noexcept override;


			//See FileResourceManager::ResourceLoaded for more details
			void ResourceLoaded(Texture &texture) noexcept override;

			//See FileResourceManager::ResourceFailed for more details
			void ResourceFailed(Texture &texture) noexcept override;

		public:

			//Default constructor
			TextureManager() noexcept;

			//Deleted copy constructor
			TextureManager(const TextureManager&) = delete;

			//Default move constructor
			TextureManager(TextureManager&&) = default;

			//Destructor
			~TextureManager() noexcept;


			/*
				Operators
			*/

			//Deleted copy assignment
			TextureManager& operator=(const TextureManager&) = delete;

			//Move assignment
			TextureManager& operator=(TextureManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all textures in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Textures() noexcept
			{
				return Resources();
			}

			//Returns an immutable range of all textures in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Textures() const noexcept
			{
				return Resources();
			}


			/*
				Modifiers
			*/

			//Sets the texture scale used by this manager to the given value
			inline void TextureNpotScale(std::optional<texture_manager::NpotScale> npot_scale) noexcept
			{
				texture_npot_scale_ = npot_scale;
			}	

			//Sets the texture scale fit used by this manager to the given value
			inline void TextureNpotScaleFit(std::optional<texture_manager::NpotScaleFit> npot_scale_fit) noexcept
			{
				texture_npot_scale_fit_ = npot_scale_fit;
			}

			//Sets the texture scale resampling used by this manager to the given value
			inline void TextureNpotScaleResampling(texture_manager::NpotScaleResampling npot_scale_resampling) noexcept
			{
				texture_npot_scale_resampling_ = npot_scale_resampling;
			}


			/*
				Observers
			*/


			//Returns the texture scale used by this manager when converting from NPOT to POT
			//Returns nullopt if NPOT textures should not be scaled to POT
			[[nodiscard]] inline auto& TextureNpotScale() const noexcept
			{
				return texture_npot_scale_;
			}

			//Returns the texture scale fit used by this manager when converting from NPOT to POT
			//Returns nullopt if best fit is automatically calculated
			[[nodiscard]] inline auto& TextureNpotScaleFit() const noexcept
			{
				return texture_npot_scale_fit_;
			}

			//Returns the texture scale resampling used by this manager when converting from NPOT to POT
			[[nodiscard]] inline auto TextureNpotScaleResampling() const noexcept
			{
				return texture_npot_scale_resampling_;
			}		


			/*
				Textures
				Creating
			*/

			//Create a texture with the given name and asset name
			NonOwningPtr<Texture> CreateTexture(std::string name, std::string asset_name);

			//Create a texture with the given name, asset name, texture filter for min/mag, mip filter and texture wrap for s/t
			NonOwningPtr<Texture> CreateTexture(std::string name, std::string asset_name,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
				texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode);

			//Create a texture with the given name, asset name, texture filter, mip filter and texture wrap
			NonOwningPtr<Texture> CreateTexture(std::string name, std::string asset_name,
				texture::TextureFilter filter, texture::MipmapFilter mip_filter, texture::TextureWrapMode wrap_mode);

			//Create a texture with the given name, asset name, texture filter and texture wrap (no mipmap)
			NonOwningPtr<Texture> CreateTexture(std::string name, std::string asset_name,
				texture::TextureFilter filter, texture::TextureWrapMode wrap_mode);


			//Create a texture as a copy of the given texture
			NonOwningPtr<Texture> CreateTexture(const Texture &texture);

			//Create a texture by moving the given texture
			NonOwningPtr<Texture> CreateTexture(Texture &&texture);


			/*
				Texture atlases
				Creating
			*/

			//Create a texture atlas with the given name, asset name, number of rows, columns, sub textures and sub texture order
			//Creates textures with names <name>_1 to <name>_N, where N is the number of sub textures in the atlas
			NonOwningPtr<TextureAtlas> CreateTextureAtlas(std::string name, std::string asset_name,
				int rows, int columns, int sub_textures, texture_atlas::AtlasSubTextureOrder sub_texture_order = texture_atlas::AtlasSubTextureOrder::RowMajor);

			//Create a texture atlas with the given name, asset name, texture filter for min/mag, mip filter, texture wrap for s/t,
			//number of rows, columns, sub textures and sub texture order
			//Creates textures with names <name>_1 to <name>_N, where N is the number of sub textures in the atlas
			NonOwningPtr<TextureAtlas> CreateTextureAtlas(std::string name, std::string asset_name,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
				texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode,
				int rows, int columns, int sub_textures, texture_atlas::AtlasSubTextureOrder sub_texture_order = texture_atlas::AtlasSubTextureOrder::RowMajor);

			//Create a texture atlas with the given name, asset name, texture filter, mip filter, texture wrap,
			//number of rows, columns, sub textures and sub texture order
			//Creates textures with names <name>_1 to <name>_N, where N is the number of sub textures in the atlas
			NonOwningPtr<TextureAtlas> CreateTextureAtlas(std::string name, std::string asset_name,
				texture::TextureFilter filter, texture::MipmapFilter mip_filter, texture::TextureWrapMode wrap_mode,
				int rows, int columns, int sub_textures, texture_atlas::AtlasSubTextureOrder sub_texture_order = texture_atlas::AtlasSubTextureOrder::RowMajor);

			//Create a texture atlas with the given name, asset name, texture filter, texture wrap (no mipmap),
			//number of rows, columns, sub textures and sub texture order
			//Creates textures with names <name>_1 to <name>_N, where N is the number of sub textures in the atlas
			NonOwningPtr<TextureAtlas> CreateTextureAtlas(std::string name, std::string asset_name,
				texture::TextureFilter filter, texture::TextureWrapMode wrap_mode,
				int rows, int columns, int sub_textures, texture_atlas::AtlasSubTextureOrder sub_texture_order = texture_atlas::AtlasSubTextureOrder::RowMajor);


			/*
				Textures
				Retrieving
			*/

			//Gets a pointer to a mutable texture with the given name
			//Returns nullptr if texture could not be found
			[[nodiscard]] NonOwningPtr<Texture> GetTexture(std::string_view name) noexcept;

			//Gets a pointer to an immutable texture with the given name
			//Returns nullptr if texture could not be found
			[[nodiscard]] NonOwningPtr<const Texture> GetTexture(std::string_view name) const noexcept;


			/*
				Textures
				Removing
			*/

			//Clear all removable textures from this manager
			void ClearTextures() noexcept;

			//Remove a removable texture from this manager
			bool RemoveTexture(Texture &texture) noexcept;

			//Remove a removable texture with the given name from this manager
			bool RemoveTexture(std::string_view name) noexcept;
	};
} //ion::graphics::textures

#endif