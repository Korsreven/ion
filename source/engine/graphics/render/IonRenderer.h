/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/render
File:	IonRenderer.h
-------------------------------------------
*/

#ifndef ION_RENDERER_H
#define ION_RENDERER_H

#include <optional>
#include <utility>
#include <vector>

#include "IonRenderPrimitive.h"
#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"
#include "types/IonTypes.h"
#include "vertex/IonVertexBatch.h"
#include "vertex/IonVertexBufferObject.h"

namespace ion::graphics::render
{
	using namespace types::type_literals;

	namespace renderer
	{
		namespace detail
		{
			constexpr auto default_vertex_size = 3 + 3 + 4 + 3; //Based on mesh vertex declaration
			constexpr auto default_primitive_size = default_vertex_size * 6; //Based on a sprite with two triangles
			constexpr auto default_batch_data_size = default_primitive_size * 32;
			constexpr auto default_data_size = default_batch_data_size * 32;

			using render_primitives = std::vector<RenderPrimitive*>;

			enum class groupable_status
			{
				Yes,
				YesEmpty,
				No
			};


			struct render_batch_slot
			{
				int capacity = 0;
				RenderPrimitive *primitive = nullptr;
				bool need_update = true;

				//Constructor
				render_batch_slot(int capacity, RenderPrimitive *primitive) noexcept;
			};

			using render_batch_slots = std::vector<render_batch_slot>;


			struct render_batch
			{
				real z = 0.0_r;
				int offset = 0;
				int capacity = 0;
				int used_capacity = 0;
				int last_used_capacity = 0;

				render_batch_slots slots;
				vertex::VertexBatch vertex_batch;
				bool need_update = true;

				//Constructor
				render_batch(real z, int offset, int capacity, vertex::VertexBatch vertex_batch) noexcept;
			};
			
			using render_batches = std::vector<OwningPtr<render_batch>>;


			/*
				Graphics API
			*/

			void set_point_size(real size) noexcept;
			void set_line_width(real width) noexcept;

			void enable_wire_frame() noexcept;
			void disable_wire_frame() noexcept;
			
			void enable_point_sprite() noexcept;
			void disable_point_sprite() noexcept;
		} //detail
	} //renderer


	//A class representing a renderer that groups render primitives into batches
	//All batches are sorted and compressed, before the vertex data is uploaded to VRAM and drawn
	class Renderer
	{
		private:

			int used_capacity_ = 0;
			int initial_data_size_ = renderer::detail::default_data_size;
			int initial_batch_data_size_ = renderer::detail::default_batch_data_size;

			renderer::detail::render_batches batches_;
			renderer::detail::render_primitives added_primitives_;
			renderer::detail::render_primitives hidden_primitives_;

			render_primitive::vertex_data vertex_data_; //Data (RAM)
			std::optional<vertex::VertexBufferObject> vbo_; //Stream buffer (VRAM)
			bool need_update_ = true;


			NonOwningPtr<renderer::detail::render_batch> InsertBatch(renderer::detail::render_batches::iterator where, const RenderPrimitive &primitive);
			void GrowBatch(renderer::detail::render_batches::iterator where, int size);
			renderer::detail::groupable_status IsGroupable(const RenderPrimitive &primitive, const renderer::detail::render_batch &batch) const noexcept;

			void RefreshPrimitives();
			void GroupAddedPrimitives();
			void GroupWithBatch(RenderPrimitive &primitive, renderer::detail::render_batch &batch);
			void CompressBatches() noexcept;
			void UpdateBatches();
			void PrepareVertexData();

		public:

			//Construct a new renderer with the default data sizes
			Renderer();

			//Construct a new renderer with the given data sizes
			Renderer(int initial_data_size, int initial_batch_data_size);

			//Destructor
			~Renderer() noexcept;


			/*
				Modifiers
			*/

			//Sets the initial data size to the given size
			inline void InitialDataSize(int size) noexcept
			{
				initial_data_size_ = size > 0 ? size : 0;
			}

			//Sets the initial batch data size to the given size
			inline void InitialBatchDataSize(int size) noexcept
			{
				initial_batch_data_size_ = size > 0 ? size : 0;
			}


			/*
				Observers
			*/

			//Returns the initial data size used by this renderer
			[[nodiscard]] inline auto InitialDataSize() const noexcept
			{
				return initial_data_size_;
			}

			//Returns the initial batch data size used by this renderer
			[[nodiscard]] inline auto InitialBatchDataSize() const noexcept
			{
				return initial_data_size_;
			}


			/*
				Batches
			*/

			//Clears all batches from this renderer
			void ClearBatches() noexcept;


			/*
				Primitives
			*/

			//Add a primitive to this renderer
			bool AddPrimitive(RenderPrimitive &primitive);

			//Refresh a primitive in this renderer
			bool RefreshPrimitive(RenderPrimitive &primitive);


			//Remove a primitive from this renderer
			bool RemovePrimitive(RenderPrimitive &primitive) noexcept;

			//Clear all primitives from this renderer
			void ClearPrimitives() noexcept;


			/*
				Preparing / drawing
			*/

			//Prepare this renderer such that it is ready to draw
			void Prepare();

			//Draw all primitives added to this renderer, in batches
			void Draw() noexcept;	


			/*
				Elapse time
			*/

			//Elapse the total time for each batch in this renderer by the given time in seconds
			//This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;
	};
} //ion::graphics::render

#endif