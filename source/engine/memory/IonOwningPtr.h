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
#include <type_traits>
#include <utility>

namespace ion::memory
{
	template <typename T>
	class NonOwningPtr;

	namespace owning_ptr::detail
	{
		template <typename T>
		using pointer_type = std::unique_ptr<T>;


		struct ControlBlock
		{
			int ref_count = 1;
			bool expired = false;
		};
	} //owning_ptr::detail


	///@brief A class representing a pointer that has ownership to some data
	///@details Internally this class uses std::unique_ptr + a control block.
	///Semantically this class is a faster, non-thread safe version of std::shared_ptr.
	///Use std::shared_ptr in environments where thread safety is important!
	template <typename T>
	class OwningPtr final
	{
		template <typename U>
		friend class OwningPtr;
		
		template <typename U>
		friend class NonOwningPtr;

		public:
		
			using pointer = typename owning_ptr::detail::pointer_type<T>::pointer;
			using element_type = typename owning_ptr::detail::pointer_type<T>::element_type;
			using deleter_type = typename owning_ptr::detail::pointer_type<T>::deleter_type;

		private:

			owning_ptr::detail::pointer_type<T> ptr_;
			owning_ptr::detail::ControlBlock *ctrl_block_ = nullptr;

		public:

			///@brief Default constructor
			OwningPtr() = default;

			///@brief Constructs a new owning ptr with the given raw ptr
			explicit OwningPtr(pointer ptr) noexcept :
				ptr_{ptr},
				ctrl_block_{ptr_ ? new owning_ptr::detail::ControlBlock{} : nullptr}
			{
				//Empty
			}

			///@brief Constructs a new owning ptr with a nullptr
			OwningPtr(std::nullptr_t) noexcept :
				ptr_{nullptr}
			{
				//Empty
			}

			///@brief Constructs a new owning ptr with the given unique ptr
			OwningPtr(std::unique_ptr<T> &&ptr) noexcept :
				ptr_{std::move(ptr)},
				ctrl_block_{ptr_ ? new owning_ptr::detail::ControlBlock{} : nullptr}
			{
				//Empty
			}

			///@brief Constructs a new owning ptr with the given unique ptr of type U
			template <typename U, typename = std::enable_if_t<std::is_convertible_v<typename std::unique_ptr<U>::pointer, pointer>>>
			OwningPtr(std::unique_ptr<U> &&ptr) noexcept :
				ptr_{std::move(ptr)},
				ctrl_block_{ptr_ ? new owning_ptr::detail::ControlBlock{} : nullptr}
			{
				//Empty
			}

			///@brief Deleted copy constructor
			OwningPtr(const OwningPtr<T>&) = delete;

			///@brief Move constructor
			OwningPtr(OwningPtr<T> &&rhs) noexcept :	
				ptr_{std::move(rhs.ptr_)},
				ctrl_block_{std::exchange(rhs.ctrl_block_, nullptr)}
			{
				//Empty
			}

			///@brief Move construct a new owning ptr with the given owning ptr of type U
			template <typename U, typename = std::enable_if_t<std::is_convertible_v<typename OwningPtr<U>::pointer, pointer>>>
			OwningPtr(OwningPtr<U> &&rhs) noexcept :
				ptr_{std::move(rhs.ptr_)},
				ctrl_block_{std::exchange(rhs.ctrl_block_, nullptr)}
			{
				//Empty
			}

			///@brief Destructor
			~OwningPtr() noexcept
			{
				if (ctrl_block_)
				{
					if (--ctrl_block_->ref_count == 0)
						delete ctrl_block_;
					else
						ctrl_block_->expired = true;
				}
			}


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			auto& operator=(const OwningPtr<T>&) = delete;

			///@brief Move assignment
			auto& operator=(OwningPtr<T> &&rhs) noexcept
			{
				OwningPtr{std::move(rhs)}.swap(*this);
				return *this;
			}

			///@brief Move assign with the given owning ptr of type U
			template <typename U>
			auto& operator=(OwningPtr<U> &&rhs) noexcept
			{
				OwningPtr{std::move(rhs)}.swap(*this);
				return *this;
			}


			///@brief Compares this equal to rhs
			[[nodiscard]] auto operator==(const OwningPtr<T> &rhs) const noexcept
			{
				return ptr_ == rhs.ptr_;
			}

			///@brief Compares this different from rhs
			[[nodiscard]] auto operator!=(const OwningPtr<T> &rhs) const noexcept
			{
				return ptr_ != rhs.ptr_;
			}

			///@brief Compares this less than rhs
			[[nodiscard]] auto operator<(const OwningPtr<T> &rhs) const noexcept
			{
				return ptr_ < rhs.ptr_;
			}

			///@brief Compares this less or equal to rhs
			[[nodiscard]] auto operator<=(const OwningPtr<T> &rhs) const noexcept
			{
				return ptr_ <= rhs.ptr_;
			}

			///@brief Compares this greater than rhs
			[[nodiscard]] auto operator>(const OwningPtr<T> &rhs) const noexcept
			{
				return ptr_ > rhs.ptr_;
			}

			///@brief Compares this greater or equal to rhs
			[[nodiscard]] auto operator>=(const OwningPtr<T> &rhs) const noexcept
			{
				return ptr_ >= rhs.ptr_;
			}


			///@brief Checks if there is an associated owned object
			[[nodiscard]] explicit operator bool() const noexcept
			{
				return static_cast<bool>(ptr_);
			}

			///@brief Returns a pointer to the owned object
			[[nodiscard]] auto operator->() const noexcept
			{
				return ptr_.get();
			}

			///@brief Dereferences pointer to the owned object
			[[nodiscard]] auto& operator*() const noexcept
			{
				return *ptr_;
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Releases the ownership of the owned object if any
			[[nodiscard]] auto release() noexcept
			{
				auto ptr = ptr_.release();
				reset();
				return ptr;
			}

			///@brief Replaces the owned object
			void reset(pointer ptr = pointer{}) noexcept
			{
				OwningPtr{ptr}.swap(*this);
			}

			///@brief Swaps the owned objects
			void swap(OwningPtr<T> &rhs) noexcept
			{
				ptr_.swap(rhs.ptr_);
				std::swap(ctrl_block_, rhs.ctrl_block_);
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns a pointer to the owned object
			[[nodiscard]] auto get() const noexcept
			{
				return ptr_.get();
			}

			///@}
	};


	/**
		@name Operators
		@{
	*/

	///@brief Compares ptr equal to nullptr
	template <typename T>
	[[nodiscard]] auto operator==(const OwningPtr<T> &ptr, std::nullptr_t) noexcept
	{
		return ptr.get() == nullptr;
	}

	///@brief Compares nullptr equal to ptr
	template <typename T>
	[[nodiscard]] auto operator==(std::nullptr_t, const OwningPtr<T> &ptr) noexcept
	{
		return nullptr == ptr.get();
	}

	///@brief Compares ptr different from nullptr
	template <typename T>
	[[nodiscard]] auto operator!=(const OwningPtr<T> &ptr, std::nullptr_t) noexcept
	{
		return ptr.get() != nullptr;
	}

	///@brief Compares nullptr different from ptr
	template <typename T>
	[[nodiscard]] auto operator!=(std::nullptr_t, const OwningPtr<T> &ptr) noexcept
	{
		return nullptr != ptr.get();
	}

	///@brief Compares ptr less than nullptr
	template <typename T>
	[[nodiscard]] auto operator<(const OwningPtr<T> &ptr, std::nullptr_t) noexcept
	{
		return ptr.get() < nullptr;
	}

	///@brief Compares nullptr less than ptr
	template <typename T>
	[[nodiscard]] auto operator<(std::nullptr_t, const OwningPtr<T> &ptr) noexcept
	{
		return nullptr < ptr.get();
	}

	///@brief Compares ptr less or equal to nullptr
	template <typename T>
	[[nodiscard]] auto operator<=(const OwningPtr<T> &ptr, std::nullptr_t) noexcept
	{
		return ptr.get() <= nullptr;
	}

	///@brief Compares nullptr less or equal to ptr
	template <typename T>
	[[nodiscard]] auto operator<=(std::nullptr_t, const OwningPtr<T> &ptr) noexcept
	{
		return nullptr <= ptr.get();
	}

	///@brief Compares ptr greater than nullptr
	template <typename T>
	[[nodiscard]] auto operator>(const OwningPtr<T> &ptr, std::nullptr_t) noexcept
	{
		return ptr.get() > nullptr;
	}

	///@brief Compares nullptr greater than ptr
	template <typename T>
	[[nodiscard]] auto operator>(std::nullptr_t, const OwningPtr<T> &ptr) noexcept
	{
		return nullptr > ptr.get();
	}

	///@brief Compares ptr greater or equal to nullptr
	template <typename T>
	[[nodiscard]] auto operator>=(const OwningPtr<T> &ptr, std::nullptr_t) noexcept
	{
		return ptr.get() >= nullptr;
	}

	///@brief Compares nullptr greater or equal to ptr
	template <typename T>
	[[nodiscard]] auto operator>=(std::nullptr_t, const OwningPtr<T> &ptr) noexcept
	{
		return nullptr >= ptr.get();
	}


	///@brief Helper function for creating a owning ptr
	template <typename T, typename... Args>
	[[nodiscard]] auto make_owning(Args &&...args)
	{
		return OwningPtr<T>{std::make_unique<T>(std::forward<Args>(args)...)};
	}

	///@}
} //ion::memory

namespace ion
{
	/**
		@name Pull common stuff out of the memory namespace
		@{
	*/

	using memory::OwningPtr;
	using memory::make_owning;

	///@}
} //ion

#endif