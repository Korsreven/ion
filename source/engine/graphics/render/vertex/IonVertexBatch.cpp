/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render/vertex
File:	IonVertexBatch.cpp
-------------------------------------------
*/

#include "IonVertexBatch.h"

#include <cstddef>

#include "graphics/IonGraphicsAPI.h"
#include "graphics/materials/IonMaterial.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "graphics/textures/IonAnimation.h"
#include "graphics/textures/IonTexture.h"
#include "types/IonTypeTraits.h"

namespace ion::graphics::render::vertex
{

using namespace vertex_batch;

namespace vertex_batch::detail
{

int vertex_draw_mode_to_gl_draw_mode(VertexDrawMode draw_mode) noexcept
{
	switch (draw_mode)
	{
		case VertexDrawMode::Points:
		return GL_POINTS;

		case VertexDrawMode::Lines:
		return GL_LINES;

		case VertexDrawMode::LineLoop:
		return GL_LINE_LOOP;

		case VertexDrawMode::LineStrip:
		return GL_LINE_STRIP;

		case VertexDrawMode::TriangleFan:
		return GL_TRIANGLE_FAN;

		case VertexDrawMode::TriangleStrip:
		return GL_TRIANGLE_STRIP;

		case VertexDrawMode::Quads:
		return GL_QUADS;

		case VertexDrawMode::Polygon:
		return GL_POLYGON;

		case VertexDrawMode::Triangles:
		default:
		return GL_TRIANGLES;
	}
}

int get_vertex_count(const VertexDeclaration &vertex_declaration, const VertexDataView &vertex_data) noexcept
{
	return vertex_data && vertex_declaration.VertexSize() > 0 ?
		vertex_data.Size() / vertex_declaration.VertexSize() :
		0;
}


std::tuple<NonOwningPtr<textures::Animation>, NonOwningPtr<textures::Texture>, std::optional<int>> get_textures(const texture_type &some_texture) noexcept
{
	using tuple_type = std::tuple<NonOwningPtr<textures::Animation>, NonOwningPtr<textures::Texture>, std::optional<int>>;

	return std::visit(types::overloaded{
		[](std::monostate) { return tuple_type{nullptr, nullptr, std::nullopt}; },
		[](NonOwningPtr<textures::Animation> animation) { return tuple_type{animation, nullptr, std::nullopt}; },
		[](NonOwningPtr<textures::Texture> texture) { return tuple_type{nullptr, texture, std::nullopt}; },
		[](int texture_handle) { return tuple_type{nullptr, nullptr, texture_handle}; }
	}, some_texture);
}

std::optional<int> get_texture_handle(const texture_type &some_texture, duration time) noexcept
{
	if (auto [animation, texture, texture_handle] = get_textures(some_texture); animation)
	{
		if (auto frame = animation->FrameAt(time); frame)
			return frame->Handle();
		else
			return {};
	}
	else if (texture)
		return texture->Handle();
	else
		return texture_handle;
}


/*
	Graphics API
*/

void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;

	for (auto &vertex_element : vertex_declaration.Elements())
	{
		if (auto attribute = shader_program.GetAttribute(vertex_element.Name); attribute)
		{
			switch (vertex_element.Type)
			{
				case vertex_declaration::VertexElementType::Float1:
				attribute->Get<float>().Pointer((void*)(vbo_offset + vertex_element.Offset), vertex_element.Stride);
				break;

				case vertex_declaration::VertexElementType::Float2:
				attribute->Get<glsl::vec2>().Pointer((void*)(vbo_offset + vertex_element.Offset), vertex_element.Stride);
				break;

				case vertex_declaration::VertexElementType::Float3:
				attribute->Get<glsl::vec3>().Pointer((void*)(vbo_offset + vertex_element.Offset), vertex_element.Stride);
				break;

				case vertex_declaration::VertexElementType::Float4:
				attribute->Get<glsl::vec4>().Pointer((void*)(vbo_offset + vertex_element.Offset), vertex_element.Stride);
				break;
			}
			
			glEnableVertexAttribArray(attribute->Location().value_or(-1));
		}
	}
}

void set_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const void *data, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;

	for (auto &vertex_element : vertex_declaration.Elements())
	{
		if (auto attribute = shader_program.GetAttribute(vertex_element.Name); attribute)
		{
			auto pointer = (void*)(static_cast<const std::byte*>(data) + vertex_element.Offset);

			switch (vertex_element.Type)
			{
				case vertex_declaration::VertexElementType::Float1:
				attribute->Get<float>().Pointer(pointer, vertex_element.Stride);
				break;

				case vertex_declaration::VertexElementType::Float2:
				attribute->Get<glsl::vec2>().Pointer(pointer, vertex_element.Stride);
				break;

				case vertex_declaration::VertexElementType::Float3:
				attribute->Get<glsl::vec3>().Pointer(pointer, vertex_element.Stride);
				break;

				case vertex_declaration::VertexElementType::Float4:
				attribute->Get<glsl::vec4>().Pointer(pointer, vertex_element.Stride);
				break;
			}

			glEnableVertexAttribArray(attribute->Location().value_or(-1));
		}
	}
}

void disable_vertex_attribute_pointers(const VertexDeclaration &vertex_declaration, const shaders::ShaderProgram &shader_program) noexcept
{
	for (auto &vertex_element : vertex_declaration.Elements())
	{
		if (auto attribute = shader_program.GetAttribute(vertex_element.Name); attribute)
			glDisableVertexAttribArray(attribute->Location().value_or(-1));
	}
}


void set_vertex_pointers(const VertexDeclaration &vertex_declaration, int vbo_offset) noexcept
{
	constexpr auto type = std::is_same_v<real, float> ? GL_FLOAT : GL_DOUBLE;

	for (auto &vertex_element : vertex_declaration.Elements())
	{
		auto pointer = (void*)(vbo_offset + vertex_element.Offset);

		switch (vertex_element.Name)
		{
			case shaders::shader_layout::AttributeName::Vertex_Position:
			glVertexPointer(vertex_element.Components(), type, vertex_element.Stride, pointer);
			glEnableClientState(GL_VERTEX_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_Normal:
			glNormalPointer(/*vertex_element.Components(),*/ type, vertex_element.Stride, pointer);
			glEnableClientState(GL_NORMAL_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_Color:
			glColorPointer(vertex_element.Components(), type, vertex_element.Stride, pointer);
			glEnableClientState(GL_COLOR_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_TexCoord:
			glTexCoordPointer(vertex_element.Components(), type, vertex_element.Stride, pointer);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			break;
		}
	}
}

void set_vertex_pointers(const VertexDeclaration &vertex_declaration, const void *data) noexcept
{
	constexpr auto type = std::is_same_v<real, float> ? GL_FLOAT : GL_DOUBLE;

	for (auto &vertex_element : vertex_declaration.Elements())
	{
		auto pointer = (void*)(static_cast<const std::byte*>(data) + vertex_element.Offset);

		switch (vertex_element.Name)
		{
			case shaders::shader_layout::AttributeName::Vertex_Position:
			glVertexPointer(vertex_element.Components(), type, vertex_element.Stride, pointer);
			glEnableClientState(GL_VERTEX_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_Normal:
			glNormalPointer(/*vertex_element.Components(),*/ type, vertex_element.Stride, pointer);
			glEnableClientState(GL_NORMAL_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_Color:
			glColorPointer(vertex_element.Components(), type, vertex_element.Stride, pointer);
			glEnableClientState(GL_COLOR_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_TexCoord:
			glTexCoordPointer(vertex_element.Components(), type, vertex_element.Stride, pointer);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			break;
		}
	}
}

void disable_vertex_pointers(const VertexDeclaration &vertex_declaration) noexcept
{
	for (auto &vertex_element : vertex_declaration.Elements())
	{
		switch (vertex_element.Name)
		{
			case shaders::shader_layout::AttributeName::Vertex_Position:
			glDisableClientState(GL_VERTEX_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_Normal:
			glDisableClientState(GL_NORMAL_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_Color:
			glDisableClientState(GL_COLOR_ARRAY);
			break;

			case shaders::shader_layout::AttributeName::Vertex_TexCoord:
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			break;
		}
	}
}


void bind_texture(int texture_handle) noexcept
{
	if (texture_handle > 0)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, texture_handle);
}

void bind_texture(int texture_handle, int texture_unit) noexcept
{
	switch (gl::MultiTexture_Support())
	{
		case gl::Extension::Core:
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		break;

		case gl::Extension::ARB:
		glActiveTextureARB(GL_TEXTURE0_ARB + texture_unit);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, texture_handle);
}


void set_material_uniforms(materials::Material *material, duration time, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;

	if (auto has_material = shader_program.GetUniform(shaders::shader_layout::UniformName::Primitive_HasMaterial); has_material)
		has_material->Get<bool>() = !!material;

	if (!material)
		return; //Nothing more to set


	if (auto ambient = shader_program.GetUniform(shaders::shader_layout::UniformName::Material_Ambient); ambient)
		ambient->Get<glsl::vec4>() = material->AmbientColor();

	if (auto diffuse = shader_program.GetUniform(shaders::shader_layout::UniformName::Material_Diffuse); diffuse)
		diffuse->Get<glsl::vec4>() = material->DiffuseColor();

	if (auto specular = shader_program.GetUniform(shaders::shader_layout::UniformName::Material_Specular); specular)
		specular->Get<glsl::vec4>() = material->SpecularColor();

	if (auto emissive = shader_program.GetUniform(shaders::shader_layout::UniformName::Material_Emissive); emissive)
		emissive->Get<glsl::vec4>() = material->EmissiveColor();

	if (auto shininess = shader_program.GetUniform(shaders::shader_layout::UniformName::Material_Shininess); shininess)
		shininess->Get<float>() = material->Shininess(); //Using 'real' could make this uniform double


	auto diffuse_map_activated = false;
	auto specular_map_activated = false;
	auto normal_map_activated = false;
	
	if (auto diffuse_map = shader_program.GetUniform(shaders::shader_layout::UniformName::Material_DiffuseMap); diffuse_map)
	{
		if (auto texture = material->DiffuseMap(time); texture && texture->Handle())
		{
			if (auto texture_unit = diffuse_map->Get<glsl::sampler2D>(); texture_unit >= 0)
			{
				bind_texture(*texture->Handle(), texture_unit);
				diffuse_map_activated = true;
			}
		}
	}

	if (auto specular_map = shader_program.GetUniform(shaders::shader_layout::UniformName::Material_SpecularMap); specular_map)
	{
		if (auto texture = material->SpecularMap(time); texture && texture->Handle())
		{
			if (auto texture_unit = specular_map->Get<glsl::sampler2D>(); texture_unit >= 0)
			{
				bind_texture(*texture->Handle(), texture_unit);
				specular_map_activated = true;
			}
		}
	}
	
	if (auto normal_map = shader_program.GetUniform(shaders::shader_layout::UniformName::Material_NormalMap); normal_map)
	{
		if (auto texture = material->NormalMap(time); texture && texture->Handle())
		{
			if (auto texture_unit = normal_map->Get<glsl::sampler2D>(); texture_unit >= 0)
			{
				bind_texture(*texture->Handle(), texture_unit);
				normal_map_activated = true;
			}
		}
	}


	if (auto has_diffuse_map = shader_program.GetUniform(shaders::shader_layout::UniformName::Material_HasDiffuseMap); has_diffuse_map)
		has_diffuse_map->Get<bool>() = diffuse_map_activated;

	if (auto has_specular_map = shader_program.GetUniform(shaders::shader_layout::UniformName::Material_HasSpecularMap); has_specular_map)
		has_specular_map->Get<bool>() = specular_map_activated;

	if (auto has_normal_map = shader_program.GetUniform(shaders::shader_layout::UniformName::Material_HasNormalMap); has_normal_map)
		has_normal_map->Get<bool>() = normal_map_activated;
}

void set_texture_uniforms(texture_type &some_texture, duration time, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;
	auto texture_activated = false;

	if (auto texture = shader_program.GetUniform(shaders::shader_layout::UniformName::Primitive_Texture); texture && some_texture.index() > 0)
	{
		if (auto texture_handle = get_texture_handle(some_texture, time); texture_handle)
		{
			if (auto texture_unit = texture->Get<glsl::sampler2D>(); texture_unit >= 0)
			{
				bind_texture(*texture_handle, texture_unit);
				texture_activated = true;
			}
		}
	}

	if (auto has_texture = shader_program.GetUniform(shaders::shader_layout::UniformName::Primitive_HasTexture); has_texture)
		has_texture->Get<bool>() = texture_activated;
}

} //vertex_batch::detail


VertexBatch::VertexBatch(VertexDrawMode draw_mode, VertexDeclaration vertex_declaration) noexcept :

	draw_mode_{draw_mode},
	vertex_declaration_{std::move(vertex_declaration)}
{
	//Empty
}

VertexBatch::VertexBatch(VertexDrawMode draw_mode, VertexDeclaration vertex_declaration,
	const VertexDataView &vertex_data, NonOwningPtr<materials::Material> material) noexcept :

	draw_mode_{draw_mode},
	vertex_declaration_{std::move(vertex_declaration)},
	vertex_data_{vertex_data},
	vertex_count_{detail::get_vertex_count(vertex_declaration_, vertex_data_)},

	material_{material}
{
	//Empty
}


VertexBatch::VertexBatch(vertex_batch::VertexDrawMode draw_mode, VertexDeclaration vertex_declaration,
	const VertexDataView &vertex_data, NonOwningPtr<textures::Animation> animation) noexcept :

	draw_mode_{draw_mode},
	vertex_declaration_{std::move(vertex_declaration)},
	vertex_data_{vertex_data},
	vertex_count_{detail::get_vertex_count(vertex_declaration_, vertex_data_)},

	texture_{animation}
{
	//Empty
}

VertexBatch::VertexBatch(vertex_batch::VertexDrawMode draw_mode, VertexDeclaration vertex_declaration,
	const VertexDataView &vertex_data, NonOwningPtr<textures::Texture> texture) noexcept :

	draw_mode_{draw_mode},
	vertex_declaration_{std::move(vertex_declaration)},
	vertex_data_{vertex_data},
	vertex_count_{detail::get_vertex_count(vertex_declaration_, vertex_data_)},

	texture_{texture}
{
	//Empty
}

VertexBatch::VertexBatch(vertex_batch::VertexDrawMode draw_mode, VertexDeclaration vertex_declaration,
	const VertexDataView &vertex_data, int texture_handle) noexcept :

	draw_mode_{draw_mode},
	vertex_declaration_{std::move(vertex_declaration)},
	vertex_data_{vertex_data},
	vertex_count_{detail::get_vertex_count(vertex_declaration_, vertex_data_)},

	texture_{texture_handle}
{
	//Empty
}


/*
	Preparing / drawing
*/

void VertexBatch::Prepare() noexcept
{
	//Nothing to prepare
	if (vertex_count_ == 0)
		return;

	if (reload_vertex_data_)
	{
		//Send vertex data to VRAM
		if (vbo_ && *vbo_)
			vbo_->Data(vertex_data_);

		reload_vertex_data_ = false;
	}

	if (rebind_vertex_attributes_)
	{
		if (vbo_ && *vbo_)
		{
			if (!vao_)
				vao_.emplace();

			//Bind buffers and attributes
			if (vao_ && *vao_)
				vao_->Bind(vertex_declaration_, *vbo_);
		}

		rebind_vertex_attributes_ = false;
	}
}

void VertexBatch::Draw(shaders::ShaderProgram *shader_program) noexcept
{
	//Nothing to draw
	if (vertex_count_ == 0)
		return;
	
	auto use_shader = shader_program && shader_program->Owner() && shader_program->Handle();
	auto shader_in_use = use_shader && shader_program->Owner()->IsShaderProgramActive(*shader_program);
	auto has_all_attributes = use_shader;

	auto use_vbo = vbo_ && *vbo_;
	auto use_vao = use_shader && use_vbo && vao_ && *vao_;

	//Use shader
	if (use_shader)
	{
		//Check if shader program has all attributes declared in vertex declaration
		for (auto i = 0; auto &vertex_element : vertex_declaration_.Elements())
		{
			if (auto attribute = shader_program->GetAttribute(vertex_element.Name))
			{
				has_all_attributes &= !!attribute;
				use_vao &= attribute->Location().value_or(-1) == i++;
			}
		}

		if (!shader_in_use)
			shader_program->Owner()->ActivateShaderProgram(*shader_program);

		if (!use_vao)
		{
			if (use_vbo)
				vbo_->Bind();

			if (has_all_attributes)
			{
				//VRAM
				if (use_vbo)
					detail::set_vertex_attribute_pointers(vertex_declaration_, vbo_->Offset(), *shader_program);
				else //RAM
					detail::set_vertex_attribute_pointers(vertex_declaration_, vertex_data_.Pointer(), *shader_program);
			}
			else //Client-side
			{
				//VRAM
				if (use_vbo)
					detail::set_vertex_pointers(vertex_declaration_, vbo_->Offset());
				else //RAM
					detail::set_vertex_pointers(vertex_declaration_, vertex_data_.Pointer());
			}

			shader_program->Owner()->SendAttributeValues(*shader_program);
		}

		detail::set_material_uniforms(material_.get(), time_, *shader_program);
		detail::set_texture_uniforms(texture_, time_, *shader_program);

		shader_program->Owner()->SendUniformValues(*shader_program);
	}
	else //Fixed-function pipeline
	{
		if (!use_vao)
		{
			//VRAM
			if (use_vbo)
			{
				vbo_->Bind();
				detail::set_vertex_pointers(vertex_declaration_, vbo_->Offset());
			}
			else //RAM
				detail::set_vertex_pointers(vertex_declaration_, vertex_data_.Pointer());
		}

		//Has material or texture
		if (material_ || texture_.index() > 0)
		{
			//Enable diffuse texture
			if (auto diffuse_map = material_->DiffuseMap(time_); diffuse_map && diffuse_map->Handle())
				detail::bind_texture(*diffuse_map->Handle());

			//Enable texture
			else if (auto texture_handle = detail::get_texture_handle(texture_, time_); texture_handle)
				detail::bind_texture(*texture_handle);
		}
	}


	if (use_vao)
		vao_->Bind();

	glDrawArrays(detail::vertex_draw_mode_to_gl_draw_mode(draw_mode_), 0, vertex_count_); //Draw

	if (use_vao)
		vao_->Unbind();


	//Use shader
	if (use_shader)
	{
		if (!use_vao)
		{
			if (has_all_attributes)
				detail::disable_vertex_attribute_pointers(vertex_declaration_, *shader_program);
			else //Client-side
				detail::disable_vertex_pointers(vertex_declaration_);

			//VRAM
			if (use_vbo)
				vbo_->Unbind();
		}

		if (!shader_in_use)
			shader_program->Owner()->DeactivateShaderProgram(*shader_program);

		//Has material or texture
		if (material_ || texture_.index() > 0)
			detail::bind_texture(0, 0);
	}
	else //Fixed-function pipeline
	{
		if (!use_vao)
		{
			detail::disable_vertex_pointers(vertex_declaration_);

			//VRAM
			if (use_vbo)
				vbo_->Unbind();
		}

		//Has material or texture
		if (material_ || texture_.index() > 0)
			detail::bind_texture(0);
	}
}


/*
	Elapse time
*/

void VertexBatch::Elapse(duration time) noexcept
{
	time_ += time;
}

} //ion::graphics::render::vertex