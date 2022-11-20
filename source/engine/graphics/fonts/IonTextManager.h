/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/fonts
File:	IonTextManager.h
-------------------------------------------
*/

#ifndef ION_TEXT_MANAGER_H
#define ION_TEXT_MANAGER_H

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "IonText.h"
#include "managed/IonObjectManager.h"
#include "memory/IonNonOwningPtr.h"

namespace ion::graphics::fonts
{
	namespace text_manager::detail
	{
	} //text_manager::detail


	///@brief A class that manages and stores texts
	class TextManager final :
		public managed::ObjectManager<Text, TextManager>
	{
		public:
		
			///@brief Default constructor
			TextManager() = default;

			///@brief Deleted copy constructor
			TextManager(const TextManager&) = delete;

			///@brief Default move constructor
			TextManager(TextManager&&) = default;

			///@brief Destructor
			~TextManager() = default;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			TextManager& operator=(const TextManager&) = delete;

			///@brief Default move assignment
			TextManager& operator=(TextManager&&) = default;

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all texts in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Texts() noexcept
			{
				return Objects();
			}

			///@brief Returns an immutable range of all texts in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto Texts() const noexcept
			{
				return Objects();
			}

			///@}

			/**
				@name Texts - Creating
				@{
			*/

			///@brief Creates a (unbounded) text with the given name, content, alignment and a type face
			NonOwningPtr<Text> CreateText(std::string name, std::string content, text::TextAlignment alignment,
				NonOwningPtr<TypeFace> type_face);

			///@brief Creates a (unbounded) text with the given name, content and a type face
			NonOwningPtr<Text> CreateText(std::string name, std::string content, NonOwningPtr<TypeFace> type_face);

			///@brief Creates a text (area) with the given name, content, formatting,
			///horizontal and vertical alignment, area size, padding, line height factor and a type face
			NonOwningPtr<Text> CreateText(std::string name, std::string content, text::TextFormatting formatting,
				text::TextAlignment alignment, text::TextVerticalAlignment vertical_alignment,
				const std::optional<Vector2> &area_size, const Vector2 &padding,
				std::optional<real> line_height_factor, NonOwningPtr<TypeFace> type_face);

			///@brief Creates a text (area) with the given name, content,
			///horizontal and vertical alignment, area size, padding, line height factor and a type face
			NonOwningPtr<Text> CreateText(std::string name, std::string content,
				text::TextAlignment alignment, text::TextVerticalAlignment vertical_alignment,
				const std::optional<Vector2> &area_size, const Vector2 &padding,
				std::optional<real> line_height_factor, NonOwningPtr<TypeFace> type_face);

			///@brief Creates a text (area) with the given name, content,
			///area size, padding, line height factor and a type face
			NonOwningPtr<Text> CreateText(std::string name, std::string content,
				const std::optional<Vector2> &area_size, const Vector2 &padding,
				std::optional<real> line_height_factor, NonOwningPtr<TypeFace> type_face);


			///@brief Creates a text as a copy of the given text
			NonOwningPtr<Text> CreateText(const Text &text);

			///@brief Creates a text by moving the given text
			NonOwningPtr<Text> CreateText(Text &&text);

			///@}

			/**
				@name Texts - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable text with the given name
			///@details Returns nullptr if text could not be found
			[[nodiscard]] NonOwningPtr<Text> GetText(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable text with the given name
			///@details Returns nullptr if text could not be found
			[[nodiscard]] NonOwningPtr<const Text> GetText(std::string_view name) const noexcept;

			///@}

			/**
				@name Texts - Removing
				@{
			*/

			///@brief Clears all removable texts from this manager
			void ClearTexts() noexcept;

			///@brief Removes a removable text from this manager
			bool RemoveText(Text &text) noexcept;

			///@brief Removes a removable text with the given name from this manager
			bool RemoveText(std::string_view name) noexcept;

			///@}
	};
} //ion::graphics::fonts

#endif