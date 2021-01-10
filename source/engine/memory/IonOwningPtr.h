/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	memory
File:	IonOwningPtr.h
-------------------------------------------
*/

#ifndef ION_OWNING_PTR_H
#define ION_OWNING_PTR_H

#include <memory>

namespace ion::memory
{
	template <typename T>
	class NonOwningPtr;

	namespace owning_ptr::detail
	{
		template <typename T>
		struct ControlBlock
		{
			T *ptr = nullptr;
			int ref_count = 1;
		};
	} //owning_ptr::detail


	template <typename T>
	class OwningPtr final
	{
		friend NonOwningPtr<T>;

		private:

			std::unique_ptr<T> ptr_;
			owning_ptr::detail::ControlBlock<T> *ctrl_block_ = nullptr;

		public:
		
			using pointer = T*;
			using element_type = T;


			//Default constructor
			OwningPtr() = default;

			//Construct a new owning ptr with the given raw ptr
			explicit OwningPtr(T *ptr) noexcept :
				ptr_{ptr},
				ctrl_block_{ptr_ ? new owning_ptr::detail::ControlBlock<T>{ptr_.get()} : nullptr}
			{
				//Empty
			}

			//Construct a new owning ptr with a nullptr
			OwningPtr(std::nullptr_t) noexcept :
				ptr_{nullptr}
			{
				//Empty
			}

			//Construct a new owning ptr with the given unique ptr
			OwningPtr(std::unique_ptr<T> &&ptr) noexcept :
				ptr_{std::move(ptr)},
				ctrl_block_{ptr_ ? new owning_ptr::detail::ControlBlock<T>{ptr_.get()} : nullptr}
			{
				//Empty
			}

			//Deleted copy constructor
			OwningPtr(const OwningPtr<T>&) = delete;

			//Move constructor
			OwningPtr(OwningPtr<T> &&rhs) noexcept :	
				ptr_{std::move(rhs.ptr_)},
				ctrl_block_{std::exchange(rhs.ctrl_block_, nullptr)}
			{
				//Empty
			}

			//Destructor
			~OwningPtr() noexcept
			{
				if (ctrl_block_)
				{
					if (--ctrl_block_->ref_count == 0)
						delete ctrl_block_;
					else
						ctrl_block_->ptr = nullptr;
				}
			}


			/*
				Operators
			*/

			//Deleted copy assignment
			auto& operator=(const OwningPtr<T>&) = delete;

			//Move assignment
			auto& operator=(OwningPtr<T> &&rhs) noexcept
			{
				OwningPtr{std::move(rhs)}.swap(*this);
				return *this;
			}


			//Checks if there is an associated owned object
			[[nodiscard]] explicit operator bool() const noexcept
			{
				return static_cast<bool>(ptr_);
			}

			//Returns a pointer to the owned object
			[[nodiscard]] auto operator->() const noexcept
			{
				return ptr_.get();
			}

			//Dereferences pointer to the owned object
			[[nodiscard]] auto& operator*() const noexcept
			{
				return *ptr_;
			}
		

			/*
				Modifiers
			*/

			//Releases the ownership of the owned object if any
			[[nodiscard]] auto release() noexcept
			{
				auto ptr = ptr_.release();
				reset();
				return ptr;
			}

			//Replaces the owned object
			void reset(T *ptr = nullptr) noexcept
			{
				OwningPtr{ptr}.swap(*this);
			}

			//Swaps the owned objects
			void swap(OwningPtr<T> &rhs) noexcept
			{
				ptr_.swap(rhs.ptr_);
				std::swap(ctrl_block_, rhs.ctrl_block_);
			}


			/*
				Observers
			*/

			//Returns a pointer to the owned object
			[[nodiscard]] auto get() const noexcept
			{
				return ptr_.get();
			}
	};


	//Helper function for creating a owning ptr
	template <typename T, typename... Args>
	[[nodiscard]] auto make_owning(Args &&...args)
	{
		return OwningPtr<T>{std::make_unique<T>(std::forward<Args>(args)...)};
	}
} //ion::memory

namespace ion
{
	/*
		Pull common stuff out of the memory namespace
	*/

	using memory::OwningPtr;
	using memory::make_owning;
} //ion

#endif