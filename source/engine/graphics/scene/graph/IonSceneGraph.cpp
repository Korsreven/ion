/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/scene/graph
File:	IonSceneGraph.cpp
-------------------------------------------
*/

#include "IonSceneGraph.h"

#include <algorithm>
#include <utility>
#include <variant>

#include "graphics/IonGraphicsAPI.h"
#include "graphics/render/IonViewport.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/scene/IonCamera.h"
#include "graphics/scene/IonLight.h"
#include "graphics/scene/IonMovableObject.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/textures/IonTextureManager.h"
#include "utilities/IonMath.h"

namespace ion::graphics::scene::graph
{

using namespace scene_graph;

namespace scene_graph::detail
{

void set_camera_uniforms(const Camera &camera, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;

	if (auto position = shader_program.GetUniform(shaders::shader_layout::UniformName::Camera_Position); position)
		position->Get<glsl::vec3>() = camera.Position() + camera.ParentNode()->DerivedPosition();

	if (auto rotation = shader_program.GetUniform(shaders::shader_layout::UniformName::Camera_Rotation); rotation)
		rotation->Get<float>() = static_cast<float>(camera.Rotation() + camera.ParentNode()->DerivedRotation()); //Using 'real' could make this uniform double
}

void set_fog_uniforms(std::optional<render::Fog> fog, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;

	if (auto has_fog = shader_program.GetUniform(shaders::shader_layout::UniformName::Scene_HasFog); has_fog)
		has_fog->Get<bool>() = !!fog;

	if (!fog)
		return; //Nothing more to set


	if (auto mode = shader_program.GetUniform(shaders::shader_layout::UniformName::Fog_Mode); mode)
		mode->Get<int>() = static_cast<int>(fog->Mode());

	if (auto density = shader_program.GetUniform(shaders::shader_layout::UniformName::Fog_Density); density)
		density->Get<float>() = static_cast<float>(fog->Density()); //Using 'real' could make this uniform double

	if (auto near_distance = shader_program.GetUniform(shaders::shader_layout::UniformName::Fog_Near); near_distance)
		near_distance->Get<float>() = static_cast<float>(fog->NearDistance()); //Using 'real' could make this uniform double

	if (auto far_distance = shader_program.GetUniform(shaders::shader_layout::UniformName::Fog_Far); far_distance)
		far_distance->Get<float>() = static_cast<float>(fog->FarDistance()); //Using 'real' could make this uniform double

	if (auto color = shader_program.GetUniform(shaders::shader_layout::UniformName::Fog_Color); color)
		color->Get<glsl::vec4>() = fog->Tint();
}

void set_light_uniforms(const light_pointers &lights, std::optional<textures::texture::TextureHandle> &texture_handle,
	const Camera &camera, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;
	using namespace ion::utilities;

	if (auto count = shader_program.GetUniform(shaders::shader_layout::UniformName::Scene_LightCount); count)
		count->Get<int>() = std::ssize(lights);

	if (std::empty(lights))
		return; //Nothing to set


	if (auto scene_lights = shader_program.GetUniform(shaders::shader_layout::UniformName::Scene_Lights); scene_lights)
		texture_handle = light::detail::upload_light_data(texture_handle, lights, camera);
	else
	{
		auto type = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Type);
		auto position = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Position);
		auto direction = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Direction);
		auto radius = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Radius);

		auto ambient = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Ambient);
		auto diffuse = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Diffuse);
		auto specular = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Specular);

		auto constant = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Constant);
		auto linear = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Linear);
		auto quadratic = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Quadratic);

		auto cutoff = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Cutoff);
		auto outer_cutoff = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_OuterCutoff);


		for (auto i = 0; auto &light : lights)
		{
			if (i == max_light_count)
				break;


			if (type)
				(*type)[i].Get<int>() = static_cast<int>(light->Type());

			if (position)
				(*position)[i].Get<glsl::vec3>() =
					camera.ViewMatrix() * (light->Position() + light->ParentNode()->DerivedPosition()); //View adjusted

			if (direction)
				(*direction)[i].Get<glsl::vec3>() =
					light->Direction().Deviant(light->ParentNode()->DerivedRotation() -
					(camera.Rotation() + camera.ParentNode()->DerivedRotation())); //View adjusted

			if (radius)
			{
				auto [sx, sy] = light->ParentNode()->DerivedScaling().XY();
				(*radius)[i].Get<float>() = static_cast<float>(light->Radius() * std::max(sx, sy)); //Using 'real' could make this uniform double
			}


			if (ambient)
				(*ambient)[i].Get<glsl::vec4>() = light->AmbientColor();

			if (diffuse)
				(*diffuse)[i].Get<glsl::vec4>() = light->DiffuseColor();

			if (specular)
				(*specular)[i].Get<glsl::vec4>() = light->SpecularColor();


			auto [constant_attenuation, linear_attenuation, quadratic_attenuation] = light->Attenuation();

			if (constant)
				(*constant)[i].Get<float>() = static_cast<float>(constant_attenuation); //Using 'real' could make this uniform double

			if (linear)
				(*linear)[i].Get<float>() = static_cast<float>(linear_attenuation); //Using 'real' could make this uniform double

			if (quadratic)
				(*quadratic)[i].Get<float>() = static_cast<float>(quadratic_attenuation); //Using 'real' could make this uniform double


			auto [cutoff_angle, outer_cutoff_angle] = light->Cutoff();

			if (cutoff)
				(*cutoff)[i].Get<float>() = static_cast<float>(math::Cos(cutoff_angle)); //Using 'real' could make this uniform double

			if (outer_cutoff)
				(*outer_cutoff)[i].Get<float>() = static_cast<float>(math::Cos(outer_cutoff_angle)); //Using 'real' could make this uniform double


			++i; //Increase light index
		}
	}
}

void set_emissive_light_uniforms(const light_pointers &lights, std::optional<textures::texture::TextureHandle> &texture_handle,
	const Camera &camera, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;
	using namespace ion::utilities;
	
	if (auto count = shader_program.GetUniform(shaders::shader_layout::UniformName::Scene_EmissiveLightCount); count)
		count->Get<int>() = std::ssize(lights);

	if (std::empty(lights))
		return; //Nothing to set


	if (auto scene_lights = shader_program.GetUniform(shaders::shader_layout::UniformName::Scene_EmissiveLights); scene_lights)
		texture_handle = light::detail::upload_emissive_light_data(texture_handle, lights, camera);
	else
	{
		auto position = shader_program.GetUniform(shaders::shader_layout::UniformName::EmissiveLight_Position);
		auto radius = shader_program.GetUniform(shaders::shader_layout::UniformName::EmissiveLight_Radius);
		auto color = shader_program.GetUniform(shaders::shader_layout::UniformName::EmissiveLight_Color);


		for (auto i = 0; auto &light : lights)
		{
			if (i == max_light_count)
				break;


			if (position)
				(*position)[i].Get<glsl::vec3>() =
					camera.ViewMatrix() * (light->Position() + light->ParentNode()->DerivedPosition()); //View adjusted

			if (radius)
			{
				auto [sx, sy] = light->ParentNode()->DerivedScaling().XY();
				(*radius)[i].Get<float>() = static_cast<float>(light->Radius() * std::max(sx, sy)); //Using 'real' could make this uniform double
			}

			if (color)
				(*color)[i].Get<glsl::vec4>() = light->DiffuseColor();


			++i; //Increase light index
		}
	}
}

void set_matrix_uniforms(const Matrix4 &projection_mat, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;

	if (auto projection = shader_program.GetUniform(shaders::shader_layout::UniformName::Matrix_Projection); projection)
		#ifdef ION_ROW_MAJOR
		projection->Get<glsl::mat4>() = projection_mat;
		#else
		projection->Get<glsl::mat4>() = projection_mat.TransposeCopy();
		#endif
}

void set_matrix_uniforms(const Matrix4 &projection_mat, const Matrix4 &model_view_mat, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;

	if (auto model_view = shader_program.GetUniform(shaders::shader_layout::UniformName::Matrix_ModelView); model_view)
		#ifdef ION_ROW_MAJOR
		model_view->Get<glsl::mat4>() = model_view_mat;
		#else
		model_view->Get<glsl::mat4>() = model_view_mat.TransposeCopy();
		#endif

	if (auto model_view_projection = shader_program.GetUniform(shaders::shader_layout::UniformName::Matrix_ModelViewProjection); model_view_projection)
		#ifdef ION_ROW_MAJOR
		model_view_projection->Get<glsl::mat4>() = projection_mat * model_view_mat;
		#else
		model_view_projection->Get<glsl::mat4>() = (projection_mat * model_view_mat).Transpose();
		#endif
	
	if (auto normal = shader_program.GetUniform(shaders::shader_layout::UniformName::Matrix_Normal); normal)
		#ifdef ION_ROW_MAJOR
		normal->Get<glsl::mat3>() = Matrix3{model_view_mat}.Inverse().Transpose();
		#else
		normal->Get<glsl::mat3>() = Matrix3{model_view_mat.TransposeCopy()}.Inverse().Transpose();
		#endif
}

void set_node_uniforms(const SceneNode &node, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;

	if (auto position = shader_program.GetUniform(shaders::shader_layout::UniformName::Node_Position); position)
		position->Get<glsl::vec3>() = node.DerivedPosition();

	if (auto direction = shader_program.GetUniform(shaders::shader_layout::UniformName::Node_Direction); direction)
		direction->Get<glsl::vec2>() = node.DerivedDirection();

	if (auto rotation = shader_program.GetUniform(shaders::shader_layout::UniformName::Node_Rotation); rotation)
		rotation->Get<float>() = static_cast<float>(node.DerivedRotation()); //Using 'real' could make this uniform double

	if (auto scaling = shader_program.GetUniform(shaders::shader_layout::UniformName::Node_Scaling); scaling)
		scaling->Get<glsl::vec2>() = node.DerivedScaling();
}

void set_scene_uniforms(real gamma_value, Color ambient_color, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;

	if (auto gamma = shader_program.GetUniform(shaders::shader_layout::UniformName::Scene_Gamma); gamma)
		gamma->Get<float>() = static_cast<float>(gamma_value); //Using 'real' could make this uniform double

	if (auto ambient = shader_program.GetUniform(shaders::shader_layout::UniformName::Scene_Ambient); ambient)
		ambient->Get<glsl::vec4>() = ambient_color;
}


void set_gl_model_view_matrix(const Matrix4 &model_view_mat) noexcept
{
	//Matrix should already be in model view mode
	//glMatrixMode(GL_MODELVIEW);

	//if constexpr (std::is_same_v<real, double>)...
	//would not work here, because all branches are compiled in a non-templated function
	#if defined(ION_DOUBLE_PRECISION) || defined(ION_EXTENDED_PRECISION)
		#ifdef ION_ROW_MAJOR
		glLoadMatrixd(model_view_mat.M()[0]);
		#else
		glLoadMatrixd(model_view_mat.TransposeCopy().M()[0]);
		#endif
	#else
		#ifdef ION_ROW_MAJOR
		glLoadMatrixf(model_view_mat.M()[0]);
		#else
		glLoadMatrixf(model_view_mat.TransposeCopy().M()[0]);
		#endif
	#endif
}

void mult_gl_model_view_matrix(const Matrix4 &model_view_mat) noexcept
{
	//Matrix should already be in model view mode
	//glMatrixMode(GL_MODELVIEW);

	//if constexpr (std::is_same_v<real, double>)...
	//would not work here, because all branches are compiled in a non-templated function
	#if defined(ION_DOUBLE_PRECISION) || defined(ION_EXTENDED_PRECISION)
		#ifdef ION_ROW_MAJOR
		glMultMatrixd(model_view_mat.M()[0]);
		#else
		glMultMatrixd(model_view_mat.TransposeCopy().M()[0]);
		#endif
	#else
		#ifdef ION_ROW_MAJOR
		glMultMatrixf(model_view_mat.M()[0]);
		#else
		glMultMatrixf(model_view_mat.TransposeCopy().M()[0]);
		#endif
	#endif
}

Matrix4 get_gl_model_view_matrix() noexcept
{
	Matrix4 model_view_mat;
	
	//Matrix should already be in model view mode
	//glMatrixMode(GL_MODELVIEW);

	//if constexpr (std::is_same_v<real, double>)...
	//would not work here, because all branches are compiled in a non-templated function
	#if defined(ION_DOUBLE_PRECISION) || defined(ION_EXTENDED_PRECISION)
	glGetDoublev(GL_MODELVIEW_MATRIX, const_cast<double*>(model_view_mat.M()[0]));
	#else
	glGetFloatv(GL_MODELVIEW_MATRIX, const_cast<float*>(model_view_mat.M()[0]));
	#endif

	#ifndef ION_ROW_MAJOR
	model_view_mat.Transpose();
	#endif

	return model_view_mat;
}

void push_gl_matrix() noexcept
{
	glPushMatrix();
}

void pop_gl_matrix() noexcept
{
	glPopMatrix();
}

} //scene_graph::detail


//Private

/*
	Notifying
*/

void SceneGraph::NotifyNodeRenderStarted(SceneNode &node) noexcept
{
	NodeEventsBase::NotifyAll(NodeEvents().Listeners(), &events::listeners::SceneNodeListener::NodeRenderStarted, std::ref(node));
}

void SceneGraph::NotifyNodeRenderEnded(SceneNode &node) noexcept
{
	NodeEventsBase::NotifyAll(NodeEvents().Listeners(), &events::listeners::SceneNodeListener::NodeRenderEnded, std::ref(node));
}


//Public

SceneGraph::~SceneGraph() noexcept
{
	if (light_texture_handle_)
		textures::texture_manager::detail::unload_texture(*light_texture_handle_);

	if (emissive_light_texture_handle_)
		textures::texture_manager::detail::unload_texture(*emissive_light_texture_handle_);
}


/*
	Rendering
*/

void SceneGraph::Render(render::Viewport &viewport, duration time) noexcept
{
	if (!enabled_)
		return;

	/*
		Camera
	*/

	auto camera = viewport.ConnectedCamera();

	//The camera connected to the given viewport needs to be active (visible) in this scene graph
	if (camera && camera->ParentNode() &&
		&camera->ParentNode()->RootNode() == &root_node_ &&
		camera->Visible() && camera->ParentNode()->Visible())
	{
		camera->CaptureScene(viewport);
		[[maybe_unused]] auto &aabb = camera->WorldAxisAlignedBoundingBox(true);
			//Cache camera bounding box
	}
	else
		return;
	
	const auto &projection_mat = camera->ViewFrustum().ProjectionMatrix();
	const auto &view_mat = camera->ViewMatrix();
	detail::set_gl_model_view_matrix(view_mat);


	/*
		Lights
	*/

	lights_.clear();
	emissive_lights_.clear();

	if (lighting_enabled_)
	{
		//For each visible light
		for (auto &light : root_node_.AttachedLights())
		{
			if (light->Visible() && light->ParentNode()->Visible())
				lights_.push_back(light);
		}

		//For each visible node
		for (auto &node : root_node_.OrderedSceneNodes())
		{
			if (node.Visible())
			{
				//For each attached object
				for (auto &attached_object : node.AttachedObjects())
				{
					auto object =
						std::visit(
							[=](auto &&object) noexcept -> MovableObject*
							{
								return object;
							}, attached_object);

					if (object->Visible())
					{
						//For each visible emissive light
						for (auto &light : object->EmissiveLights(false))
						{
							if (light->Visible())
								emissive_lights_.push_back(light);
						}
					}
				}
			}
		}
	}


	/*
		Scene nodes
	*/

	shader_programs_.clear();	

	//For each visible node
	for (auto &node : root_node_.OrderedSceneNodes())
	{
		shader_programs_node_.clear();
		node.Elapse(time);

		//The node render started/ended events can be called without any attached objects
		//The visibility of the node is also used as a flag to enable/disable event notifications
		if (node.Visible())
		{
			NotifyNodeRenderStarted(node);

			if (!std::empty(node.AttachedObjects()))
			{
				auto model_view_mat = view_mat * node.FullTransformation();
				detail::push_gl_matrix();
				detail::set_gl_model_view_matrix(model_view_mat);

				//For each attached object
				for (auto &attached_object : node.AttachedObjects())
				{
					auto object =
						std::visit(
							[=](auto &&object) noexcept -> MovableObject*
							{
								return object;
							}, attached_object);
					object->Elapse(time);

					//Render visible objects in view
					if (object->Visible() &&
						(object->WorldAxisAlignedBoundingBox(true, false).Empty() || //Cull based on actual geometry
						 object->WorldAxisAlignedBoundingBox(false).Intersects(camera->WorldAxisAlignedBoundingBox(false))))
					{
						//For each shader program update uniforms
						for (auto &shader_program : object->RenderPrograms())
						{
							//There is probably <= 10 distinct shader programs per scene
							//So std::find with its linear complexity will be the fastest method to make sure each added element is unique
							if (std::find(std::begin(shader_programs_), std::end(shader_programs_), shader_program) == std::end(shader_programs_))
								//One time per program per scene
							{
								detail::set_camera_uniforms(*camera, *shader_program);
								detail::set_fog_uniforms(fog_enabled_ ? fog_ : std::optional<render::Fog>{}, *shader_program);
								detail::set_light_uniforms(lights_, light_texture_handle_, *camera, *shader_program);
								detail::set_emissive_light_uniforms(emissive_lights_, emissive_light_texture_handle_, *camera, *shader_program);
								detail::set_matrix_uniforms(projection_mat, *shader_program);
								detail::set_scene_uniforms(gamma_, ambient_color_, *shader_program);
								shader_programs_.push_back(shader_program); //Only distinct
							}

							//There is probably <= 5 distinct shader programs per node
							//So std::find with its linear complexity will be the fastest method to make sure each added element is unique
							if (std::find(std::begin(shader_programs_node_), std::end(shader_programs_node_), shader_program) == std::end(shader_programs_node_))
								//One time per program per node
							{
								detail::set_matrix_uniforms(projection_mat, model_view_mat, *shader_program);
								detail::set_node_uniforms(node, *shader_program);
								shader_programs_node_.push_back(shader_program); //Only distinct
							}
						}

						object->Render();
					}
				}

				detail::pop_gl_matrix();
			}

			NotifyNodeRenderEnded(node);
		}
	}
}

} //ion::graphics::scene::graph