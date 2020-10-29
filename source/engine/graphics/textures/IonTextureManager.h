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
#include "assets/repositories/IonImageRepository.h"
#include "resources/IonFileResourceManager.h"
#include "types/IonTypes.h"

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
			int max_texture_size() noexcept;


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


			std::optional<std::pair<std::string, texture::TextureExtents>> prepare_texture(
				const std::string &file_data, const std::filesystem::path &file_path,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter,
				std::optional<NpotScale> npot_scale, std::optional<NpotScaleFit> npot_scale_fit,
				NpotScaleResampling npot_scale_resampling);

			std::optional<int> load_texture(const std::string &pixel_data, const texture::TextureExtents &extents,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
				texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode) noexcept;
			void unload_texture(int texture_handle) noexcept;
		} //detail
	} //texture_manager


	class TextureManager final :
		public resources::FileResourceManager<Texture, TextureManager, assets::repositories::ImageRepository>
	{
		private:

			std::optional<texture_manager::NpotScale> texture_npot_scale_ = texture_manager::NpotScale::ToNearest;
			std::optional<texture_manager::NpotScaleFit> texture_npot_scale_fit_;
			texture_manager::NpotScaleResampling texture_npot_scale_resampling_ = texture_manager::NpotScaleResampling::Bilinear;
			

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
			[[nodiscard]] inline const auto Textures() const noexcept
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
			Texture& CreateTexture(std::string name, std::string asset_name);

			//Create a texture with the given name, asset name, texture filter for min/mag, mip filter and texture wrap for s/t
			Texture& CreateTexture(std::string name, std::string asset_name,
				texture::TextureFilter min_filter, texture::TextureFilter mag_filter, std::optional<texture::MipmapFilter> mip_filter,
				texture::TextureWrapMode s_wrap_mode, texture::TextureWrapMode t_wrap_mode);

			//Create a texture with the given name, asset name, texture filter, mip filter and texture wrap
			Texture& CreateTexture(std::string name, std::string asset_name,
				texture::TextureFilter filter, texture::MipmapFilter mip_filter,
				texture::TextureWrapMode wrap_mode);

			//Create a texture with the given name, asset name, texture filter and texture wrap (no mipmap)
			Texture& CreateTexture(std::string name, std::string asset_name,
				texture::TextureFilter filter, texture::TextureWrapMode wrap_mode);


			//Create a texture as a copy of the given texture
			Texture& CreateTexture(const Texture &texture);

			//Create a texture by moving the given texture
			Texture& CreateTexture(Texture &&texture);


			/*
				Textures
				Retrieving
			*/

			//Gets a pointer to a mutable texture with the given name
			//Returns nullptr if texture could not be found
			[[nodiscard]] Texture* GetTexture(std::string_view name) noexcept;

			//Gets a pointer to an immutable texture with the given name
			//Returns nullptr if texture could not be found
			[[nodiscard]] const Texture* GetTexture(std::string_view name) const noexcept;


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