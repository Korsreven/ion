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

#include "IonOwningPtr.h"

namespace ion::memory
{
	template <typename T>
	class NonOwningPtr final
	{
		private:

			owning_ptr::detail::ControlBlock<T> *ctrl_block_ = nullptr;

		public:

			using pointer = T*;
			using element_type = T;


			//Default constructor
			NonOwningPtr() = default;

			//Construct a new non-owning ptr with the given owning ptr
			NonOwningPtr(const OwningPtr<T> &ptr) noexcept :
				ctrl_block_{ptr.ctrl_block_}
			{
				if (ctrl_block_)
					++ctrl_block_->ref_count;
			}

			//Copy constructor
			NonOwningPtr(const NonOwningPtr<T> &rhs) noexcept :
				ctrl_block_{rhs.ctrl_block_}
			{
				if (ctrl_block_)
					++ctrl_block_->ref_count;
			}

			//Move constructor
			NonOwningPtr(NonOwningPtr<T> &&rhs) noexcept :	
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

			//Copy assignment
			auto& operator=(const NonOwningPtr<T> &rhs) noexcept
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


			//Checks if there is an associated watched object
			[[nodiscard]] explicit operator bool() const noexcept
			{
				return ctrl_block_ && ctrl_block_->ptr_;
			}

			//Returns a pointer to the watched object
			[[nodiscard]] auto operator->() const noexcept
			{
				return ctrl_block_->ptr_;
			}

			//Dereferences pointer to the watched object
			[[nodiscard]] auto& operator*() const noexcept
			{
				return *ctrl_block_->ptr_;
			}


			/*
				Modifers
			*/

			//Releases the watched object
			void reset() noexcept
			{
				NonOwningPtr{}.swap(*this);
			}

			//Swaps the watched objects
			void swap(NonOwningPtr<T> &rhs) noexcept
			{
				std::swap(ctrl_block_, rhs.ctrl_block_);
			}


			/*
				Observers
			*/

			//Returns a pointer to the watched object
			[[nodiscard]] auto get() const noexcept
			{
				return ctrl_block_->ptr_;
			}

			//Checks whether the watched object was already deleted
			[[nodiscard]] auto expired() const noexcept
			{
				return !ctrl_block_ || !ctrl_block_->ptr;
			}
	};
} //ion::memory

namespace ion
{
	/*
		Pull common stuff out of the memory namespace
	*/

	using memory::NonOwningPtr;
} //ion

#endif