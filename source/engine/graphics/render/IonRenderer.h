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
			constexpr auto default_vertex_data_size = default_batch_data_size * 64;

			using render_primitives = std::vector<RenderPrimitive*>;

			enum class update_status
			{
				Yes,
				YesSuccessive,
				No
			};

			enum class groupable_status
			{
				Yes,
				YesEmpty,
				No
			};


			struct render_batch_slot final
			{
				int capacity = 0;
				RenderPrimitive *primitive = nullptr;
				bool need_update = true;

				///@brief Constructor
				render_batch_slot(int capacity, RenderPrimitive *primitive) noexcept;
			};

			using render_batch_slots = std::vector<render_batch_slot>;


			struct render_batch final
			{
				real z = 0.0_r;
				int offset = 0;
				int capacity = 0;
				int used_capacity = 0;
				int last_used_capacity = 0;

				render_batch_slots slots;
				vertex::VertexBatch vertex_batch;
				update_status need_update = update_status::YesSuccessive;

				///@brief Constructor
				render_batch(real z, int offset, int capacity, vertex::VertexBatch vertex_batch) noexcept;
			};
			
			using render_batches = std::vector<OwningPtr<render_batch>>;


			/**
				@name Graphics API
				@{
			*/

			void set_point_size(real size) noexcept;
			void set_line_width(real width) noexcept;

			void enable_wire_frame() noexcept;
			void disable_wire_frame() noexcept;
			
			void enable_point_sprite() noexcept;
			void disable_point_sprite() noexcept;

			///@}
		} //detail
	} //renderer


	///@brief A class representing a renderer that groups render primitives into batches
	///@details All batches are sorted and compressed, before the vertex data is uploaded to VRAM and drawn
	class Renderer final
	{
		private:

			int used_capacity_ = 0;
			int initial_vertex_data_size_ = renderer::detail::default_vertex_data_size;
			int initial_batch_data_size_ = renderer::detail::default_batch_data_size;

			renderer::detail::render_batches batches_;
			renderer::detail::render_primitives added_primitives_;
			renderer::detail::render_primitives hidden_primitives_;
			renderer::detail::render_primitives pending_primitives_;

			render_primitive::VertexContainer vertex_data_; //Data (RAM)
			std::optional<vertex::VertexBufferObject> vbo_; //Stream buffer (VRAM)
			bool need_update_ = true;


			NonOwningPtr<renderer::detail::render_batch> InsertBatch(renderer::detail::render_batches::iterator where, const RenderPrimitive &primitive);
			void GroupWithBatch(RenderPrimitive &primitive, renderer::detail::render_batch &batch);
			void GrowBatch(renderer::detail::render_batches::iterator where, int size);
			renderer::detail::groupable_status IsGroupable(const RenderPrimitive &primitive, const renderer::detail::render_batch &batch) const noexcept;

			void RefreshPrimitives();
			void GroupAddedPrimitives();
			void UpdateBatchSlots();
			void CompressBatches() noexcept;
			void UpdateBatches();
			void PrepareVertexData() noexcept;

		public:

			///@brief Default constructor
			Renderer() = default;

			///@brief Deleted copy constructor
			Renderer(const Renderer&) = delete;

			///@brief Default move constructor
			Renderer(Renderer&&) = default;

			///@brief Destructor
			~Renderer() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			Renderer& operator=(const Renderer&) = delete;

			///@brief Default move assignment
			Renderer& operator=(Renderer&&) = default;

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets the initial vertex data size to the given size
			inline void InitialVertexDataSize(int size) noexcept
			{
				initial_vertex_data_size_ = size > 0 ? size : 0;
			}

			///@brief Sets the initial batch data size to the given size
			inline void InitialBatchDataSize(int size) noexcept
			{
				initial_batch_data_size_ = size > 0 ? size : 0;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns the initial vertex data size used by this renderer
			[[nodiscard]] inline auto InitialVertexDataSize() const noexcept
			{
				return initial_vertex_data_size_;
			}

			///@brief Returns the initial batch data size used by this renderer
			[[nodiscard]] inline auto InitialBatchDataSize() const noexcept
			{
				return initial_batch_data_size_;
			}

			///@brief Returns the z range of all batches in this renderer
			[[nodiscard]] inline auto ZRange() const noexcept
				-> std::optional<std::pair<real, real>>
			{
				if (!std::empty(batches_))
					return std::pair{batches_.front()->z, batches_.back()->z};
				else
					return {};
			}


			///@brief Returns the total number of batches in this renderer
			[[nodiscard]] int TotalBatches() const noexcept;

			///@brief Returns the total number of primitives in this renderer
			[[nodiscard]] int TotalPrimitives() const noexcept;

			///@brief Returns the total number of batches to be drawn in this renderer
			[[nodiscard]] int TotalBatchesToDraw() const noexcept;

			///@brief Returns the total number of primitives to be drawn in this renderer
			[[nodiscard]] int TotalPrimitivesToDraw() const noexcept;

			///@}

			/**
				@name Batches
				@{
			*/

			///@brief Clears all batches from this renderer
			void ClearBatches() noexcept;

			///@}

			/**
				@name Primitives
				@{
			*/

			///@brief Adds a primitive to this renderer
			bool AddPrimitive(RenderPrimitive &primitive);

			///@brief Refreshes a primitive in this renderer
			bool RefreshPrimitive(RenderPrimitive &primitive);


			///@brief Removes a primitive from this renderer
			bool RemovePrimitive(RenderPrimitive &primitive) noexcept;

			///@brief Clears all primitives from this renderer
			void ClearPrimitives() noexcept;

			///@}

			/**
				@name Preparing/drawing
				@{
			*/

			///@brief Prepares this renderer such that it is ready to draw
			void Prepare();

			///@brief Draws all primitives added to this renderer, in batches
			void Draw() noexcept;	

			///@}

			/**
				@name Elapse time
				@{
			*/

			///@brief Elapses the total time for each batch in this renderer by the given time in seconds
			///@details This function is typically called each frame, with the time in seconds since last frame
			void Elapse(duration time) noexcept;

			///@}
	};
} //ion::graphics::render

#endif