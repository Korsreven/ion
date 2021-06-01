/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	sounds
File:	IonSound.h
-------------------------------------------
*/

#ifndef ION_SOUND_H
#define ION_SOUND_H

#include <string>

#include "resources/IonFileResource.h"

namespace ion::sounds
{
	class SoundManager; //Forward declaration
	
	namespace sound
	{
		namespace detail
		{
		} //detail
	} //sound


	class Sound final : public resources::FileResource<SoundManager>
	{
		private:



		public:

			using resources::FileResource<SoundManager>::FileResource;

			//Construct a new sound with the given name and asset name
			Sound(std::string name, std::string asset_name);


			/*
				Modifiers
			*/




			/*
				Observers
			*/
	};
} //ion::sounds

#endif