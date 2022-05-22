/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	events
File:	IonEventGenerator.h
-------------------------------------------
*/

#ifndef ION_EVENT_GENERATOR_H
#define ION_EVENT_GENERATOR_H

#include <functional>
#include <optional>
#include <type_traits>
#include <vector>

#include "listeners/IonListener.h"

namespace ion::events
{
	namespace event_generator::detail
	{
		template <typename T, typename = void>
		struct has_value_type : std::false_type {};

		template <typename T>
		struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};

		template <typename T>
		constexpr auto has_value_type_v = has_value_type<T>::value;


		//This type is very similar to std::monostate
		//But we do not want to #include <variant> just to get it
		struct empty final {};

		template <typename T>
		struct notify_result
		{
			using type = T;
		};

		template <>
		struct notify_result<void>
		{
			using type = empty;
		};

		template <typename T>
		using notify_result_t = typename notify_result<T>::type;


		template <typename Event, typename T, typename... Args>
		inline auto notify(Event &&event, T &listener, Args &&...args) noexcept
		{
			if (listener.Listening())
			{
				if constexpr (std::is_same_v<std::invoke_result_t<Event, T, Args...>, void>)
				{
					std::invoke(std::forward<Event>(event), std::forward<T>(listener), std::forward<Args>(args)...);
					return std::make_optional(notify_result_t<void>{});
				}
				else
					return std::make_optional(
						std::invoke(std::forward<Event>(event), std::forward<T>(listener), std::forward<Args>(args)...));
			}

			return std::optional<notify_result_t<std::invoke_result_t<Event, T, Args...>>>{};
		}
	} //event_generator::detail


	template <typename T>
	struct EventGenerator
	{
		static_assert(std::is_base_of_v<listeners::Listener<T>, T>);

		template <typename Event, typename... Args>
		inline auto Notify(Event &&event, T &listener, Args &&...args) noexcept
		{
			static_assert(std::is_invocable_v<Event, T, Args...>);
			return event_generator::detail::notify(std::forward<Event>(event), listener, std::forward<Args>(args)...);
		}

		template <typename Range, typename Event, typename... Args>
		inline auto NotifyAll(Range &&listeners, Event &&event, Args &&...args) noexcept
		{
			static_assert(event_generator::detail::has_value_type_v<Range> &&
						  std::is_same_v<typename Range::value_type, T*>);

			if (!std::empty(listeners))
			{
				std::vector<T*> listeners_to_notify;
				listeners_to_notify.reserve(std::size(listeners));

				for (auto &listener : listeners)
					listeners_to_notify.push_back(&listener);

				for (auto &listener : listeners_to_notify)
					Notify(event, *listener, args...);
						//The actual notify call could be modifying 'listeners', but not the 'listeners_to_notify' copy
						//This will make sure that all listeners at the time of the event, will be notified
			}
		}
	};
} //ion::events

#endif