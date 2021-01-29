/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	system
File:	IonGraphicsAPI.h
-------------------------------------------
*/

#ifndef ION_GRAPHICS_API_H
#define ION_GRAPHICS_API_H

#include <gl/glew.h> //OpenGL API
#define ION_GLEW

#ifdef ION_GLEW
	#ifdef _WIN32 //For both 32 and 64 bit targets
		#include <GL/wglew.h> //Windows GL extensions
		#define ION_WIN_GLEW
	#endif
#endif


namespace ion::graphics::gl
{
	enum class Extension
	{
		Core,	//Core feature
		ARB,	//Architecture Review Board approved extension
		EXT,	//Multivendor extension
		None	//None 
	};

	enum class Version
	{
		v1_0, v1_1, v1_2, v1_2_1, v1_3, v1_4, v1_5,
		v2_0, v2_1,
		v3_0, v3_1, v3_2, v3_3,
		v4_0, v4_1, v4_2, v4_3, v4_4, v4_5, v4_6
	};


	[[nodiscard]] inline auto HasSupportFor(const char *name) noexcept
	{
		#ifdef ION_GLEW
		return !!glewIsSupported(name);
		#else
		return false;
		#endif
	}

	[[nodiscard]] inline auto HasGL(Version version) noexcept
	{
		switch (version)
		{
			//GL v1.x
			case Version::v1_0:
			{
				static auto supported = HasSupportFor("GL_VERSION_1_0");
				return supported;
			}
		
			case Version::v1_1:
			{
				static auto supported = HasSupportFor("GL_VERSION_1_1");
				return supported;
			}

			case Version::v1_2:
			{
				static auto supported = HasSupportFor("GL_VERSION_1_2");
				return supported;
			}

			case Version::v1_2_1:
			{
				static auto supported = HasSupportFor("GL_VERSION_1_2_1");
				return supported;
			}

			case Version::v1_3:
			{
				static auto supported = HasSupportFor("GL_VERSION_1_3");
				return supported;
			}

			case Version::v1_4:
			{
				static auto supported = HasSupportFor("GL_VERSION_1_4");
				return supported;
			}

			case Version::v1_5:
			{
				static auto supported = HasSupportFor("GL_VERSION_1_5");
				return supported;
			}


			//GL v2.x
			case Version::v2_0:
			{
				static auto supported = HasSupportFor("GL_VERSION_2_0");
				return supported;
			}

			case Version::v2_1:
			{
				static auto supported = HasSupportFor("GL_VERSION_2_1");
				return supported;
			}


			//GL v3.x
			case Version::v3_0:
			{
				static auto supported = HasSupportFor("GL_VERSION_3_0");
				return supported;
			}

			case Version::v3_1:
			{
				static auto supported = HasSupportFor("GL_VERSION_3_1");
				return supported;
			}

			case Version::v3_2:
			{
				static auto supported = HasSupportFor("GL_VERSION_3_2");
				return supported;
			}

			case Version::v3_3:
			{
				static auto supported = HasSupportFor("GL_VERSION_3_3");
				return supported;
			}


			//GL v4.x
			case Version::v4_0:
			{
				static auto supported = HasSupportFor("GL_VERSION_4_0");
				return supported;
			}

			case Version::v4_1:
			{
				static auto supported = HasSupportFor("GL_VERSION_4_1");
				return supported;
			}

			case Version::v4_2:
			{
				static auto supported = HasSupportFor("GL_VERSION_4_2");
				return supported;
			}

			case Version::v4_3:
			{
				static auto supported = HasSupportFor("GL_VERSION_4_3");
				return supported;
			}

			case Version::v4_4:
			{
				static auto supported = HasSupportFor("GL_VERSION_4_4");
				return supported;
			}

			case Version::v4_5:
			{
				static auto supported = HasSupportFor("GL_VERSION_4_5");
				return supported;
			}

			case Version::v4_6:
			{
				static auto supported = HasSupportFor("GL_VERSION_4_6");
				return supported;
			}


			//Unknown version
			default:
			return false;
		}
	}


	[[nodiscard]] inline auto BlendFuncSeparate_Support() noexcept
	{
		if (HasGL(Version::v2_0))
			return Extension::Core;
		#ifdef ION_GLEW
		else if (GLEW_EXT_blend_func_separate)
			return Extension::EXT;
		#endif
		else
			return Extension::None;
	}

	[[nodiscard]] inline auto FrameBufferObject_Support() noexcept
	{
		if (HasGL(Version::v3_0))
			return Extension::Core;
		#ifdef ION_GLEW
		else if (GLEW_ARB_framebuffer_object)
			return Extension::ARB;
		else if (GLEW_EXT_framebuffer_object)
			return Extension::EXT;
		#endif
		else
			return Extension::None;
	}

	[[nodiscard]] inline auto MultiTexture_Support() noexcept
	{
		if (HasGL(Version::v2_0))
			return Extension::Core;
		#ifdef ION_GLEW
		else if (GLEW_ARB_multitexture)
			return Extension::ARB;
		#endif
		else
			return Extension::None;
	}

	[[nodiscard]] inline auto PointSprite_Support() noexcept
	{
		if (HasGL(Version::v1_5))
			return Extension::Core;
		#ifdef ION_GLEW
		else if (GLEW_ARB_point_sprite)
			return Extension::ARB;
		#endif
		else
			return Extension::None;
	}

	[[nodiscard]] inline auto Shader_Support() noexcept
	{
		if (HasGL(Version::v2_0))
			return Extension::Core;
		#ifdef ION_GLEW
		else if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
			return Extension::ARB;
		#endif
		else
			return Extension::None;
	}

	[[nodiscard]] inline auto TextureNonPowerOfTwo_Support() noexcept
	{
		if (HasGL(Version::v2_0))
			return Extension::Core;
		else if (GL_ARB_texture_non_power_of_two)
			return Extension::ARB;
		else
			return Extension::None;
	}

	[[nodiscard]] inline auto VertexArrayObject_Support() noexcept
	{
		if (HasGL(Version::v3_0))
			return Extension::Core;
		#ifdef ION_GLEW
		else if (GLEW_ARB_vertex_array_object)
			return Extension::ARB;
		#endif
		else
			return Extension::None;
	}

	[[nodiscard]] inline auto VertexBufferObject_Support() noexcept
	{
		if (HasGL(Version::v2_1))
			return Extension::Core;
		#ifdef ION_GLEW
		else if (GLEW_ARB_vertex_buffer_object)
			return Extension::ARB;
		#endif
		else
			return Extension::None;
	}
	

	[[nodiscard]] inline auto MaxFragmentUniformComponents() noexcept
	{
		auto max_uniform_components = 0;

		switch (Shader_Support())
		{
			case Extension::Core:
			glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &max_uniform_components);
			break;

			case Extension::ARB:
			glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB, &max_uniform_components);
			break;
		}

		return max_uniform_components;
	}

	[[nodiscard]] inline auto MaxVertexUniformComponents() noexcept
	{
		auto max_uniform_components = 0;

		switch (Shader_Support())
		{
			case Extension::Core:
			glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &max_uniform_components);
			break;

			case Extension::ARB:
			glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB, &max_uniform_components);
			break;
		}

		return max_uniform_components;
	}


	[[nodiscard]] inline auto MaxTextureSize() noexcept
	{
		auto max_texture_size = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
		return max_texture_size;
	}

	[[nodiscard]] inline auto MaxTextureUnits() noexcept
	{
		auto max_texture_units = 0;

		switch (MultiTexture_Support())
		{
			case Extension::Core:
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_texture_units);
			break;

			case Extension::ARB:
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB, &max_texture_units);
			break;
		}

		return max_texture_units;
	}
} //ion::graphics::api

#endif