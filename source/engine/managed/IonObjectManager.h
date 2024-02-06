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
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "IonManagedObject.h"
#include "adaptors/ranges/IonDereferenceIterable.h"
#include "events/IonListenable.h"
#include "events/listeners/IonManagedObjectListener.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"

namespace ion::managed
{
	namespace object_manager::detail
	{
		template <typename T>
		using container_type = std::vector<OwningPtr<T>>;


		template <typename T>
		inline auto get_object_by_name(std::string_view name, const object_manager::detail::container_type<T> &objects) noexcept
		{
			auto iter =
				std::find_if(std::begin(objects), std::end(objects),
					[&](auto &object) noexcept
					{
						if (auto object_name = object->Name(); object_name)
							return *object_name == name;
						else
							return false;
					});

			return iter != std::end(objects) ? NonOwningPtr<T>{*iter} : NonOwningPtr<T>{};
		}
	} //object_manager::detail


	///@brief A non-templated base class for all object managers
	///@details This class is needed to store different types of object managers in a single object register
	struct ObjectManagerBase
	{
		//Default virtual destructor
		virtual ~ObjectManagerBase() = default;
	};


	///@brief A class representing an owning manager that can create and store multiple objects of a single type
	///@details The created objects can be of any type, but must be derived from managed object.
	///This class must be derived from to be able to create objects
	template <typename ObjectT, typename OwnerT, typename ListenerT = events::listeners::ManagedObjectListener<ObjectT, OwnerT>>
	class ObjectManager : virtual public ObjectManagerBase, public events::Listenable<ListenerT>
	{
		static_assert(std::is_base_of_v<ManagedObject<OwnerT>, ObjectT>);

		public:
			
			using object_type = ObjectT;
			using owner_type = OwnerT;

		private:

			object_manager::detail::container_type<ObjectT> objects_;


			/**
				@name Notifying
				@{
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

			///@}

			/**
				@name Creating
				@{
			*/

			template <typename T, typename... Args>
			auto Emplace(Args &&...args)
			{
				AdditionStarted();

				auto &ptr = objects_.emplace_back(
					make_owning<T>(std::forward<Args>(args)...));
				ptr->Owner(static_cast<OwnerT&>(*this));
				NotifyCreated(*ptr);

				AdditionEnded();
				return NonOwningPtr<ObjectT>{ptr};
			}

			///@}

		protected:

			/**
				@name Events
				@{
			*/

			///@brief See ManagedObjectListener::ObjectCreated for more details
			virtual void Created([[maybe_unused]] ObjectT &object) noexcept
			{
				//Optional to override
			}

			///@brief See ManagedObjectListener::ObjectRemovable for more details
			virtual bool Removable([[maybe_unused]] ObjectT &object) noexcept
			{
				//Optional to override
				return true;
			}

			///@brief See ManagedObjectListener::ObjectRemoved for more details
			virtual void Removed([[maybe_unused]] ObjectT &object) noexcept
			{
				//Optional to override
			}

			///@brief See ManagedObjectListener::ObjectMoved for more details
			virtual void Moved([[maybe_unused]] ObjectT &object) noexcept
			{
				//Optional to override
			}


			///@brief Called right before starting to create or adopt objects
			virtual void AdditionStarted() noexcept
			{
				//Optional to override
			}

			///@brief Called right after objects has been created or adopted
			virtual void AdditionEnded() noexcept
			{
				//Optional to override
			}


			///@brief Called right before starting to remove or orphan objects
			virtual void RemovalStarted() noexcept
			{
				//Optional to override
			}

			///@brief Called right after objects has been removed or orphaned
			virtual void RemovalEnded() noexcept
			{
				//Optional to override
			}

			///@}

			/**
				@name Creating
				@{
			*/

			///@brief Creates an object with the given arguments
			template <typename T = ObjectT, typename... Args,
				typename = std::enable_if_t<std::is_base_of_v<ObjectT, std::remove_cvref_t<T>>>>
			auto Create(Args &&...args)
			{
				return Emplace<std::remove_cvref_t<T>>(std::forward<Args>(args)...);
			}

			///@brief Creates an object with the given name and arguments
			template <typename T = ObjectT, typename... Args,
				typename = std::enable_if_t<std::is_base_of_v<ObjectT, std::remove_cvref_t<T>>>>
			auto Create(std::string name, Args &&...args)
			{
				//Check if an object with that name already exists
				if (auto ptr = object_manager::detail::get_object_by_name(name, objects_); ptr)
					return ptr;
				else
					return Emplace<std::remove_cvref_t<T>>(std::move(name), std::forward<Args>(args)...);
			}

			///@brief Creates an object with the given name and arguments
			template <typename T = ObjectT, typename... Args,
				typename = std::enable_if_t<std::is_base_of_v<ObjectT, std::remove_cvref_t<T>>>>
			auto Create(std::optional<std::string> name, Args &&...args)
			{
				//Object has name
				if (name)
				{
					//Check if an object with that name already exists
					if (auto ptr = object_manager::detail::get_object_by_name(*name, objects_); ptr)
						return ptr;
				}

				return Emplace<std::remove_cvref_t<T>>(std::move(name), std::forward<Args>(args)...);
			}

			///@brief Creates an object by copying/moving the given object
			template <typename T,
				typename = std::enable_if_t<std::is_base_of_v<ObjectT, std::remove_cvref_t<T>>>>
			auto Create(T &&object)
			{
				//Object has name
				if (auto object_name = object.Name(); object_name)
				{
					//Check if an object with that name already exists
					if (auto ptr = object_manager::detail::get_object_by_name(*object_name, objects_); ptr)
						return ptr;
				}
				
				return Emplace<std::remove_cvref_t<T>>(std::forward<T>(object));
			}

			///@}

			/**
				@name Removing
				@{
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
					return object_ptr;
				}
				else
					return typename decltype(objects_)::value_type{};
			}

			template <typename UnaryPredicate>
			auto ExtractIf(UnaryPredicate p) noexcept
			{
				static_assert(std::is_invocable_r_v<bool, UnaryPredicate, ObjectT&>);

				//[objects to keep, objects to remove]
				auto iter =
					std::stable_partition(std::begin(objects_), std::end(objects_),
						[&](auto &object) noexcept
						{
							return !NotifyRemovable(*object) || !p(*object);
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
					return objects;
				}
				else
					return decltype(objects_){};
			}

			auto ExtractAll() noexcept
			{
				return ExtractIf([](ObjectT&) { return true; });
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

			///@}

		public:

			///@brief Default constructor
			ObjectManager() = default;

			///@brief Deleted copy constructor
			ObjectManager(const ObjectManager&) = delete;

			///@brief Default move constructor
			ObjectManager(ObjectManager &&rhs) noexcept :
				events::Listenable<ListenerT>{std::move(rhs)},
				objects_{std::move(rhs.objects_)}
			{
				NotifyMovedAll(objects_);
			}

			///@brief Virtual destructor
			virtual ~ObjectManager() noexcept
			{
				Tidy();
			}


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			ObjectManager& operator=(const ObjectManager&) = delete;

			///@brief Move assignment
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

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all objects in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Objects() noexcept
			{
				return adaptors::ranges::DereferenceIterable<object_manager::detail::container_type<ObjectT>&>{objects_};
			}

			///@brief Returns an immutable range of all objects in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Objects() const noexcept
			{
				return adaptors::ranges::DereferenceIterable<const object_manager::detail::container_type<ObjectT>&>{objects_};
			}

			///@}

			/**
				@name Take/release ownership
				@{
			*/

			///@brief Adopts (take ownership of) the given object and returns a pointer to the adopted object
			///@details Returns nullptr if the object could not be adopted and object_ptr will remain untouched
			auto Adopt(typename decltype(objects_)::value_type &object_ptr) noexcept
			{
				assert(object_ptr);

				if (auto object_name = object_ptr->Name(); object_name)
				{
					//Check if an object with that name already exists
					if (auto ptr = object_manager::detail::get_object_by_name(*object_name, objects_); ptr)
						return NonOwningPtr<ObjectT>{}; //Object exists, could not adopt
				}

				AdditionStarted();

				auto &ptr = objects_.emplace_back(std::move(object_ptr));
				ptr->Owner(static_cast<OwnerT&>(*this));
				NotifyCreated(*ptr);

				AdditionEnded();
				return NonOwningPtr<ObjectT>{ptr};
			}

			///@brief Adopts (take ownership of) the given object and returns a pointer to the adopted object
			///@details Returns nullptr if the object could not be adopted and object_ptr will be released
			auto Adopt(typename decltype(objects_)::value_type &&object_ptr) noexcept
			{	
				return Adopt(object_ptr);
			}

			///@brief Adopts (take ownership of) all the given objects
			///@details If one or more objects could not be adopted, they will remain untouched in the given container
			void AdoptAll(typename decltype(objects_) &objects) noexcept
			{
				decltype(objects_) adoptable_objects;

				for (auto iter = std::begin(objects); iter != std::end(objects) && *iter;)
				{
					if (auto object_name = (*iter)->Name(); object_name)
					{
						if (auto ptr = object_manager::detail::get_object_by_name(*object_name, objects_); ptr)
						{
							++iter;
							continue; //Object exists, could not adopt
						}
					}

					adoptable_objects.push_back(std::move(*iter));
					iter = objects.erase(iter);
				}

				if (!std::empty(adoptable_objects))
				{
					AdditionStarted();

					std::move(std::begin(adoptable_objects), std::end(adoptable_objects), std::back_inserter(objects_));
					std::for_each(std::end(objects_) - std::size(adoptable_objects), std::end(objects_),
						[&](auto &ptr) mutable noexcept
						{
							ptr->Owner(static_cast<OwnerT&>(*this));
							NotifyCreated(*ptr);
						});

					AdditionEnded();
				}
			}

			///@brief Adopts (take ownership of) all the given objects
			///@details If one or more objects could not be adopted, they will be released
			void AdoptAll(typename decltype(objects_) &&objects) noexcept
			{
				AdoptAll(objects);
			}


			///@brief Orphans (release ownership of) the given object
			///@details Returns a pointer to the object released
			[[nodiscard]] auto Orphan(ObjectT &object) noexcept
			{
				auto object_ptr = Extract(object);

				//Release ownership
				if (object_ptr)
					object_ptr->Owner(nullptr);

				return object_ptr;
			}

			///@brief Orphans (release ownership of) all objects in this manager
			///@details Returns a pointer to the object released
			[[nodiscard]] auto OrphanAll() noexcept
			{
				auto objects = ExtractAll();

				//Release ownership
				for (auto &object : objects)
					object->Owner(nullptr);

				return objects;
			}

			///@}

			/**
				@name Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable object with the given name
			///@details Returns nullptr if object could not be found
			[[nodiscard]] auto Get(std::string_view name) noexcept
			{
				return object_manager::detail::get_object_by_name(name, objects_);
			}

			///@brief Gets a pointer to an immutable object with the given name
			///@details Returns nullptr if object could not be found
			[[nodiscard]] auto Get(std::string_view name) const noexcept
				-> NonOwningPtr<const ObjectT>
			{
				return object_manager::detail::get_object_by_name(name, objects_);
			}

			///@}

			/**
				@name Removing
				@{
			*/

			///@brief Clears all removable objects from this manager
			void Clear() noexcept
			{
				ExtractAll();
			}

			///@brief Removes a removable object from this manager
			auto Remove(ObjectT &object) noexcept
			{
				auto ptr = Extract(object);
				return !!ptr;
			}

			///@brief Removes a removable object with the given name from this manager
			auto Remove(std::string_view name) noexcept
			{
				if (auto ptr = Get(name); ptr)
					return Remove(*ptr);
				else
					return false;
			}

			template <typename UnaryPredicate>
			void RemoveIf(UnaryPredicate p) noexcept
			{
				ExtractIf(std::move(p));
			}

			///@}
	};
} //ion::managed

#endif