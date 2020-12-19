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

#include "graphics/textures/IonAnimationManager.h"
#include "graphics/textures/IonTextureManager.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonVector2.h"
#include "managed/IonManagedObject.h"
#include "managed/IonObservedObject.h"
#include "types/IonTypes.h"

namespace ion::graphics::materials
{
	struct MaterialManager; //Forward declaration

	using namespace types::type_literals;
	using namespace utilities;

	using textures::Animation;
	using textures::Texture;

	namespace material::detail
	{
		using map_type = std::variant<
			std::monostate,
			managed::ObservedObject<Animation>,
			managed::ObservedObject<Texture>>;


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

		Vector2 get_normalized_tex_coords(const Vector2 &tex_coords, const Vector2 &min, const Vector2 &max) noexcept;
		std::pair<Vector2, Vector2> get_normalized_tex_coords(const Vector2 &lower_left, const Vector2 &upper_right,
			const Vector2 &min, const Vector2 &max) noexcept;


		/*
			Texture map
		*/

		std::pair<const Animation*, const Texture*> get_texture_maps(const map_type &map) noexcept;
		const Texture* get_texture_map(const map_type &map) noexcept;
		const Texture* get_first_texture_map(const map_type &diffuse_map, const map_type &specular_map, const map_type &normal_map) noexcept;

		std::pair<bool, bool> is_texture_map_repeatable(const Texture &texture,
			const Vector2 &lower_left, const Vector2 &upper_right) noexcept;
	} //material::detail


	class Material final : public managed::ManagedObject<MaterialManager>
	{
		private:

			Color ambient_color_;
			Color diffuse_color_;
			Color specular_color_;
			real shininess_ = 0.0_r;

			material::detail::map_type diffuse_map_;
			material::detail::map_type specular_map_;
			material::detail::map_type normal_map_;

			Vector2 lower_left_tex_coords_ = vector2::Zero;
			Vector2 upper_right_tex_coords_ = vector2::UnitScale;
			std::optional<Color> emissive_color_;
			bool receive_shadows_ = true;

		public:
			
			//Constructs a new material with the given name, ambient color, diffuse color, specular color and shininess
			Material(std::string name,
				const Color &ambient, const Color &diffuse, const Color &specular, real shininess);

			//Constructs a new material with the given name, ambient color, diffuse color, specular color, shininess,
			//and emissive color and receive shadows
			Material(std::string name,
				const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
				const std::optional<Color> &emissive, bool receive_shadows = true);


			//Constructs a new material with the given name, ambient color, diffuse color, specular color, shininess,
			//and diffuse map, specular map and normal map as animations
			Material(std::string name,
				const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
				Animation *diffuse_map, Animation *specular_map, Animation *normal_map);

			//Constructs a new material with the given name, ambient color, diffuse color, specular color, shininess,
			//and diffuse map, specular map and normal map as animations,
			//and emissive color and receive shadows
			Material(std::string name,
				const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
				Animation *diffuse_map, Animation *specular_map, Animation *normal_map,
				const std::optional<Color> &emissive, bool receive_shadows = true);


			//Constructs a new material with the given name, ambient color, diffuse color, specular color, shininess,
			//and diffuse map, specular map and normal map as textures
			Material(std::string name,
				const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
				Texture *diffuse_map, Texture *specular_map, Texture *normal_map);

			//Constructs a new material with the given name, ambient color, diffuse color, specular color, shininess,
			//and diffuse map, specular map and normal map as textures,
			//and emissive color and receive shadows
			Material(std::string name,
				const Color &ambient, const Color &diffuse, const Color &specular, real shininess,
				Texture *diffuse_map, Texture *specular_map, Texture *normal_map,
				const std::optional<Color> &emissive, bool receive_shadows = true);


			/*
				Modifiers
			*/

			//Sets the ambient color of this material to the given color
			inline void AmbientColor(const Color &ambient) noexcept
			{
				ambient_color_ = ambient;
			}
			
			//Sets the diffuse color of this material to the given color
			inline void DiffuseColor(const Color &diffuse) noexcept
			{
				diffuse_color_ = diffuse;
			}
			
			//Sets the specular color of this material to the given color
			inline void SpecularColor(const Color &specular) noexcept
			{
				specular_color_ = specular;
			}

			//Sets the shininess of the material to the given value
			inline void Shininess(real shininess) noexcept
			{
				shininess_ = shininess;
			}


			//Attach the given animation as a diffuse map for the material
			void DiffuseMap(Animation &animation);

			//Attach the given texture as a diffuse map for the material
			void DiffuseMap(Texture &texture);

			//Detach the diffuse map from the material
			void DiffuseMap(std::nullptr_t) noexcept;


			//Attach the given animation as a specular map for the material
			void SpecularMap(Animation &animation);

			//Attach the given texture as a specular map for the material
			void SpecularMap(Texture &texture);

			//Detach the specular map from the material
			void SpecularMap(std::nullptr_t) noexcept;


			//Attach the given animation as a normal map for the material
			void NormalMap(Animation &animation);

			//Attach the given texture as a normal map for the material
			void NormalMap(Texture &texture);

			//Detach the normal map from the material
			void NormalMap(std::nullptr_t) noexcept;


			//Sets the lower left and upper right texture coordinates for this material to the given coordinates
			inline void TexCoords(const Vector2 &lower_left, const Vector2 &upper_right) noexcept
			{
				lower_left_tex_coords_ = lower_left;
				upper_right_tex_coords_ = upper_right;
			}

			//Sets the emissive (self-illumination) color of this material to the given color
			inline void EmissiveColor(const std::optional<Color> &emissive) noexcept
			{
				emissive_color_ = emissive;
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

			//Returns the shininess of this material
			[[nodiscard]] inline auto Shininess() const noexcept
			{
				return shininess_;
			}


			//Returns the attached diffuse map as a pair of either animation or texture
			//Returns nullptr on both components if no diffuse map is in use
			[[nodiscard]] std::pair<const Animation*, const Texture*> DiffuseMap() const noexcept;

			//Returns the attached specular map as a pair of either animation or texture
			//Returns nullptr on both components if no specular map is in use
			[[nodiscard]] std::pair<const Animation*, const Texture*> SpecularMap() const noexcept;

			//Returns the attached normal map as a pair of either animation or texture
			//Returns nullptr on both components if no normal map is in use
			[[nodiscard]] std::pair<const Animation*, const Texture*> NormalMap() const noexcept;


			//Returns the lower left and upper right texture coordinates for this material
			[[nodiscard]] inline auto TexCoords() const noexcept
			{
				return std::pair{lower_left_tex_coords_, upper_right_tex_coords_};
			}

			//Returns the emissive (self-illumination) color of the material
			//Returns nullopt if this material is not emissive
			[[nodiscard]] inline auto& EmissiveColor() const noexcept
			{
				return emissive_color_;
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