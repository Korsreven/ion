/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/materials
File:	IonMaterial.h
-------------------------------------------
*/

#ifndef ION_MATERIAL_H
#define ION_MATERIAL_H

#include <optional>
#include <string>
#include <utility>
#include <variant>

#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "managed/IonManagedObject.h"
#include "memory/IonNonOwningPtr.h"
#include "types/IonTypes.h"

//Forward declarations
namespace ion::graphics
{
	namespace textures
	{
		class Animation;
		class Texture;
	}
}

namespace ion::graphics::materials
{
	struct MaterialManager; //Forward declaration

	using namespace types::type_literals;
	using namespace utilities;

	using textures::Animation;
	using textures::Texture;

	namespace material::detail
	{
		using texture_map_type = std::variant<
			std::monostate,
			NonOwningPtr<Animation>,
			NonOwningPtr<Texture>>;

		constexpr auto default_shininess = 32.0_r;


		/*
			Texure coordinates
		*/

		inline auto is_cropped(const Vector2 &lower_left, const Vector2 &upper_right) noexcept
		{
			auto [ll_s, ll_t] = lower_left.XY();
			auto [ur_s, ur_t] = upper_right.XY();

			return ll_s > 0.0_r || ll_t > 0.0_r ||
				   ur_s < 1.0_r || ur_t < 1.0_r;
		}

		inline auto is_repeated(const Vector2 &lower_left, const Vector2 &upper_right) noexcept
		{
			auto [ll_s, ll_t] = lower_left.XY();
			auto [ur_s, ur_t] = upper_right.XY();

			return ll_s < 0.0_r || ll_t < 0.0_r ||
				   ur_s > 1.0_r || ur_t > 1.0_r;
		}

		inline auto is_flipped_horizontally(const Vector2 &lower_left, const Vector2 &upper_right) noexcept
		{
			return upper_right.X() < lower_left.X();
		}

		inline auto is_flipped_vertically(const Vector2 &lower_left, const Vector2 &upper_right) noexcept
		{
			return upper_right.Y() < lower_left.Y();
		}

		std::pair<Vector2, Vector2> get_tex_coords(const Vector2 &lower_left, const Vector2 &upper_right,
			const Vector2 &new_lower_left, const Vector2 &new_upper_right) noexcept;
		std::pair<Vector2, Vector2> get_unflipped_tex_coords(const Vector2 &lower_left, const Vector2 &upper_right) noexcept;

		Vector2 get_normalized_tex_coord(const Vector2 &tex_coord, const Vector2 &min, const Vector2 &max,
			const Vector2 &new_min, const Vector2 &new_max) noexcept;
		std::pair<Vector2, Vector2> get_normalized_tex_coords(const Vector2 &lower_left, const Vector2 &upper_right,
			const Vector2 &min, const Vector2 &max) noexcept;


		/*
			Texture map
		*/

		std::pair<NonOwningPtr<Animation>, NonOwningPtr<Texture>> get_texture_maps(const texture_map_type &texture_map) noexcept;
		NonOwningPtr<Texture> get_texture_map(const texture_map_type &texture_map) noexcept;
		NonOwningPtr<Texture> get_first_texture_map(const texture_map_type &diffuse_map, const texture_map_type &normal_map,
			const texture_map_type &specular_map, const texture_map_type &emissive_map) noexcept;

		std::pair<bool, bool> is_texture_map_repeatable(const Texture &texture,
			const Vector2 &lower_left, const Vector2 &upper_right) noexcept;
	} //material::detail


	class Material final : public managed::ManagedObject<MaterialManager>
	{
		private:

			Color ambient_color_ = color::White;
			Color diffuse_color_ = color::White;
			Color specular_color_ = color::DarkGray;
			Color emissive_color_ = color::Black;
			real shininess_ = material::detail::default_shininess;

			material::detail::texture_map_type diffuse_map_;
			material::detail::texture_map_type normal_map_;
			material::detail::texture_map_type specular_map_;
			material::detail::texture_map_type emissive_map_;

			Vector2 lower_left_tex_coord_ = vector2::Zero;
			Vector2 upper_right_tex_coord_ = vector2::UnitScale;

			std::optional<real> emissive_light_radius_;
			bool lighting_enabled_ = true;
			bool receive_shadows_ = true;

		public:

			//Constructs a new material with the given name
			explicit Material(std::string name);

			//Constructs a new material with the given name and diffuse color
			Material(std::string name, const Color &diffuse);

			//Constructs a new material with the given name, diffuse map and diffuse color
			Material(std::string name, NonOwningPtr<Animation> diffuse_map, const Color &diffuse = color::White);

			//Constructs a new material with the given name, diffuse map and diffuse color
			Material(std::string name, NonOwningPtr<Texture> diffuse_map, const Color &diffuse = color::White);


			//Constructs a new material with the given name, ambient color, diffuse color, specular color, emissive color and shininess
			Material(std::string name,
				const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive,
				real shininess = material::detail::default_shininess);

			//Constructs a new material with the given name, diffuse map, normal map, specular map, emissive map,
			//diffuse color and shininess
			Material(std::string name, NonOwningPtr<Animation> diffuse_map,
				NonOwningPtr<Animation> normal_map, NonOwningPtr<Animation> specular_map, NonOwningPtr<Animation> emissive_map,
				const Color &diffuse = color::White, real shininess = material::detail::default_shininess);

			//Constructs a new material with the given name, diffuse map, normal map, specular map, emissive map,
			//diffuse color and shininess
			Material(std::string name, NonOwningPtr<Texture> diffuse_map,
				NonOwningPtr<Texture> normal_map, NonOwningPtr<Texture> specular_map, NonOwningPtr<Texture> emissive_map,
				const Color &diffuse = color::White, real shininess = material::detail::default_shininess);

			//Constructs a new material with the given name, diffuse map, normal map, specular map, emissive map,
			//ambient color, diffuse color, specular color, emissive color and shininess
			Material(std::string name, NonOwningPtr<Animation> diffuse_map,
				NonOwningPtr<Animation> normal_map, NonOwningPtr<Animation> specular_map, NonOwningPtr<Animation> emissive_map,
				const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive,
				real shininess = material::detail::default_shininess);

			//Constructs a new material with the given name, diffuse map, normal map, specular map, emissive map,
			//ambient color, diffuse color, specular color, emissive color and shininess
			Material(std::string name,
				NonOwningPtr<Texture> diffuse_map,
				NonOwningPtr<Texture> normal_map, NonOwningPtr<Texture> specular_map, NonOwningPtr<Texture> emissive_map,
				const Color &ambient, const Color &diffuse, const Color &specular, const Color &emissive,
				real shininess = material::detail::default_shininess);


			/*
				Modifiers
			*/

			//Sets the ambient color of this material to the given color
			inline void AmbientColor(const Color &ambient) noexcept
			{
				ambient_color_ = ambient;
			}
			
			//Sets the diffuse color of this material to the given color
			//Also sets the ambient color if equal to the diffuse color
			inline void DiffuseColor(const Color &diffuse) noexcept
			{
				if (diffuse_color_ == ambient_color_)
					ambient_color_ = diffuse;

				diffuse_color_ = diffuse;
			}
			
			//Sets the specular color of this material to the given color
			inline void SpecularColor(const Color &specular) noexcept
			{
				specular_color_ = specular;
			}

			//Sets the emissive (self-illumination) color of this material to the given color
			inline void EmissiveColor(const Color &emissive) noexcept
			{
				emissive_color_ = emissive;
			}

			//Sets the shininess of the material to the given value
			inline void Shininess(real shininess) noexcept
			{
				shininess_ = shininess;
			}


			//Attach the given animation as a diffuse map for the material
			inline void DiffuseMap(NonOwningPtr<Animation> animation) noexcept
			{
				diffuse_map_ = animation;
			}

			//Attach the given texture as a diffuse map for the material
			inline void DiffuseMap(NonOwningPtr<Texture> texture) noexcept
			{
				diffuse_map_ = texture;
			}

			//Detach the diffuse map from the material
			inline void DiffuseMap(std::nullptr_t) noexcept
			{
				diffuse_map_ = std::monostate{};
			}


			//Attach the given animation as a normal map for the material
			inline void NormalMap(NonOwningPtr<Animation> animation) noexcept
			{
				normal_map_ = animation;
			}

			//Attach the given texture as a normal map for the material
			inline void NormalMap(NonOwningPtr<Texture> texture) noexcept
			{
				normal_map_ = texture;
			}

			//Detach the normal map from the material
			inline void NormalMap(std::nullptr_t) noexcept
			{
				normal_map_ = std::monostate{};
			}


			//Attach the given animation as a specular map for the material
			inline void SpecularMap(NonOwningPtr<Animation> animation) noexcept
			{
				specular_map_ = animation;
			}

			//Attach the given texture as a specular map for the material
			inline void SpecularMap(NonOwningPtr<Texture> texture) noexcept
			{
				specular_map_ = texture;
			}

			//Detach the specular map from the material
			inline void SpecularMap(std::nullptr_t) noexcept
			{
				specular_map_ = std::monostate{};
			}


			//Attach the given animation as a emissive map for the material
			inline void EmissiveMap(NonOwningPtr<Animation> animation) noexcept
			{
				if (emissive_map_.index() == 0 && animation && emissive_color_ == color::Black)
					emissive_color_ = color::White;

				emissive_map_ = animation;
			}

			//Attach the given texture as a emissive map for the material
			inline void EmissiveMap(NonOwningPtr<Texture> texture) noexcept
			{
				if (emissive_map_.index() == 0 && texture && emissive_color_ == color::Black)
					emissive_color_ = color::White;

				emissive_map_ = texture;
			}

			//Detach the emissive map from the material
			inline void EmissiveMap(std::nullptr_t) noexcept
			{
				if (emissive_map_.index() > 0 && emissive_color_ == color::White)
					emissive_color_ = color::Black;

				emissive_map_ = std::monostate{};
			}


			//Sets the lower left and upper right texture coordinates for this material to the given coordinates
			inline void TexCoords(const Vector2 &lower_left, const Vector2 &upper_right) noexcept
			{
				lower_left_tex_coord_ = lower_left;
				upper_right_tex_coord_ = upper_right;
			}


			//Sets the emissive light radius of the material to the given value
			inline void EmissiveLightRadius(std::optional<real> radius) noexcept
			{
				emissive_light_radius_ = radius;
			}

			//Sets if this material has lighting enabled or not
			inline void LightingEnabled(bool enabled) noexcept
			{
				lighting_enabled_ = enabled;
			}

			//Sets if this material receives shadows or not
			inline void ReceiveShadows(bool enabled) noexcept
			{
				receive_shadows_ = enabled;
			}


			/*
				Observers
			*/

			//Returns the ambient color of the material
			[[nodiscard]] inline auto& AmbientColor() const noexcept
			{
				return ambient_color_;
			}
			
			//Returns the diffuse color of the material
			[[nodiscard]] inline auto& DiffuseColor() const noexcept
			{
				return diffuse_color_;
			}
			
			//Returns the specular color of the material
			[[nodiscard]] inline auto& SpecularColor() const noexcept
			{
				return specular_color_;
			}

			//Returns the emissive (self-illumination) color of the material
			[[nodiscard]] inline auto& EmissiveColor() const noexcept
			{
				return emissive_color_;
			}

			//Returns the shininess of this material
			[[nodiscard]] inline auto Shininess() const noexcept
			{
				return shininess_;
			}


			//Returns the attached diffuse map as a pair of either animation or texture
			//Returns nullptr on both components if no diffuse map is in use
			[[nodiscard]] inline auto DiffuseMap() const noexcept
			{
				return material::detail::get_texture_maps(diffuse_map_);
			}

			//Returns the attached normal map as a pair of either animation or texture
			//Returns nullptr on both components if no normal map is in use
			[[nodiscard]] inline auto NormalMap() const noexcept
			{
				return material::detail::get_texture_maps(normal_map_);
			}

			//Returns the attached specular map as a pair of either animation or texture
			//Returns nullptr on both components if no specular map is in use
			[[nodiscard]] inline auto SpecularMap() const noexcept
			{
				return material::detail::get_texture_maps(specular_map_);
			}

			//Returns the attached emissive map as a pair of either animation or texture
			//Returns nullptr on both components if no emissive map is in use
			[[nodiscard]] inline auto EmissiveMap() const noexcept
			{
				return material::detail::get_texture_maps(emissive_map_);
			}


			//Returns the attached diffuse map texture at the given time
			//Returns nullptr if no diffuse map is in use
			[[nodiscard]] NonOwningPtr<Texture> DiffuseMap(duration time) const noexcept;

			//Returns the attached normal map texture at the given time
			//Returns nullptr if no normal map is in use
			[[nodiscard]] NonOwningPtr<Texture> NormalMap(duration time) const noexcept;

			//Returns the attached specular map texture at the given time
			//Returns nullptr if no specular map is in use
			[[nodiscard]] NonOwningPtr<Texture> SpecularMap(duration time) const noexcept;

			//Returns the attached emissive map texture at the given time
			//Returns nullptr if no emissive map is in use
			[[nodiscard]] NonOwningPtr<Texture> EmissiveMap(duration time) const noexcept;


			//Returns the lower left and upper right texture coordinates for this material
			[[nodiscard]] inline auto TexCoords() const noexcept
			{
				return std::pair{lower_left_tex_coord_, upper_right_tex_coord_};
			}


			//Returns the emissive light radius of the material
			//Returns nullopt if light radius is automatic
			[[nodiscard]] inline auto EmissiveLightRadius() const noexcept
			{
				return emissive_light_radius_;
			}

			//Returns true if this material has lighting enabled
			[[nodiscard]] inline auto LightingEnabled() const noexcept
			{
				return lighting_enabled_;
			}

			//Returns true if this material receives shadows
			[[nodiscard]] inline auto ReceiveShadows() const noexcept
			{
				return receive_shadows_;
			}


			/*
				Texture coordinates
			*/

			//Crop material texture maps by the given area, where values are in range [0.0, 1.0]
			//This operation will discard any repeating previously applied
			void Crop(const std::optional<Aabb> &area) noexcept;

			//Repeat material texture maps by the given amount, where values are in range [0.0, oo)
			//This operation will discard any cropping previously applied
			void Repeat(const std::optional<Vector2> &amount) noexcept;


			//Flip material texture maps horizontally (mirror)
			void FlipHorizontal() noexcept;

			//Flip material texture maps vertically (up-down)
			void FlipVertical() noexcept;


			//Returns true if this materials texture maps are cropped
			[[nodiscard]] bool IsCropped() const noexcept;

			//Returns true if this materials texture maps are repeated
			[[nodiscard]] bool IsRepeated() const noexcept;

			//Returns a pair of true/false to indicate which texture maps axis for this material is repeatable
			//For animations, repeatability is only retrieved from the first frame
			[[nodiscard]] std::pair<bool, bool> IsRepeatable() const noexcept;


			//Returns true if this materials texture maps are flipped horizontally
			[[nodiscard]] bool IsFlippedHorizontally() const noexcept;

			//Returns true if this materials texture maps are flipped vertically
			[[nodiscard]] bool IsFlippedVertically() const noexcept;


			//Returns the lower left and upper right texture coordinates for this material
			//Combines the relative tex coords for the material, to that of the actual texture
			//For animations, tex coords are only retrieved from the first frame
			[[nodiscard]] std::pair<Vector2, Vector2> WorldTexCoords() const noexcept;
	};
} //ion::graphics::materials

#endif