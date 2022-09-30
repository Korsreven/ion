/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonRenderer.cpp
-------------------------------------------
*/

#include "IonRenderer.h"

#include <algorithm>
#include "graphics/IonGraphicsAPI.h"
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"

namespace ion::graphics::render
{

using namespace renderer;

namespace renderer::detail
{

//Batch primitive

render_batch_slot::render_batch_slot(int capacity, RenderPrimitive *primitive) noexcept :

	capacity{capacity},
	primitive{primitive}
{
	//Empty
}


//Render batch

render_batch::render_batch(real z, int offset, int capacity, vertex::VertexBatch vertex_batch) noexcept :

	z{z},
	offset{offset},
	capacity{capacity},

	vertex_batch{vertex_batch}
{
	//Empty
}


/*
	Graphics API
*/

void set_point_size(real size) noexcept
{
	glPointSize(static_cast<float>(size));
}

void set_line_width(real width) noexcept
{
	glLineWidth(static_cast<float>(width));
}


void enable_wire_frame() noexcept
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void disable_wire_frame() noexcept
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void enable_point_sprite() noexcept
{
	switch (gl::PointSprite_Support())
	{
		case gl::Extension::Core:
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE); //Enable sprite tex coords
		glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT); //Set 0,0 to lower left
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE); //Enable varying point size
		glEnable(GL_POINT_SPRITE); //Enable point sprite
		break;

		case gl::Extension::ARB:
		glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE); //Enable sprite tex coords
		glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT); //Set 0,0 to lower left
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB); //Enable varying point size
		glEnable(GL_POINT_SPRITE_ARB); //Enable point sprite
		break;
	}
}

void disable_point_sprite() noexcept
{
	switch (gl::PointSprite_Support())
	{
		case gl::Extension::Core:
		glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT); //Set 0,0 back to upper left
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_FALSE); //Disable sprite tex coords
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE); //Disable varying point size
		glDisable(GL_POINT_SPRITE); //Disable point sprite
		break;

		case gl::Extension::ARB:
		glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT); //Set 0,0 back to upper left
		glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE); //Disable sprite tex coords
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB); //Disable varying point size
		glDisable(GL_POINT_SPRITE_ARB); //Disable point sprite
		break;
	}
}

} //renderer::detail


//Private

NonOwningPtr<renderer::detail::render_batch> Renderer::InsertBatch(renderer::detail::render_batches::iterator where, const RenderPrimitive &primitive)
{
	auto offset =
		where == std::begin(batches_) ? 0 :
		(*(where - 1))->offset + (*(where - 1))->capacity;

	where = batches_.insert(where,
		make_owning<renderer::detail::render_batch>(
			primitive.WorldZ(), offset, 0, primitive.MakeVertexBatch())
		);

	auto vertex_data_size = primitive.VertexDataSize();
	auto initial_size = std::max(vertex_data_size, initial_batch_data_size_);
	GrowBatch(where, initial_size);
	return *where;
}

void Renderer::GrowBatch(renderer::detail::render_batches::iterator where, int size)
{
	//Reallocate if not enough capacity
	if (std::ssize(vertex_data_) < used_capacity_ + size)
	{
		//Grow with minimum required size
		auto grow_size = used_capacity_ + size - std::ssize(vertex_data_);
		vertex_data_.insert(std::end(vertex_data_), grow_size, 0.0_r); //Reallocates!

		//Fill up allocated space (using vectors growth strategy)
		if (std::size(vertex_data_) < vertex_data_.capacity())
		{
			grow_size = static_cast<int>(vertex_data_.capacity() - std::size(vertex_data_));
			vertex_data_.insert(std::end(vertex_data_), grow_size, 0.0_r); //Does not reallocate
		}

		need_update_ = true; //Vertex data has reallocated
	}

	//Batch is not last, copy data to the right
	if (where + 1 != std::end(batches_))
	{
		std::copy_backward(std::begin(vertex_data_) + (*where)->offset,
			std::begin(vertex_data_) + used_capacity_,
			std::begin(vertex_data_) + used_capacity_ + size);

		//Update offset on all succeeding batches
		for (auto iter = where + 1, end = std::end(batches_); iter != end; ++iter)
			(*iter)->offset += size;
	}

	used_capacity_ += size;
	(*where)->capacity += size;
	(*where)->need_update = true;
}

detail::groupable_status Renderer::IsGroupable(const RenderPrimitive &primitive, const renderer::detail::render_batch &batch) const noexcept
{
	if (auto iter =
		std::find_if(std::begin(batch.slots), std::end(batch.slots),
			[&](auto &slot) noexcept
			{
				return !!slot.primitive;
			}); iter != std::end(batch.slots))
		//Batch not empty, check if groupable with first primitive
		return primitive.IsGroupable(*iter->primitive) ?
			detail::groupable_status::Yes : detail::groupable_status::No;
	else //Batch empty
		return detail::groupable_status::YesEmpty;
}


void Renderer::RefreshPrimitives()
{
	//Refresh all primitives in batches
	for (auto &batch : batches_)
	{
		for (auto &slot : batch->slots)
		{
			if (slot.primitive)
				slot.primitive->Refresh();
		}
	}

	//Refresh all primitives just added
	for (auto &primitive : added_primitives_)
		primitive->Refresh();

	//Refresh all hidden primitives
	for (auto &primitive : hidden_primitives_)
		primitive->Refresh();
}

void Renderer::GroupAddedPrimitives()
{
	//Sort added primitives before grouping
	std::stable_sort(std::begin(added_primitives_), std::end(added_primitives_),
		[](auto &primitive, auto &primitive2) noexcept
		{
			return primitive->WorldZ() < primitive2->WorldZ(); //Desc
		});

	for (auto &primitive : added_primitives_)
	{
		auto z = primitive->WorldZ();
		auto where_to_group = std::optional<detail::render_batches::iterator>{};
		auto where_to_insert = std::make_optional(std::begin(batches_));
		
		//Search from right to left
		for (auto iter = std::rbegin(batches_), end = std::rend(batches_); iter != end; ++iter)
		{
			if ((*iter)->z <= z)
			{
				auto status = detail::groupable_status::No;

				//Check if primitive can be grouped with batch
				if ((*iter)->z == z &&
					(status = IsGroupable(*primitive, **iter)) != detail::groupable_status::No)
				{
					if (status == detail::groupable_status::Yes)
					{
						GroupWithBatch(*primitive, **iter);
						where_to_group = {};
						where_to_insert = {};
						break;
					}
					else //YesEmpty
						where_to_group = iter.base() - 1; //Record empty batch (not optimal)
				}
				else if (*where_to_insert == std::begin(batches_))
					*where_to_insert = iter.base(); //Record batch insertion point

				if ((*iter)->z < z)
					break;
			}
		}
		
		//Can only be grouped with empty batch
		if (where_to_group)
		{
			(**where_to_group)->used_capacity = 0;
			(**where_to_group)->slots.clear();
			(**where_to_group)->vertex_batch = primitive->MakeVertexBatch();
			GroupWithBatch(*primitive, ***where_to_group);
		}
		//Could not be grouped with any existing batches
		else if (where_to_insert)
		{
			auto ptr = InsertBatch(*where_to_insert, *primitive);
			GroupWithBatch(*primitive, *ptr);
		}
	}

	added_primitives_.clear();
}

void Renderer::GroupWithBatch(RenderPrimitive &primitive, renderer::detail::render_batch &batch)
{
	auto vertex_data_size = primitive.VertexDataSize();
	auto min_space_left = std::optional<int>();
	auto where = std::make_optional(std::end(batch.slots));

	for (auto iter = std::begin(batch.slots), end = *where; iter != end; ++iter)
	{
		//Check if slot is occupied
		if (iter->primitive)
			continue;

		if (auto space_left = iter->capacity - vertex_data_size;
			space_left >= 0) //Has space to store primitive
		{
			if (space_left < min_space_left.value_or(space_left + 1))
			{
				min_space_left = space_left;
				*where = iter; //Record slot that fits the primitive best
			}

			//No space left, exact fit
			if (space_left == 0)
				break;
		}
	}

	//Add primitive to slot (has space)
	if (where != std::end(batch.slots))
	{
		(*where)->primitive = &primitive;
		(*where)->need_update = true;

		//Has space left
		if (*min_space_left > 0)
		{
			(*where)->capacity -= *min_space_left;

			//Add empty slot with the remaining space
			batch.slots.emplace(*where + 1, *min_space_left, nullptr);
		}
	}
	else //Could not be added to any existing slots (no slots with space available)
	{
		batch.slots.emplace_back(vertex_data_size, &primitive);
		batch.used_capacity += batch.slots.back().capacity;
	}
}

void Renderer::CompressBatches() noexcept
{
	for (auto &batch : batches_)
	{
		//Find first empty slot
		auto iter =
			std::find_if(std::begin(batch->slots), std::end(batch->slots),
				[&](auto &slot) noexcept
				{
					return !slot.primitive;
				});

		if (iter == std::end(batch->slots))
			continue; //Nothing to compress

		//Find offset of first empty slot
		auto slot_offset = 0;
		for (auto it = std::begin(batch->slots); it != iter; ++it)
			slot_offset += it->capacity;

		//Erase all empty slots
		batch->slots.erase(
			std::remove_if(iter, std::end(batch->slots),
				[&](auto &slot) noexcept
				{
					auto empty_slot = !slot.primitive;
					if (empty_slot)
					{
						//Empty slot, copy data to the left
						std::copy(std::begin(vertex_data_) + slot_offset + slot.capacity,
							std::begin(vertex_data_) + batch->offset + batch->used_capacity,
							std::begin(vertex_data_) + slot_offset);

						batch->used_capacity -= slot.capacity;
					}

					slot_offset += slot.capacity;
					return empty_slot;
				}), std::end(batch->slots));
	}
}

void Renderer::UpdateBatches()
{
	for (auto iter = std::begin(batches_), end = std::end(batches_); iter != end; ++iter)
	{
		//Grow batch if not enough capacity
		if ((*iter)->capacity < (*iter)->used_capacity)
			GrowBatch(iter, (*iter)->used_capacity - (*iter)->capacity);
	}
}

void Renderer::PrepareVertexData()
{
	auto out_of_date = false;
	for (auto &batch : batches_)
	{
		auto data_to_upload = std::optional<std::pair<int, int>>{};
			//A minimal data chunk [offset, size] that must be uploaded to VRAM

		//Prepare all primitives
		for (auto slot_offset = batch->offset; auto &slot : batch->slots)
		{
			slot.need_update |= slot.primitive->Prepare();

			//Copy data from primitive to vertex data
			if (slot.need_update)
			{
				auto &vertex_data = slot.primitive->WorldVertexData();
				std::copy(std::begin(vertex_data), std::end(vertex_data), std::begin(vertex_data_) + slot_offset);
				slot.need_update = false;

				if (!data_to_upload)
					data_to_upload = std::pair{slot_offset, 0}; //Upload offset

				data_to_upload->second = //Upload size
					slot_offset + slot.capacity - data_to_upload->first;
			}

			slot_offset += slot.capacity;
		}


		if (batch->need_update)
			out_of_date = true; //This and all succeeding batches
		
		if (out_of_date || batch->used_capacity != batch->last_used_capacity)
		{
			//Vertex data has not reallocated
			if (!need_update_)
			{
				//Has vertex buffer object
				if (vbo_ && *vbo_ && out_of_date)
					batch->vertex_batch.VertexBuffer( //Update vertex buffer
						vbo_->SubBuffer(batch->offset * sizeof(real), batch->capacity * sizeof(real))
					);

				//Update vertex data
				batch->vertex_batch.VertexData(
					{std::data(vertex_data_) + batch->offset, batch->used_capacity}
				);
			}

			batch->need_update = false;
		}
		else if (data_to_upload)
		{
			//Vertex data has not reallocated
			if (!need_update_)
			{
				//Has vertex buffer object
				if (vbo_ && *vbo_)
					vbo_->Data( //Upload minimal data chunk from RAM to VRAM
						{std::data(vertex_data_) + data_to_upload->first, data_to_upload->second},
						data_to_upload->first * sizeof(real)
					);
				else //Update vertex data
					batch->vertex_batch.VertexData(
						{std::data(vertex_data_) + batch->offset, batch->used_capacity}
					);
			}
		}

		batch->last_used_capacity = batch->used_capacity;
	}

	//Vertex data has reallocated
	if (need_update_)
	{
		if (!vbo_)
			vbo_.emplace(render::vertex::vertex_buffer_object::VertexBufferUsage::Dynamic);
		
		//Has vertex buffer object
		if (vbo_ && *vbo_)
		{
			vbo_->Data(vertex_data_); //Upload data from RAM to VRAM

			//Update all vertex buffers
			for (auto &batch : batches_)
				batch->vertex_batch.VertexBuffer(
					vbo_->SubBuffer(batch->offset * sizeof(real), batch->capacity * sizeof(real)),
					false //Data already uploaded
				);
		}

		//Update all vertex data
		for (auto &batch : batches_)
			batch->vertex_batch.VertexData(
				{std::data(vertex_data_) + batch->offset, batch->used_capacity},
				false //Data already uploaded
			);

		need_update_ = false;
	}
}


//Public

Renderer::~Renderer() noexcept
{
	ClearBatches();
}


/*
	Batches
*/

//
void Renderer::ClearBatches() noexcept
{
	ClearPrimitives();
	batches_.clear();
	batches_.shrink_to_fit();
}


/*
	Primitives
*/

bool Renderer::AddPrimitive(RenderPrimitive &primitive)
{
	if (!primitive.ParentRenderer())
	{
		primitive.ParentRenderer(this);

		//Primitive is visible
		if (primitive.WorldVisible() && primitive.VertexCount() > 0)
			added_primitives_.push_back(&primitive);
		else
			hidden_primitives_.push_back(&primitive);

		return true;
	}
	else
		return false;
}

bool Renderer::RefreshPrimitive(RenderPrimitive &primitive)
{
	if (primitive.ParentRenderer() != this)
		return false;

	auto iter_hidden = std::find(std::begin(hidden_primitives_), std::end(hidden_primitives_), &primitive);
	auto iter_added = iter_hidden == std::end(hidden_primitives_) ?
		std::find(std::begin(added_primitives_), std::end(added_primitives_), &primitive) :
		std::end(added_primitives_);

	//Primitive is visible
	if (primitive.WorldVisible() && primitive.VertexCount() > 0)
	{
		//Primitive has just been added, no-op
		if (iter_added != std::end(added_primitives_))
			return true;

		//Move from hidden to added, fast
		else if (iter_hidden != std::end(hidden_primitives_))
		{
			hidden_primitives_.erase(iter_hidden);
			added_primitives_.push_back(&primitive);
			return true;
		}	
	}
	else //Primitive is not visible
	{
		//Primitive is already hidden, no-op
		if (iter_hidden != std::end(hidden_primitives_))
			return true;

		//Move from added to hidden, fast
		else if (iter_added != std::end(added_primitives_))
		{
			added_primitives_.erase(iter_added);
			hidden_primitives_.push_back(&primitive);
			return true;
		}
	}
	
	//Full refresh, slow
	RemovePrimitive(primitive);
	AddPrimitive(primitive);
	return true;
}


bool Renderer::RemovePrimitive(RenderPrimitive &primitive) noexcept
{
	if (primitive.ParentRenderer() != this)
		return false;

	//Check if primitive is in a batch
	for (auto &batch : batches_)
	{
		for (auto &slot : batch->slots)
		{
			if (slot.primitive == &primitive)
			{
				slot.primitive->ParentRenderer(nullptr);
				slot.primitive = nullptr;
				return true;
			}
		}
	}

	//Check if primitive has just been added
	if (auto iter = std::find(std::begin(added_primitives_), std::end(added_primitives_), &primitive);
		iter != std::end(added_primitives_))
	{
		(*iter)->ParentRenderer(nullptr);
		added_primitives_.erase(iter);
		return true;
	}

	//Check if primitive is hidden
	if (auto iter = std::find(std::begin(hidden_primitives_), std::end(hidden_primitives_), &primitive);
		iter != std::end(hidden_primitives_))
	{
		(*iter)->ParentRenderer(nullptr);
		hidden_primitives_.erase(iter);
		return true;
	}
	
	return false;
}

void Renderer::ClearPrimitives() noexcept
{
	//Remove all primitives from batches
	for (auto &batch : batches_)
	{
		for (auto &slot : batch->slots)
		{
			if (slot.primitive)
			{
				slot.primitive->ParentRenderer(nullptr);
				slot.primitive = nullptr;
			}
		}

		batch->slots.clear();
		batch->slots.shrink_to_fit();
		batch->used_capacity = 0;
	}

	//Remove all primitives just added
	for (auto &primitive : added_primitives_)
	{
		if (primitive)
		{
			primitive->ParentRenderer(nullptr);
			primitive = nullptr;
		}
	}

	added_primitives_.clear();
	added_primitives_.shrink_to_fit();

	//Remove all hidden primitives
	for (auto &primitive : hidden_primitives_)
	{
		if (primitive)
		{
			primitive->ParentRenderer(nullptr);
			primitive = nullptr;
		}
	}

	hidden_primitives_.clear();
	hidden_primitives_.shrink_to_fit();
}


/*
	Preparing / drawing
*/

void Renderer::Prepare()

	//Set initial data size
	if (vertex_data_.capacity() == 0 &&
		initial_vertex_data_size_ > 0)
	{
		vertex_data_.reserve(initial_vertex_data_size_);
		vertex_data_.insert(std::end(vertex_data_), initial_vertex_data_size_, 0.0_r);
	}

	RefreshPrimitives();
	GroupAddedPrimitives();
	CompressBatches();
	UpdateBatches();
	PrepareVertexData();

	//Prepare all batches
	for (auto &batch : batches_)
	{
		//Has one or more primitives
		if (batch->used_capacity > 0)
			batch->vertex_batch.Prepare();
	}
}

void Renderer::Draw() noexcept
{
	static NonOwningPtr<shaders::ShaderProgram> active_shader_program = nullptr;

	//Draw all batches
	for (auto &batch : batches_)
	{
		//Check if batch has vertex data to draw
		if (batch->used_capacity == 0)
			continue;

		auto primitive = batch->slots.front().primitive;
			//Use first primitive (could be any primitive)

		//For each pass
		for (auto &pass : primitive->RenderPasses())
		{
			auto active_program = active_shader_program.get();
			auto shader_program = pass.RenderProgram().get();	
			auto use_shader = shader_program && shader_program->Owner() && shader_program->Handle();

			//Switch shader program
			if (shader_program != active_program)
			{
				if (use_shader) //Custom pipeline
					shader_program->Owner()->ActivateShaderProgram(*shader_program);
				else if (active_program) //Fixed-function pipeline
					active_program->Owner()->DeactivateShaderProgram(*active_program);

				active_shader_program = pass.RenderProgram();
			}


			pass.Blend();

			if (primitive->PointSize() != 1.0_r)
				detail::set_point_size(primitive->PointSize());

			if (primitive->LineThickness() != 1.0_r)
				detail::set_line_width(primitive->LineThickness());

			if (primitive->WireFrame())
				detail::enable_wire_frame();

			if (primitive->PointSprite())
				detail::enable_point_sprite();


			for (auto iterations = pass.Iterations(); iterations > 0; --iterations)
				batch->vertex_batch.Draw(shader_program); //Draw batch


			if (primitive->PointSprite())
				detail::disable_point_sprite();

			if (primitive->WireFrame())
				detail::disable_wire_frame();

			if (primitive->LineThickness() != 1.0_r)
				detail::set_line_width(1.0_r);

			if (primitive->PointSize() != 1.0_r)
				detail::set_point_size(1.0_r);


			//Optimization
			//When not deactivating the active shader program below...
			//a program will always be active until switched with another program

			//if (use_shader)
			//	shader_program->Owner()->DeactivateShaderProgram(*shader_program);
		}
	}
}


/*
	Elapse time
*/

void Renderer::Elapse(duration time) noexcept
{
	//Elapse all batches with primitives
	for (auto &batch : batches_)
	{
		if (batch->used_capacity > 0)
			batch->vertex_batch.Elapse(time);
	}
}

} //ion::graphics::render