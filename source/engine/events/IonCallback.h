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

#ifndef _ION_CALLBACK_
#define _ION_CALLBACK_

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


	template <typename Ret = void, typename... Args>
	class Callback
	{
		public:

			using functor_type = std::function<Ret(Args...)>;

		private:

			functor_type functor_;

		public:

			Callback(functor_type functor) noexcept :
				functor_(std::move(functor))
			{
				//Empty
			}

			template <typename Callable>
			Callback(Callable callable) noexcept :
				functor_(callable)
			{
				static_assert(std::is_invocable_v<Callable, Args...>);
			}
			
			template <typename Callable, typename Caller>
			Callback(Callable callable, Caller *const caller) noexcept :
				functor_(callback::detail::bind(callable, caller))
			{
				static_assert(std::is_invocable_v<Callable, Caller, Args...>);
				assert(caller != nullptr);
			}

			template <typename Callable, typename Caller>
			Callback(Callable callable, Caller &caller) noexcept :
				functor_(callback::detail::bind(callable, &caller))
			{
				static_assert(std::is_invocable_v<Callable, Caller, Args...>);
			}


			/*
				Function call operator
			*/

			//Calls the callback with the given arguments
			inline auto operator()(Args &&...args) const noexcept
			{
				return functor_(std::forward<Args>(args)...);
			}
	};
} //ion::events

#endif