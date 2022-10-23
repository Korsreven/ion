/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	memory
File:	IonNonOwningPtr.h
-------------------------------------------
*/

#ifndef ION_NON_OWNING_PTR_H
#define ION_NON_OWNING_PTR_H

#include <type_traits>
#include "IonOwningPtr.h"

namespace ion::memory
{
	namespace non_owning_ptr::detail
	{
		inline auto inc_ref_count(owning_ptr::detail::ControlBlock *ctrl_block) noexcept
		{
			if (ctrl_block)
				++ctrl_block->ref_count;

			return ctrl_block;
		}
	} //non_owning_ptr::detail


	//A class representing a pointer that has non-ownership to some data
	//Internally this class uses a raw pointer + a pointer to the control block
	//A non-owning pointer becomes nullptr when the owned data has expired
	//Use raw pointers when you know that the owned data has not expired yet!
	template <typename T>
	class NonOwningPtr final
	{
		template <typename U>
		friend class NonOwningPtr;

		public:

			using pointer = typename OwningPtr<T>::pointer;
			using element_type = typename OwningPtr<T>::element_type;

		private:

			pointer ptr_ = nullptr;
			owning_ptr::detail::ControlBlock *ctrl_block_ = nullptr;

		public:

			//Default constructor
			NonOwningPtr() = default;

			//Constructs a new non-owning ptr with a nullptr
			NonOwningPtr(std::nullptr_t) noexcept
			{
				//Empty
			}

			//Constructs a new non-owning ptr with the given owning ptr
			NonOwningPtr(const OwningPtr<T> &ptr) noexcept :
				ptr_{ptr.get()},
				ctrl_block_{non_owning_ptr::detail::inc_ref_count(ptr.ctrl_block_)}
			{
				//Empty
			}

			//Constructs a new non-owning ptr with the given owning ptr of type U
			template <typename U, typename = std::enable_if_t<std::is_convertible_v<typename OwningPtr<U>::pointer, pointer>>>
			NonOwningPtr(const OwningPtr<U> &ptr) noexcept :
				ptr_{static_cast<pointer>(ptr.get())},
				ctrl_block_{non_owning_ptr::detail::inc_ref_count(ptr.ctrl_block_)}
			{
				//Empty
			}

			//Copy constructor
			NonOwningPtr(const NonOwningPtr<T> &rhs) noexcept :
				ptr_{rhs.ptr_},
				ctrl_block_{non_owning_ptr::detail::inc_ref_count(rhs.ctrl_block_)}
			{
				//Empty
			}

			//Copy construct a new non-owning ptr with the given non-owning ptr of type U
			template <typename U, typename = std::enable_if_t<std::is_convertible_v<typename NonOwningPtr<U>::pointer, pointer>>>
			NonOwningPtr(const NonOwningPtr<U> &rhs) noexcept :
				ptr_{static_cast<pointer>(rhs.ptr_)},
				ctrl_block_{non_owning_ptr::detail::inc_ref_count(rhs.ctrl_block_)}
			{
				//Empty
			}

			//Copy construct a new non-owning ptr with the given non-owning ptr of type U and the given ptr
			template <typename U>
			NonOwningPtr(const NonOwningPtr<U> &rhs, pointer ptr) noexcept :
				ptr_{ptr},
				ctrl_block_{non_owning_ptr::detail::inc_ref_count(rhs.ctrl_block_)}
			{
				//Empty
			}

			//Move constructor
			NonOwningPtr(NonOwningPtr<T> &&rhs) noexcept :
				ptr_{std::exchange(rhs.ptr_, nullptr)},
				ctrl_block_{std::exchange(rhs.ctrl_block_, nullptr)}
			{
				//Empty
			}

			//Move construct a new non-owning ptr with the given non-owning ptr of type U
			template <typename U, typename = std::enable_if_t<std::is_convertible_v<typename NonOwningPtr<U>::pointer, pointer>>>
			NonOwningPtr(NonOwningPtr<U> &&rhs) noexcept :
				ptr_{static_cast<pointer>(std::exchange(rhs.ptr_, nullptr))},
				ctrl_block_{std::exchange(rhs.ctrl_block_, nullptr)}
			{
				//Empty
			}

			//Move construct a new non-owning ptr with the given non-owning ptr of type U and the given ptr
			template <typename U>
			NonOwningPtr(NonOwningPtr<U> &&rhs, pointer ptr) noexcept :
				ptr_{ptr},
				ctrl_block_{std::exchange(rhs.ctrl_block_, nullptr)}
			{
				//Empty
			}

			//Destructor
			~NonOwningPtr() noexcept
			{
				if (ctrl_block_ && --ctrl_block_->ref_count == 0)
					delete ctrl_block_;
			}


			/*
				Operators
			*/

			//Assign the non-owning ptr with the given owning ptr
			auto& operator=(const OwningPtr<T> &ptr) noexcept
			{
				NonOwningPtr{ptr}.swap(*this);
				return *this;
			}

			//Assign the non-owning ptr with the given owning ptr of type U
			template <typename U>
			auto& operator=(const OwningPtr<U> &ptr) noexcept
			{
				NonOwningPtr{ptr}.swap(*this);
				return *this;
			}

			//Copy assignment
			auto& operator=(const NonOwningPtr<T> &rhs) noexcept
			{
				NonOwningPtr{rhs}.swap(*this);
				return *this;
			}

			//Copy assign with the given non-owning ptr of type U
			template <typename U>
			auto& operator=(const NonOwningPtr<U> &rhs) noexcept
			{
				NonOwningPtr{rhs}.swap(*this);
				return *this;
			}

			//Move assignment
			auto& operator=(NonOwningPtr<T> &&rhs) noexcept
			{
				NonOwningPtr{std::move(rhs)}.swap(*this);
				return *this;
			}

			//Move assign with the given non-owning ptr of type U
			template <typename U>
			auto& operator=(NonOwningPtr<U> &&rhs) noexcept
			{
				NonOwningPtr{std::move(rhs)}.swap(*this);
				return *this;
			}


			//Compares this equal to rhs
			[[nodiscard]] auto operator==(const NonOwningPtr<T> &rhs) const noexcept
			{
				return ctrl_block_ == rhs.ctrl_block_;
			}

			//Compares this different from rhs
			[[nodiscard]] auto operator!=(const NonOwningPtr<T> &rhs) const noexcept
			{
				return ctrl_block_ != rhs.ctrl_block_;
			}

			//Compares this less than rhs
			[[nodiscard]] auto operator<(const NonOwningPtr<T> &rhs) const noexcept
			{
				return ctrl_block_ < rhs.ctrl_block_;
			}

			//Compares this less or equal to rhs
			[[nodiscard]] auto operator<=(const NonOwningPtr<T> &rhs) const noexcept
			{
				return ctrl_block_ <= rhs.ctrl_block_;
			}

			//Compares this greater than rhs
			[[nodiscard]] auto operator>(const NonOwningPtr<T> &rhs) const noexcept
			{
				return ctrl_block_ > rhs.ctrl_block_;
			}

			//Compares this greater or equal to rhs
			[[nodiscard]] auto operator>=(const NonOwningPtr<T> &rhs) const noexcept
			{
				return ctrl_block_ >= rhs.ctrl_block_;
			}


			//Checks if there is an associated watched object
			[[nodiscard]] explicit operator bool() const noexcept
			{
				return ctrl_block_ && !ctrl_block_->expired;
			}

			//Returns a pointer to the watched object
			[[nodiscard]] auto operator->() const noexcept
			{
				return ptr_;
			}

			//Dereferences pointer to the watched object
			[[nodiscard]] auto& operator*() const noexcept
			{
				return *ptr_;
			}


			/*
				Modifers
			*/

			//Replaces the watched object
			void reset(pointer ptr = pointer{}) noexcept
			{
				NonOwningPtr{ptr}.swap(*this);
			}

			//Swaps the watched objects
			void swap(NonOwningPtr<T> &rhs) noexcept
			{
				std::swap(ptr_, rhs.ptr_);
				std::swap(ctrl_block_, rhs.ctrl_block_);
			}


			/*
				Observers
			*/

			//Returns a pointer to the watched object
			//Returns nullptr if the watched object has expired
			[[nodiscard]] auto get() const noexcept
			{
				return *this ? ptr_ : nullptr;
			}

			//Checks whether the watched object was already deleted
			[[nodiscard]] auto expired() const noexcept
			{
				return !*this;
			}
	};


	/*
		Operators

		NonOwningPtr<T> <=> nullptr
		nullptr <=> NonOwningPtr<T>
	*/

	//Compares ptr equal to nullptr
	template <typename T>
	[[nodiscard]] auto operator==(const NonOwningPtr<T> &ptr, std::nullptr_t) noexcept
	{
		return ptr.get() == nullptr;
	}

	//Compares nullptr equal to ptr
	template <typename T>
	[[nodiscard]] auto operator==(std::nullptr_t, const NonOwningPtr<T> &ptr) noexcept
	{
		return nullptr == ptr.get();
	}

	//Compares ptr different from nullptr
	template <typename T>
	[[nodiscard]] auto operator!=(const NonOwningPtr<T> &ptr, std::nullptr_t) noexcept
	{
		return ptr.get() != nullptr;
	}

	//Compares nullptr different from ptr
	template <typename T>
	[[nodiscard]] auto operator!=(std::nullptr_t, const NonOwningPtr<T> &ptr) noexcept
	{
		return nullptr != ptr.get();
	}

	//Compares ptr less than nullptr
	template <typename T>
	[[nodiscard]] auto operator<(const NonOwningPtr<T> &ptr, std::nullptr_t) noexcept
	{
		return ptr.get() < nullptr;
	}

	//Compares nullptr less than ptr
	template <typename T>
	[[nodiscard]] auto operator<(std::nullptr_t, const NonOwningPtr<T> &ptr) noexcept
	{
		return nullptr < ptr.get();
	}

	//Compares ptr less or equal to nullptr
	template <typename T>
	[[nodiscard]] auto operator<=(const NonOwningPtr<T> &ptr, std::nullptr_t) noexcept
	{
		return ptr.get() <= nullptr;
	}

	//Compares nullptr less or equal to ptr
	template <typename T>
	[[nodiscard]] auto operator<=(std::nullptr_t, const NonOwningPtr<T> &ptr) noexcept
	{
		return nullptr <= ptr.get();
	}

	//Compares ptr greater than nullptr
	template <typename T>
	[[nodiscard]] auto operator>(const NonOwningPtr<T> &ptr, std::nullptr_t) noexcept
	{
		return ptr.get() > nullptr;
	}

	//Compares nullptr greater than ptr
	template <typename T>
	[[nodiscard]] auto operator>(std::nullptr_t, const NonOwningPtr<T> &ptr) noexcept
	{
		return nullptr > ptr.get();
	}

	//Compares ptr greater or equal to nullptr
	template <typename T>
	[[nodiscard]] auto operator>=(const NonOwningPtr<T> &ptr, std::nullptr_t) noexcept
	{
		return ptr.get() >= nullptr;
	}

	//Compares nullptr greater or equal to ptr
	template <typename T>
	[[nodiscard]] auto operator>=(std::nullptr_t, const NonOwningPtr<T> &ptr) noexcept
	{
		return nullptr >= ptr.get();
	}


	/*
		Pointer casts
	*/

	template <typename T, typename U>
	[[nodiscard]] auto static_pointer_cast(const NonOwningPtr<U> &ptr) noexcept
	{
		auto p = static_cast<typename NonOwningPtr<T>::pointer>(ptr.get());
		return NonOwningPtr<T>{ptr, p};
	}

	template <typename T, typename U>
	[[nodiscard]] auto static_pointer_cast(NonOwningPtr<U> &&ptr) noexcept
	{
		auto p = static_cast<typename NonOwningPtr<T>::pointer>(ptr.get());
		return NonOwningPtr<T>{std::move(ptr), p};
	}


	template <typename T, typename U>
	[[nodiscard]] auto dynamic_pointer_cast(const NonOwningPtr<U> &ptr) noexcept
	{
		if (auto p = dynamic_cast<typename NonOwningPtr<T>::pointer>(ptr.get()); p)
			return NonOwningPtr<T>{ptr, p};
		else
			return NonOwningPtr<T>{};
	}

	template <typename T, typename U>
	[[nodiscard]] auto dynamic_pointer_cast(NonOwningPtr<U> &&ptr) noexcept
	{
		if (auto p = dynamic_cast<typename NonOwningPtr<T>::pointer>(ptr.get()); p)
			return NonOwningPtr<T>{std::move(ptr), p};
		else
			return NonOwningPtr<T>{};
	}


	template <typename T, typename U>
	[[nodiscard]] auto const_pointer_cast(const NonOwningPtr<U> &ptr) noexcept
	{
		auto p = const_cast<typename NonOwningPtr<T>::pointer>(ptr.get());
		return NonOwningPtr<T>{ptr, p};
	}

	template <typename T, typename U>
	[[nodiscard]] auto const_pointer_cast(NonOwningPtr<U> &&ptr) noexcept
	{
		auto p = const_cast<typename NonOwningPtr<T>::pointer>(ptr.get());
		return NonOwningPtr<T>{std::move(ptr), p};
	}


	template <typename T, typename U>
	[[nodiscard]] auto reinterpret_pointer_cast(const NonOwningPtr<U> &ptr) noexcept
	{
		auto p = reinterpret_cast<typename NonOwningPtr<T>::pointer>(ptr.get());
		return NonOwningPtr<T>{ptr, p};
	}

	template <typename T, typename U>
	[[nodiscard]] auto reinterpret_pointer_cast(NonOwningPtr<U> &&ptr) noexcept
	{
		auto p = reinterpret_cast<typename NonOwningPtr<T>::pointer>(ptr.get());
		return NonOwningPtr<T>{std::move(ptr), p};
	}
} //ion::memory

namespace ion
{
	/*
		Pull common stuff out of the memory namespace
	*/

	using memory::NonOwningPtr;

	using memory::static_pointer_cast;
	using memory::dynamic_pointer_cast;
	using memory::const_pointer_cast;
	using memory::reinterpret_pointer_cast;
} //ion

#endif