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


			/*
				Attribute
			*/

			std::optional<int> get_attribute_location(int shader_program_handle, const std::string &attribute_name) noexcept;
			void update_attribute_value(int shader_program_handle, AttributeVariable &attribute_variable) noexcept;


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


					//Scalars and vectors
					template <typename T>
					inline void operator()(const glsl::attribute<T> &value) const noexcept
					{
						if constexpr (std::is_same_v<glsl::attribute<T>::basic_type, int32>)
							set_vertex_ipointer(location_, value.Components(), value.Normalized(), value.Stride(), value.VertexData(), int32{});
						else if constexpr (std::is_same_v<glsl::attribute<T>::basic_type, uint32>)
							set_vertex_ipointer(location_, value.Components(), value.Normalized(), value.Stride(), value.VertexData(), uint32{});
						else if constexpr (std::is_same_v<glsl::attribute<T>::basic_type, float32>)
							set_vertex_pointer(location_, value.Components(), value.Normalized(), value.Stride(), value.VertexData(), float32{});
						else if constexpr (std::is_same_v<glsl::attribute<T>::basic_type, float64>)
							set_vertex_lpointer(location_, value.Components(), value.Normalized(), value.Stride(), value.VertexData());
					}

					//Matrices
					template <typename T, int N, int M>
					inline void operator()(const glsl::attribute<glsl::Mat<N, M, T>> &value) const noexcept
					{
						if constexpr (std::is_same_v<T, int32>)
						{
							set_vertex_ipointer(location_, M, value.Normalized(), value.Stride(), value.VertexData(), int32{});

							if constexpr (N >= 1)
								set_vertex_ipointer(location_ + 1, M, value.Normalized(), value.Stride(), value.VertexData(), int32{});
							if constexpr (N >= 2)
								set_vertex_ipointer(location_ + 2, M, value.Normalized(), value.Stride(), value.VertexData(), int32{});
							if constexpr (N >= 3)
								set_vertex_ipointer(location_ + 3, M, value.Normalized(), value.Stride(), value.VertexData(), int32{});
						}
						else if constexpr (std::is_same_v<T, uint32>)
						{
							set_vertex_ipointer(location_, M, value.Normalized(), value.Stride(), value.VertexData(), uint32{});

							if constexpr (N >= 1)
								set_vertex_ipointer(location_ + 1, M, value.Normalized(), value.Stride(), value.VertexData(), uint32{});
							if constexpr (N >= 2)
								set_vertex_ipointer(location_ + 2, M, value.Normalized(), value.Stride(), value.VertexData(), uint32{});
							if constexpr (N >= 3)
								set_vertex_ipointer(location_ + 3, M, value.Normalized(), value.Stride(), value.VertexData(), uint32{});
						}
						else if constexpr (std::is_same_v<T, float32>)
						{
							set_vertex_pointer(location_, M, value.Normalized(), value.Stride(), value.VertexData(), float32{});

							if constexpr (N >= 1)
								set_vertex_pointer(location_ + 1, M, value.Normalized(), value.Stride(), value.VertexData(), float32{});
							if constexpr (N >= 2)
								set_vertex_pointer(location_ + 2, M, value.Normalized(), value.Stride(), value.VertexData(), float32{});
							if constexpr (N >= 3)
								set_vertex_pointer(location_ + 3, M, value.Normalized(), value.Stride(), value.VertexData(), float32{});
						}
						else if constexpr (std::is_same_v<T, float64>)
						{
							set_vertex_lpointer(location_, M, value.Normalized(), value.Stride(), value.VertexData());

							if constexpr (N >= 1)
								set_vertex_lpointer(location_ + 1, M, value.Normalized(), value.Stride(), value.VertexData());
							if constexpr (N >= 2)
								set_vertex_lpointer(location_ + 2, M, value.Normalized(), value.Stride(), value.VertexData());
							if constexpr (N >= 3)
								set_vertex_lpointer(location_ + 3, M, value.Normalized(), value.Stride(), value.VertexData());
						}
					}
			};

			
			/*
				Uniform
			*/

			std::optional<int> get_uniform_location(int shader_program_handle, const std::string &uniform_name) noexcept;
			void update_uniform_value(int shader_program_handle, UniformVariable &uniform_variable) noexcept;

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

					void operator()(const glsl::uniform<glsl::isampler2D> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::usampler2D> &value) const noexcept;
					void operator()(const glsl::uniform<glsl::sampler2D> &value) const noexcept;
			};
		} //detail
	} //shader_manager


	class ShaderProgramManager final :
		public resources::ResourceManager<ShaderProgram, ShaderProgramManager>
	{
		private:

			std::optional<shader_program_manager::InfoLogLevel> log_level_;

		protected:

			/*
				Events
			*/

			bool LoadResource(ShaderProgram &shader) noexcept override;
			bool UnloadResource(ShaderProgram &shader) noexcept override;

		public:

			//Default constructor
			ShaderProgramManager() = default;

			//Deleted copy constructor
			ShaderProgramManager(const ShaderProgramManager&) = delete;

			//Default move constructor
			ShaderProgramManager(ShaderProgramManager&&) = default;

			//Destructor
			~ShaderProgramManager() noexcept;


			/*
				Operators
			*/

			//Deleted copy assignment
			ShaderProgramManager& operator=(const ShaderProgramManager&) = delete;

			//Move assignment
			ShaderProgramManager& operator=(ShaderProgramManager&&) = default;


			/*
				Ranges
			*/

			//Returns a mutable range of all shader programs in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ShaderPrograms() noexcept
			{
				return Resources();
			}

			//Returns an immutable range of all shader programs in this manager
			//This can be used directly with a range-based for loop
			[[nodiscard]] inline auto ShaderPrograms() const noexcept
			{
				return Resources();
			}


			/*
				Modifiers
			*/

			//Sets info log level for the shader program manager
			//Pass nullopt or {} to turn off any shader info log
			inline void LogLevel(std::optional<shader_program_manager::InfoLogLevel> log_level) noexcept
			{
				log_level_ = log_level;
			}


			/*
				Observers
			*/

			//Returns info log level for the shader program manager
			//Returns nullopt if shader info log is turned off
			[[nodiscard]] inline auto LogLevel() const noexcept
			{
				return log_level_;
			}


			/*
				Shader programs
				Creating
			*/

			//Create a shader program with the given name and a shader
			NonOwningPtr<ShaderProgram> CreateShaderProgram(std::string name, NonOwningPtr<Shader> shader);

			//Create a shader program with the given name, a vertex and fragment shader
			NonOwningPtr<ShaderProgram> CreateShaderProgram(std::string name, NonOwningPtr<Shader> vertex_shader, NonOwningPtr<Shader> fragment_shader);


			//Create a shader program by moving the given shader program
			NonOwningPtr<ShaderProgram> CreateShaderProgram(ShaderProgram &&shader_program);


			/*
				Shader programs
				Retrieving
			*/

			//Gets a pointer to a mutable shader program with the given name
			//Returns nullptr if shader program could not be found
			[[nodiscard]] NonOwningPtr<ShaderProgram> GetShaderProgram(std::string_view name) noexcept;

			//Gets a pointer to an immutable shader program with the given name
			//Returns nullptr if shader program could not be found
			[[nodiscard]] NonOwningPtr<const ShaderProgram> GetShaderProgram(std::string_view name) const noexcept;


			/*
				Shader programs
				Removing
			*/

			//Clear all removable shader programs from this manager
			void ClearShaderPrograms() noexcept;

			//Remove a removable shader program from this manager
			bool RemoveShaderProgram(ShaderProgram &shader_program) noexcept;

			//Remove a removable shader program with the given name from this manager
			bool RemoveShaderProgram(std::string_view name) noexcept;


			/*
				Shader variables
				Updating
			*/

			//Update all shader variables attached to the given shader program
			void UpdateShaderVariables(ShaderProgram &shader_program) noexcept;

			//Update all attribute variables attached to the given shader program
			void UpdateAttributeVariables(ShaderProgram &shader_program) noexcept;

			//Update all uniform variables attached to the given shader program
			void UpdateUniformVariables(ShaderProgram &shader_program) noexcept;


			/*
				Outputting
			*/

			//Prints the info log (if any) from the given shader program
			//Returns nullopt if shader program is not valid or loaded
			[[nodiscard]] std::optional<std::string> PrintInfoLog(const ShaderProgram &shader_program) const;
	};
} //ion::graphics::shaders

#endif