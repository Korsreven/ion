/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders
File:	IonShaderProgramManager.h
-------------------------------------------
*/

#ifndef ION_SHADER_PROGRAM_MANAGER_H
#define ION_SHADER_PROGRAM_MANAGER_H

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

#include "IonShaderLayout.h"
#include "IonShaderProgram.h"
#include "memory/IonNonOwningPtr.h"
#include "resources/IonResourceManager.h"
#include "types/IonTypes.h"
#include "variables/IonShaderTypes.h"

namespace ion::graphics::shaders
{
	namespace shader_program_manager
	{
		enum class InfoLogLevel : bool
		{
			Warning,
			Error
		};

		namespace detail
		{
			using namespace variables;

			std::optional<int> load_shader_program(std::optional<int> vertex_shader_handle, std::optional<int> fragment_shader_handle,
				const std::filesystem::path &shader_source_path, std::optional<InfoLogLevel> log_level) noexcept;
			void unload_shader_program(int shader_program_handle) noexcept;

			std::optional<std::string> print_info_log(int shader_program_handle);

			int get_active_shader_program() noexcept;
			void use_shader_program(int program_handle) noexcept;


			/**
				@name Attribute
				@{
			*/

			std::optional<int> get_attribute_location(int shader_program_handle, const std::string &attribute_name) noexcept;

			void load_attribute_location(int shader_program_handle, AttributeVariable &attribute_variable) noexcept;
			void send_attribute_value(int shader_program_handle, AttributeVariable &attribute_variable) noexcept;


			class set_attribute_value
			{
				private:

					int location_ = -1;

				public:

					set_attribute_value(int location) noexcept;

					//Generic (internally converted to float)
					void set_vertex_pointer(int index, int size, bool normalized, int stride, const void *pointer, int32) const noexcept;
					void set_vertex_pointer(int index, int size, bool normalized, int stride, const void *pointer, uint32) const noexcept;
					void set_vertex_pointer(int index, int size, bool normalized, int stride, const void *pointer, float32) const noexcept;
					void set_vertex_pointer(int index, int size, bool normalized, int stride, const void *pointer, float64) const noexcept;

					//Integers (not converted)
					void set_vertex_ipointer(int index, int size, bool normalized, int stride, const void *pointer, int32) const noexcept;
					void set_vertex_ipointer(int index, int size, bool normalized, int stride, const void *pointer, uint32) const noexcept;

					//Doubles (not converted)
					void set_vertex_lpointer(int index, int size, bool normalized, int stride, const void *pointer) const noexcept;


					///@brief Scalars and vectors
					template <typename T>
					inline void operator()(const glsl::attribute<T> &value) const noexcept
					{
						if constexpr (std::is_same_v<typename glsl::attribute<T>::basic_type, int32>)
							set_vertex_ipointer(location_, value.Components(), value.Normalized(), value.Stride(), value.Pointer(), int32{});
						else if constexpr (std::is_same_v<typename glsl::attribute<T>::basic_type, uint32>)
							set_vertex_ipointer(location_, value.Components(), value.Normalized(), value.Stride(), value.Pointer(), uint32{});
						else if constexpr (std::is_same_v<typename glsl::attribute<T>::basic_type, float32>)
							set_vertex_pointer(location_, value.Components(), value.Normalized(), value.Stride(), value.Pointer(), real{});
						else if constexpr (std::is_same_v<typename glsl::attribute<T>::basic_type, float64>)
							set_vertex_lpointer(location_, value.Components(), value.Normalized(), value.Stride(), value.Pointer());
					}

					///@brief Matrices
					template <typename T, int N, int M>
					inline void operator()(const glsl::attribute<glsl::Mat<N, M, T>> &value) const noexcept
					{
						if constexpr (std::is_same_v<T, int32>)
						{
							set_vertex_ipointer(location_, M, value.Normalized(), value.Stride(), value.Pointer(), int32{});

							if constexpr (N >= 1)
								set_vertex_ipointer(location_ + 1, M, value.Normalized(), value.Stride(), value.Pointer(), int32{});
							if constexpr (N >= 2)
								set_vertex_ipointer(location_ + 2, M, value.Normalized(), value.Stride(), value.Pointer(), int32{});
							if constexpr (N >= 3)
								set_vertex_ipointer(location_ + 3, M, value.Normalized(), value.Stride(), value.Pointer(), int32{});
						}
						else if constexpr (std::is_same_v<T, uint32>)
						{
							set_vertex_ipointer(location_, M, value.Normalized(), value.Stride(), value.Pointer(), uint32{});

							if constexpr (N >= 1)
								set_vertex_ipointer(location_ + 1, M, value.Normalized(), value.Stride(), value.Pointer(), uint32{});
							if constexpr (N >= 2)
								set_vertex_ipointer(location_ + 2, M, value.Normalized(), value.Stride(), value.Pointer(), uint32{});
							if constexpr (N >= 3)
								set_vertex_ipointer(location_ + 3, M, value.Normalized(), value.Stride(), value.Pointer(), uint32{});
						}
						else if constexpr (std::is_same_v<T, float32>)
						{
							set_vertex_pointer(location_, M, value.Normalized(), value.Stride(), value.Pointer(), real{});

							if constexpr (N >= 1)
								set_vertex_pointer(location_ + 1, M, value.Normalized(), value.Stride(), value.Pointer(), real{});
							if constexpr (N >= 2)
								set_vertex_pointer(location_ + 2, M, value.Normalized(), value.Stride(), value.Pointer(), real{});
							if constexpr (N >= 3)
								set_vertex_pointer(location_ + 3, M, value.Normalized(), value.Stride(), value.Pointer(), real{});
						}
						else if constexpr (std::is_same_v<T, float64>)
						{
							set_vertex_lpointer(location_, M, value.Normalized(), value.Stride(), value.Pointer());

							if constexpr (N >= 1)
								set_vertex_lpointer(location_ + 1, M, value.Normalized(), value.Stride(), value.Pointer());
							if constexpr (N >= 2)
								set_vertex_lpointer(location_ + 2, M, value.Normalized(), value.Stride(), value.Pointer());
							if constexpr (N >= 3)
								set_vertex_lpointer(location_ + 3, M, value.Normalized(), value.Stride(), value.Pointer());
						}
					}
			};

			///@}

			/**
				@name Uniform
				@{
			*/

			std::optional<int> get_uniform_location(int shader_program_handle, const std::string &uniform_name) noexcept;

			void load_uniform_location(int shader_program_handle, UniformVariable &uniform_variable) noexcept;
			void send_uniform_value(int shader_program_handle, UniformVariable &uniform_variable) noexcept;

			class set_uniform_value
			{
				private:

					int location_ = -1;

				public:

					set_uniform_value(int location) noexcept;


					//Scalars

					void operator()(const glsl::uniform<bool> &value) const noexcept;
					void operator()(const glsl::uniform<int32> &value) const noexcept;
					void operator()(const glsl::uniform<uint32> &value) const noexcept;
					void operator()(const glsl::uniform<float32> &value) const noexcept;
					void operator()(const glsl::uniform<float64> &value) const noexcept;


					//Vectors
					//2-components

					void operator()(const glsl::uniform<glsl::bvec2> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::ivec2> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::uvec2> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::vec2> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::dvec2> &value) const noexcept;

					//3-components

					void operator()(const glsl::uniform<glsl::bvec3> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::ivec3> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::uvec3> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::vec3> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::dvec3> &value) const noexcept;

					//4-components

					void operator()(const glsl::uniform<glsl::bvec4> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::ivec4> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::uvec4> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::vec4> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::dvec4> &value) const noexcept;


					//Matrices

					void operator()(const glsl::uniform<glsl::mat2> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::dmat2> &value) const noexcept;

					void operator()(const glsl::uniform<glsl::mat2x3> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::dmat2x3> &value) const noexcept;

					void operator()(const glsl::uniform<glsl::mat2x4> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::dmat2x4> &value) const noexcept;


					void operator()(const glsl::uniform<glsl::mat3x2> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::dmat3x2> &value) const noexcept;

					void operator()(const glsl::uniform<glsl::mat3> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::dmat3> &value) const noexcept;

					void operator()(const glsl::uniform<glsl::mat3x4> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::dmat3x4> &value) const noexcept;


					void operator()(const glsl::uniform<glsl::mat4x2> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::dmat4x2> &value) const noexcept;

					void operator()(const glsl::uniform<glsl::mat4x3> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::dmat4x3> &value) const noexcept;

					void operator()(const glsl::uniform<glsl::mat4> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::dmat4> &value) const noexcept;


					//Samplers

					void operator()(const glsl::uniform<glsl::isampler1D> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::usampler1D> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::sampler1D> &value) const noexcept;

					void operator()(const glsl::uniform<glsl::isampler1DArray> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::usampler1DArray> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::sampler1DArray> &value) const noexcept;


					void operator()(const glsl::uniform<glsl::isampler2D> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::usampler2D> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::sampler2D> &value) const noexcept;

					void operator()(const glsl::uniform<glsl::isampler2DArray> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::usampler2DArray> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::sampler2DArray> &value) const noexcept;
			};

			///@}
		} //detail
	} //shader_manager


	///@brief A class that manages and stores shader programs and shader layouts
	class ShaderProgramManager final :
		public resources::ResourceManager<ShaderProgram, ShaderProgramManager>,
		public managed::ObjectManager<ShaderLayout, ShaderProgramManager>
	{
		private:

			using ShaderProgramBase = resources::ResourceManager<ShaderProgram, ShaderProgramManager>;
			using ShaderLayoutBase = managed::ObjectManager<ShaderLayout, ShaderProgramManager>;

			std::optional<shader_program_manager::InfoLogLevel> log_level_;


			static inline auto active_shader_program_handle_ = 0;
				//Only one shader program can be in use at any given time

			static void ActivateShaderProgram(int handle) noexcept;
			static void DeactivateShaderProgram(int handle) noexcept;
			static bool IsShaderProgramActive(int handle) noexcept;

		protected:

			/**
				@name Events
				@{
			*/

			bool LoadResource(ShaderProgram &shader) override;
			bool UnloadResource(ShaderProgram &shader) noexcept override;

			void Created(ShaderProgram &shader_program) noexcept override;
			bool Removable(ShaderLayout &shader_layout) noexcept override;

			///@}

		public:

			///@brief Default constructor
			ShaderProgramManager();

			///@brief Deleted copy constructor
			ShaderProgramManager(const ShaderProgramManager&) = delete;

			///@brief Default move constructor
			ShaderProgramManager(ShaderProgramManager&&) = default;

			///@brief Destructor
			~ShaderProgramManager() noexcept;


			/**
				@name Operators
				@{
			*/

			///@brief Deleted copy assignment
			ShaderProgramManager& operator=(const ShaderProgramManager&) = delete;

			///@brief Default move assignment
			ShaderProgramManager& operator=(ShaderProgramManager&&) = default;

			///@}

			/**
				@name Managers
				@{
			*/

			///@brief Returns a mutable reference to the shader layout manager of this shader program manager
			[[nodiscard]] inline auto& ShaderLayoutManager() noexcept
			{
				return static_cast<ShaderLayoutBase&>(*this);
			}

			///@brief Returns an immutable reference to the shader layout manager of this shader program manager
			[[nodiscard]] inline auto& ShaderLayoutManager() const noexcept
			{
				return static_cast<const ShaderLayoutBase&>(*this);
			}

			///@}

			/**
				@name Ranges
				@{
			*/

			///@brief Returns a mutable range of all shader programs in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ShaderPrograms() noexcept
			{
				return Resources();
			}

			///@brief Returns an immutable range of all shader programs in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ShaderPrograms() const noexcept
			{
				return Resources();
			}


			///@brief Returns a mutable range of all shader layouts in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ShaderLayouts() noexcept
			{
				return ShaderLayoutBase::Objects();
			}

			///@brief Returns an immutable range of all shader layouts in this manager
			///@details This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ShaderLayouts() const noexcept
			{
				return ShaderLayoutBase::Objects();
			}

			///@}

			/**
				@name Modifiers
				@{
			*/

			///@brief Sets info log level for the shader program manager
			///@details Pass nullopt or {} to turn off any shader info log
			inline void LogLevel(std::optional<shader_program_manager::InfoLogLevel> log_level) noexcept
			{
				log_level_ = log_level;
			}

			///@}

			/**
				@name Observers
				@{
			*/

			///@brief Returns info log level for the shader program manager
			///@details Returns nullopt if shader info log is turned off
			[[nodiscard]] inline auto LogLevel() const noexcept
			{
				return log_level_;
			}

			///@}

			/**
				@name Shader programs - Creating
				@{
			*/

			///@brief Creates a shader program with the given name and shader
			NonOwningPtr<ShaderProgram> CreateShaderProgram(std::string name, NonOwningPtr<Shader> shader);

			///@brief Creates a shader program with the given name, shader and a user defined shader layout
			NonOwningPtr<ShaderProgram> CreateShaderProgram(std::string name, NonOwningPtr<Shader> shader,
				NonOwningPtr<ShaderLayout> shader_layout);

			///@brief Creates a shader program with the given name, vertex and fragment shader
			NonOwningPtr<ShaderProgram> CreateShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader);

			///@brief Creates a shader program with the given name, vertex and fragment shader and a user defined shader layout
			NonOwningPtr<ShaderProgram> CreateShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader,
				NonOwningPtr<ShaderLayout> shader_layout);


			///@brief Creates a shader program by moving the given shader program
			NonOwningPtr<ShaderProgram> CreateShaderProgram(ShaderProgram &&shader_program);

			///@}

			/**
				@name Shader programs - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable shader program with the given name
			///@details Returns nullptr if shader program could not be found
			[[nodiscard]] NonOwningPtr<ShaderProgram> GetShaderProgram(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable shader program with the given name
			///@details Returns nullptr if shader program could not be found
			[[nodiscard]] NonOwningPtr<const ShaderProgram> GetShaderProgram(std::string_view name) const noexcept;

			///@}

			/**
				@name Shader programs - Removing
				@{
			*/

			///@brief Clears all removable shader programs from this manager
			void ClearShaderPrograms() noexcept;

			///@brief Removes a removable shader program from this manager
			bool RemoveShaderProgram(ShaderProgram &shader_program) noexcept;

			///@brief Removes a removable shader program with the given name from this manager
			bool RemoveShaderProgram(std::string_view name) noexcept;

			///@}

			/**
				@name Shader programs - Activate/deactivate
				@{
			*/

			///@brief Activates (use) the given shader progam (if not already active)
			void ActivateShaderProgram(const ShaderProgram &shader_program) noexcept;

			///@brief Deactivates the given shader progam (if active)
			void DeactivateShaderProgram(const ShaderProgram &shader_program) noexcept;

			///@brief Returns true if the given shader program is currently active (in use)
			[[nodiscard]] bool IsShaderProgramActive(const ShaderProgram &shader_program) const noexcept;

			///@}

			/**
				@name Shader variables - Load/send
				@{
			*/

			///@brief Loads all shader variable locations attached to the given shader program
			void LoadShaderVariableLocations(ShaderProgram &shader_program) noexcept;

			///@brief Loads all attribute variable locations attached to the given shader program
			void LoadAttributeLocations(ShaderProgram &shader_program) noexcept;

			///@brief Loads all uniform variable locations attached to the given shader program
			void LoadUniformLocations(ShaderProgram &shader_program) noexcept;


			///@brief Sends all shader variable values to the given shader program
			void SendShaderVariableValues(ShaderProgram &shader_program) noexcept;

			///@brief Sends all attribute variable values to the given shader program
			void SendAttributeValues(ShaderProgram &shader_program) noexcept;

			///@brief Sends all uniform variable values to the given shader program
			void SendUniformValues(ShaderProgram &shader_program) noexcept;

			///@brief Sends all uniform variable values inside the given struct to the shader program
			void SendUniformValues(ShaderStruct &shader_struct) noexcept;

			///@}

			/**
				@name Shader layouts - Creating
				@{
			*/

			///@brief Creates a shader layout with the given name
			NonOwningPtr<ShaderLayout> CreateShaderLayout(std::string name);

			///@brief Creates a shader layout with the given name, struct, attribute and uniform bindings
			NonOwningPtr<ShaderLayout> CreateShaderLayout(std::string name, shader_layout::StructBindings struct_bindings,
				shader_layout::AttributeBindings attribute_bindings, shader_layout::UniformBindings uniform_bindings);


			///@brief Creates a shader layout by copying the given shader layout
			NonOwningPtr<ShaderLayout> CreateShaderLayout(const ShaderLayout &shader_layout);

			///@brief Creates a shader layout by moving the given shader layout
			NonOwningPtr<ShaderLayout> CreateShaderLayout(ShaderLayout &&shader_layout);

			///@}

			/**
				@name Shader layouts - Retrieving
				@{
			*/

			///@brief Gets a pointer to a mutable shader layout with the given name
			///@details Returns nullptr if shader layout could not be found
			[[nodiscard]] NonOwningPtr<ShaderLayout> GetShaderLayout(std::string_view name) noexcept;

			///@brief Gets a pointer to an immutable shader layout with the given name
			///@details Returns nullptr if shader layout could not be found
			[[nodiscard]] NonOwningPtr<const ShaderLayout> GetShaderLayout(std::string_view name) const noexcept;

			///@}

			/**
				@name Shader layouts - Removing
				@{
			*/

			///@brief Clears all removable shader layouts from this manager
			void ClearShaderLayouts() noexcept;

			///@brief Removes a removable shader layout from this manager
			bool RemoveShaderLayout(ShaderLayout &shader_layout) noexcept;

			///@brief Removes a removable shader layout with the given name from this manager
			bool RemoveShaderLayout(std::string_view name) noexcept;

			///@}

			/**
				@name Outputting
				@{
			*/

			///@brief Prints the info log (if any) from the given shader program
			///@details Returns nullopt if shader program is not valid or loaded
			[[nodiscard]] std::optional<std::string> PrintInfoLog(const ShaderProgram &shader_program) const;

			///@}
	};
} //ion::graphics::shaders

#endif