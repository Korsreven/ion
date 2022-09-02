/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	resources
File:	IonResource.h
-------------------------------------------
*/

#ifndef ION_RESOURCE_H
#define ION_RESOURCE_H

#include "managed/IonManagedObject.h"

namespace ion::resources
{
	namespace resource
	{
		enum class LoadingState
		{
			PreparePending,	//Resource is waiting for preparing
			Preparing,		//Load into memory and prepare data (background thread)
			Prepared,		//Data available in memory

			LoadPending,	//Resource is waiting for loading
			Loading,		//Load from memory to vram (main thread)
			Loaded,			//Resource is available for use

			UnloadPending,	//Resource is waiting for unloading
			Unloading,		//Unloading resource from vram
			Unloaded,		//Resource is unloaded

			Failed			//Resource failed to prepare/load
		};

		enum class LoadingAction
		{
			Prepare,	//Prepare resource
			Load,		//Load resource
			Unload		//Unload resource
		};

		namespace detail
		{
		} //detail
	} //resource


	//A class representing a general resource that can be prepared, loaded and unloaded
	template <typename T>
	class Resource : public managed::ManagedObject<T>
	{
		private:

			resource::LoadingState loading_state_ = resource::LoadingState::Unloaded;
			resource::LoadingAction loading_action_ = resource::LoadingAction::Unload;

		public:

			using managed::ManagedObject<T>::ManagedObject;


			/*
				Modifiers
			*/

			//Sets the resource to the given loading state
			inline void LoadingState(resource::LoadingState loading_state) noexcept
			{
				loading_state_ = loading_state;
			}


			/*
				Observers
			*/

			//Returns the loading state of the resource
			[[nodiscard]] inline auto LoadingState() const noexcept
			{
				return loading_state_;
			}

			//Returns the loading action of the resource
			[[nodiscard]] inline auto LoadingAction() const noexcept
			{
				return loading_action_;
			}


			/*
				Loading
			*/

			//Signaling that this resource needs to be prepared
			inline auto Prepare() noexcept
			{
				switch (loading_state_)
				{
					case resource::LoadingState::Unloaded:
					case resource::LoadingState::Failed:
					loading_state_ = resource::LoadingState::PreparePending;
					loading_action_ = resource::LoadingAction::Prepare;
					return true;

					default:
					return false;
				}			
			}

			//Signaling that this resource needs to be loaded
			inline auto Load() noexcept
			{
				switch (loading_state_)
				{
					case resource::LoadingState::Unloaded:
					case resource::LoadingState::Failed:
					loading_state_ = resource::LoadingState::PreparePending;
					loading_action_ = resource::LoadingAction::Load;
					return true;

					case resource::LoadingState::Prepared:
					loading_state_ = resource::LoadingState::LoadPending;
					loading_action_ = resource::LoadingAction::Load;
					return true;

					default:
					return false;
				}
			}

			//Signaling that this resource needs to be unloaded
			inline auto Unload() noexcept
			{
				switch (loading_state_)
				{
					case resource::LoadingState::Prepared:
					case resource::LoadingState::Loaded:
					case resource::LoadingState::Failed:
					loading_state_ = resource::LoadingState::UnloadPending;
					loading_action_ = resource::LoadingAction::Unload;
					return true;

					default:
					return false;
				}
			}


			//Signaling that this resource needs to be reloaded
			inline auto Reload() noexcept
			{
				switch (loading_state_)
				{			
					case resource::LoadingState::Prepared:
					case resource::LoadingState::Loaded:
					case resource::LoadingState::Failed:
					loading_state_ = resource::LoadingState::UnloadPending;
					loading_action_ = resource::LoadingAction::Load; 
					return true;

					default:
					return false;
				}
			}

			//Signaling that this resource needs to be repaired if failed
			inline auto Repair() noexcept
			{
				switch (loading_state_)
				{
					case resource::LoadingState::Failed:
					return Reload();

					default:
					return false;
				}
			}


			//Returns true if this resource is preparing
			[[nodiscard]] inline auto IsPreparing() const noexcept
			{
				switch (loading_state_)
				{
					case resource::LoadingState::PreparePending:
					case resource::LoadingState::Preparing:
					return true;

					default:
					return false;
				}
			}

			//Returns true if this resource is prepared
			[[nodiscard]] inline auto IsPrepared() const noexcept
			{
				return loading_state_ == resource::LoadingState::Prepared;
			}

			//Returns true if this resource is loading
			[[nodiscard]] inline auto IsLoading() const noexcept
			{
				switch (loading_state_)
				{
					case resource::LoadingState::LoadPending:
					case resource::LoadingState::Loading:
					return true;

					default:
					return false;
				}
			}

			//Returns true if this resource is loaded
			[[nodiscard]] inline auto IsLoaded() const noexcept
			{
				return loading_state_ == resource::LoadingState::Loaded;
			}

			//Returns true if this resource is unloading
			[[nodiscard]] inline auto IsUnloading() const noexcept
			{
				switch (loading_state_)
				{
					case resource::LoadingState::UnloadPending:
					case resource::LoadingState::Unloading:
					return true;

					default:
					return false;
				}
			}

			//Returns true if this resource is unloaded
			[[nodiscard]] inline auto IsUnloaded() const noexcept
			{
				return loading_state_ == resource::LoadingState::Unloaded;
			}

			//Returns true if this resource has failed
			[[nodiscard]] inline auto HasFailed() const noexcept
			{
				return loading_state_ == resource::LoadingState::Failed;
			}


			//Returns true if this resource is busy preparing, loading or unloading
			[[nodiscard]] inline auto IsBusy() const noexcept
			{
				return IsPreparing() || IsLoading() || IsUnloading();
			}

			
	};
} //ion::resources

#endif