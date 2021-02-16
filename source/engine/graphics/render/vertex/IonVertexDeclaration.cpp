/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render/vertex
File:	IonVertexDeclaration.cpp
-------------------------------------------
*/

#include "IonVertexDeclaration.h"

namespace ion::graphics::render::vertex
{

using namespace vertex_declaration;

namespace vertex_declaration
{

//Vertex element

VertexElement::VertexElement(shaders::shader_layout::AttributeName name, VertexElementType type, int offset, int stride) noexcept :
	Name{name}, Type{type}, Offset{offset}, Stride{stride}
{
	//Empty
}


/*
	Observers
*/

int VertexElement::Components() const noexcept
{
	return static_cast<int>(Type);
}

} //vertex_declaration


VertexDeclaration::VertexDeclaration(vertex_declaration::VertexElements vertex_elements, std::optional<int> vertex_size = std::nullopt) :
	
	vertex_elements_{std::move(vertex_elements)},
	vertex_size_{vertex_size}
{
	//Empty
}


/*
	Observers
*/

int VertexDeclaration::Components() const noexcept
{
	auto components = 0;

	for (auto &element : Elements())
		components += element.Components();

	return components;
}


/*
	Vertex elements
	Adding
*/

void VertexDeclaration::AddElement(shaders::shader_layout::AttributeName name, vertex_declaration::VertexElementType type, int offset, int stride)
{
	vertex_elements_.emplace_back(name, type, offset, stride);
}

void VertexDeclaration::AddElement(const vertex_declaration::VertexElement &element)
{
	vertex_elements_.push_back(element);
}


/*
	Vertex elements
	Removing
*/

void VertexDeclaration::ClearElements() noexcept
{
	vertex_elements_.clear();
	vertex_elements_.shrink_to_fit();
}

} //ion::graphics::render::vertex