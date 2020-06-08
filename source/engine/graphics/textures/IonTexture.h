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

#include "resources/files/IonFileResource.h"

namespace ion::graphics::textures
{
	class TextureManger; //Forward declaration

	class Texture : public resources::files::FileResource<TextureManger>
	{
		private:

			std::optional<int> handle_;

		public:

			using resources::files::FileResource<TextureManger>::FileResource;


			/*
				Modifiers
			*/

			//Sets the handle for the texture to the given value
			inline void Handle(std::optional<int> handle)
			{
				handle_ = handle;
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
	};
} //ion::graphics::textures

#endif