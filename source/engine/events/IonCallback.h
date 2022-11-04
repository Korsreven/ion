/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events
File:	IonCallback.h
-------------------------------------------
*/

#ifndef ION_CALLBACK_H
#define ION_CALLBACK_H

#include <cassert>
#include <functional>
#include <type_traits>

namespace ion::events
{
	namespace callback::detail
	{
		template <typename Callable, typename Caller>
		inline auto bind(Callable callable, Caller *const caller) noexcept
		{
			return [callable, caller](auto &&...args) noexcept
				{
					return (caller->*callable)(std::forward<decltype(args)>(args)...);
				};
		};
	} //callback::detail


	///@brief A class representing a callback that matches the given signature
	///@details A callback is anything invocable, like a free function, member function, lambda or an object with a call operator
	template <typename Ret = void, typename... Args>
	class Callback
	{
		public:

			using functor_type = std::function<Ret(Args...)>;

		private:

			functor_type functor_;

		public:

			///@brief Default constructor
			Callback() = default;

			///@brief Default constructor
			Callback(functor_type functor) noexcept :
				functor_(std::move(functor))
			{
				//Empty
			}

			///@brief Constructs a callback with the given callable
			template <typename Callable>
			Callback(Callable callable) noexcept :
				functor_(callable)
			{
				static_assert(std::is_invocable_v<Callable, Args...>);
			}
			
			///@brief Constructs a callback with the given callable and caller
			template <typename Callable, typename Caller>
			Callback(Callable callable, Caller *const caller) noexcept :
				functor_(callback::detail::bind(callable, caller))
			{
				static_assert(std::is_invocable_v<Callable, Caller, Args...>);
				assert(caller != nullptr);
			}

			///@brief Constructs a callback with the given callable and caller
			template <typename Callable, typename Caller>
			Callback(Callable callable, Caller &caller) noexcept :
				functor_(callback::detail::bind(callable, &caller))
			{
				static_assert(std::is_invocable_v<Callable, Caller, Args...>);
			}


			/**
				@name Function call operator
				@{
			*/

			///@brief Calls the callback with the given arguments
			inline auto operator()(Args ...args) const noexcept
			{
				return functor_(std::forward<Args>(args)...);
			}

			///@}
	};
} //ion::events

#endif