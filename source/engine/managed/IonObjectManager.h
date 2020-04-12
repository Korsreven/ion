/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	managed
File:	IonObjectManager.h
-------------------------------------------
*/

#ifndef ION_OBJECT_MANAGER_H
#define ION_OBJECT_MANAGER_H

#include <algorithm>
#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "IonManagedObject.h"
#include "adaptors/ranges/IonDereferenceIterable.h"
#include "events/IonListenable.h"
#include "events/listeners/IonManagedObjectListener.h"

namespace ion::managed
{
	namespace object_manager::detail
	{
		template <typename T>
		using container_type = std::vector<std::unique_ptr<T>>; //Owning
	} //object_manager::detail


	template <typename ObjectT, typename OwnerT, typename ListenerT = events::listeners::ManagedObjectListener<ObjectT, OwnerT>>
	class ObjectManager : public events::Listenable<ListenerT>
	{
		static_assert(std::is_base_of_v<ManagedObject<OwnerT>, ObjectT>);

		public:
			
			using object_type = ObjectT;
			using owner_type = OwnerT;

		private:

			object_manager::detail::container_type<ObjectT> objects_;


			/*
				Notifying
			*/

			void NotifyCreated(ObjectT &object) noexcept
			{
				Created(object); //Notify derived first
				this->NotifyAll(this->Listeners(), &ListenerT::ObjectCreated, object);
			}

			auto NotifyRemovable(ObjectT &object) noexcept
			{
				auto removable = Removable(object); //Notify derived first
				
				for (auto &listener : this->Listeners())
					removable &= this->Notify(&ListenerT::ObjectRemovable, listener, object).value_or(true);
						//If one listener disagrees (returns false), the object can not be removed

				return removable;
			}

			void NotifyRemoved(ObjectT &object) noexcept
			{
				Removed(object); //Notify derived first
				this->NotifyAll(this->Listeners(), &ListenerT::ObjectRemoved, object);
			}

			void NotifyMoved(ObjectT &object) noexcept
			{
				Moved(object); //Notify derived first
				this->NotifyAll(this->Listeners(), &ListenerT::ObjectMoved, static_cast<OwnerT&>(*this));
			}


			void NotifyRemovableAll() noexcept
			{
				for (auto &object : objects_)
					NotifyRemovable(*object);
			}

			void NotifyRemovedAll(decltype(objects_) &objects) noexcept
			{
				for (auto &object : objects)
					NotifyRemoved(*object);
			}

			void NotifyMovedAll(decltype(objects_) &objects) noexcept
			{
				for (auto &object : objects)
				{
					object->Owner(static_cast<OwnerT&>(*this));
					NotifyMoved(*object);
				}
			}

		protected:

			/*
				Events
			*/

			//See ManagedObjectListener::ObjectCreated for more details
			virtual void Created([[maybe_unused]] ObjectT &object) noexcept
			{
				//Optional to override
			}

			//See ManagedObjectListener::ObjectRemovable for more details
			virtual bool Removable([[maybe_unused]] ObjectT &object) noexcept
			{
				//Optional to override
				return true;
			}

			//See ManagedObjectListener::ObjectRemoved for more details
			virtual void Removed([[maybe_unused]] ObjectT &object) noexcept
			{
				//Optional to override
			}

			//See ManagedObjectListener::ObjectMoved for more details
			virtual void Moved([[maybe_unused]] ObjectT &object) noexcept
			{
				//Optional to override
			}


			//Called right before starting to create or adopt objects
			virtual void AdditionStarted() noexcept
			{
				//Optional to override
			}

			//Called right after objects has been created or adopted
			virtual void AdditionEnded() noexcept
			{
				//Optional to override
			}


			//Called right before starting to remove or orphan objects
			virtual void RemovalStarted() noexcept
			{
				//Optional to override
			}

			//Called right after objects has been removed or orphaned
			virtual void RemovalEnded() noexcept
			{
				//Optional to override
			}


			/*
				Creating
			*/

			//Create a object with the given arguments
			template <typename... Args>
			auto& Create(Args &&...args)
			{
				AdditionStarted();

				auto &object = objects_.emplace_back(
					std::make_unique<ObjectT>(std::forward<Args>(args)...));
				object->Owner(static_cast<OwnerT&>(*this));
				NotifyCreated(*object);

				AdditionEnded();
				return *object;
			}


			/*
				Removing
			*/

			auto Extract(ObjectT &object) noexcept
			{
				auto iter =
					std::find_if(std::begin(objects_), std::end(objects_),
						[&](auto &x) noexcept
						{
							return x.get() == &object;
						});

				//object found
				if (iter != std::end(objects_) &&
					NotifyRemovable(object))
				{
					RemovalStarted();

					typename decltype(objects_)::value_type object_ptr = std::move(*iter); //Extend lifetime
					objects_.erase(iter);
					NotifyRemoved(object); //Still valid

					RemovalEnded();
					return std::move(object_ptr);
				}
				else
					return typename decltype(objects_)::value_type{};
			}

			auto ExtractAll() noexcept
			{
				//[objects to keep, objects to remove]
				auto iter =
					std::stable_partition(std::begin(objects_), std::end(objects_),
						[&](auto &object) noexcept
						{
							return !NotifyRemovable(*object);
						});

				//Something to remove
				if (iter != std::end(objects_))
				{
					RemovalStarted();

					decltype(objects_) objects;
					std::move(iter, std::end(objects_), std::back_inserter(objects)); //Extend lifetime
					objects_.erase(iter, std::end(objects_)); //Erase the moved range
					objects_.shrink_to_fit();	
					NotifyRemovedAll(objects); //Still valid

					RemovalEnded();
					return std::move(objects);
				}
				else
					return decltype(objects_){};
			}

			void Tidy() noexcept
			{
				if (!std::empty(objects_))
				{
					NotifyRemovableAll();
					RemovalStarted();

					decltype(objects_) objects = std::move(objects_);
					objects_.shrink_to_fit();
					NotifyRemovedAll(objects);

					RemovalEnded();
				}
			}

		public:

			//Default constructor
			ObjectManager() = default;

			//Deleted copy constructor
			ObjectManager(const ObjectManager&) = delete;

			//Default move constructor
			ObjectManager(ObjectManager &&rhs) noexcept :
				events::Listenable<ListenerT>{std::move(rhs)},
				objects_{std::move(rhs.objects_)}
			{
				NotifyMovedAll(objects_);
			}

			//Destructor
			~ObjectManager() noexcept
			{
				Tidy();
			}


			/*
				Operators
			*/

			//Deleted copy assignment
			ObjectManager& operator=(const ObjectManager&) = delete;

			//Move assignment
			inline auto& operator=(ObjectManager &&rhs) noexcept
			{
				events::Listenable<ListenerT>::operator=(std::move(rhs));

				if (this != &rhs)
				{
					Tidy();
					objects_ = std::move(rhs.objects_);
					NotifyMovedAll(objects_);
				}

				return *this;
			}


			/*
				Ranges
			*/

			//Returns a mutable range of all objects in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Objects() noexcept
			{
				return adaptors::ranges::DereferenceIterable<object_manager::detail::container_type<ObjectT>&>{objects_};
			}

			//Returns an immutable range of all objects in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline const auto Objects() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const object_manager::detail::container_type<ObjectT>&>{objects_};
			}


			/*
				Take / release ownership
			*/

			//Adopt (take ownership of) the given object
			auto& Adopt(typename decltype(objects_)::value_type object_ptr) noexcept
			{
				assert(object_ptr);
				AdditionStarted();

				auto &object = objects_.emplace_back(std::move(object_ptr));
				object->Owner(static_cast<OwnerT&>(*this));
				NotifyCreated(*object);

				AdditionEnded();
				return *object;
			}

			//Adopt (take ownership of) all the given objects
			void Adopt(typename decltype(objects_) objects) noexcept
			{
				objects.erase(
					std::remove_if(std::begin(objects), std::end(objects),
						[](const auto &object) noexcept
						{
							return !object;
						}),
					std::end(objects));

				if (!std::empty(objects))
				{
					AdditionStarted();

					std::move(std::begin(objects), std::end(objects), std::back_inserter(objects_));
					std::for_each(std::end(objects_) - std::size(objects), std::end(objects_),
						[](auto &object) mutable noexcept
						{
							object->Owner(static_cast<OwnerT&>(*this));
							NotifyCreated(*object);
						});

					AdditionEnded();
				}
			}


			//Orphan (release ownership of) the given object
			//Returns a pointer to the object released
			[[nodiscard]] auto Orphan(ObjectT &object) noexcept
			{
				auto object_ptr = Extract(object);

				//Release ownership
				if (object_ptr)
					object_ptr->Release();

				return object_ptr;
			}

			//Orphan (release ownership of) all objects in this manager
			//Returns a pointer to the object released
			[[nodiscard]] auto OrphanAll() noexcept
			{
				auto objects = ExtractAll();

				//Release ownership
				for (auto &object : objects)
					object->Release();

				return objects;
			}


			/*
				Removing
			*/

			//Clear all removable objects from this manager
			void Clear() noexcept
			{
				ExtractAll();
			}

			//Remove a removable object from this manager
			auto Remove(ObjectT &object) noexcept
			{
				auto ptr = Extract(object);
				return !!ptr;
			}
	};
} //ion::managed

#endif