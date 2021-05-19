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
#include <variant>

#include "graphics/IonGraphicsAPI.h"
#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonCamera.h"
#include "graphics/scene/IonLight.h"
#include "graphics/scene/IonMovableObject.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/shaders/IonShaderProgram.h"
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
		position->Get<glsl::vec3>() = camera.ParentNode()->DerivedPosition();
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
		density->Get<float>() = fog->Density(); //Using 'real' could make this uniform double

	if (auto near_distance = shader_program.GetUniform(shaders::shader_layout::UniformName::Fog_Near); near_distance)
		near_distance->Get<float>() = fog->NearDistance(); //Using 'real' could make this uniform double

	if (auto far_distance = shader_program.GetUniform(shaders::shader_layout::UniformName::Fog_Far); far_distance)
		far_distance->Get<float>() = fog->FarDistance(); //Using 'real' could make this uniform double

	if (auto color = shader_program.GetUniform(shaders::shader_layout::UniformName::Fog_Color); color)
		color->Get<glsl::vec4>() = fog->Tint();
}

void set_light_uniforms(const light_container &lights, int light_count, const Camera &camera, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;
	using namespace ion::utilities;

	if (light_count == 0)
		return; //Nothing to set


	auto type = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Type);
	auto position = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Position);
	auto direction = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Direction);

	auto ambient = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Ambient);
	auto diffuse = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Diffuse);
	auto specular = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Specular);

	auto constant = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Constant);
	auto linear = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Linear);
	auto quadratic = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Quadratic);

	auto cutoff = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_Cutoff);
	auto outer_cutoff = shader_program.GetUniform(shaders::shader_layout::UniformName::Light_OuterCutoff);


	for (auto i = 0; i < light_count; ++i)
	{
		if (type)
			(*type)[i].Get<int>() = static_cast<int>(lights[i]->Type());

		if (position)
			(*position)[i].Get<glsl::vec3>() = (lights[i]->Position() + lights[i]->ParentNode()->DerivedPosition()) -
											   (camera.Position() + camera.ParentNode()->DerivedPosition()); //View adjusted

		if (direction)
			(*direction)[i].Get<glsl::vec3>() = lights[i]->Direction().Deviant(lights[i]->ParentNode()->DerivedRotation() -
												(camera.Rotation() + camera.ParentNode()->DerivedRotation())); //View adjusted
												

		if (ambient)
			(*ambient)[i].Get<glsl::vec4>() = lights[i]->AmbientColor();

		if (diffuse)
			(*diffuse)[i].Get<glsl::vec4>() = lights[i]->DiffuseColor();

		if (specular)
			(*specular)[i].Get<glsl::vec4>() = lights[i]->SpecularColor();


		auto [constant_attenuation, linear_attenuation, quadratic_attenuation] = lights[i]->Attenuation();

		if (constant)
			(*constant)[i].Get<float>() = constant_attenuation;

		if (linear)
			(*linear)[i].Get<float>() = linear_attenuation;

		if (quadratic)
			(*quadratic)[i].Get<float>() = quadratic_attenuation;


		auto [cutoff_angle, outer_cutoff_angle] = lights[i]->Cutoff();

		if (cutoff)
			(*cutoff)[i].Get<float>() = math::Cos(cutoff_angle);

		if (outer_cutoff)
			(*outer_cutoff)[i].Get<float>() = math::Cos(outer_cutoff_angle);
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
		model_view_projection->Get<glsl::mat4>() = (projection_mat * model_view_mat).TransposeCopy();
		#endif
}

void set_scene_uniforms(real gamma_value, Color ambient_color, int light_count, shaders::ShaderProgram &shader_program) noexcept
{
	using namespace shaders::variables;

	if (auto gamma = shader_program.GetUniform(shaders::shader_layout::UniformName::Scene_Gamma); gamma)
		gamma->Get<float>() = gamma_value; //Using 'real' could make this uniform double

	if (auto ambient = shader_program.GetUniform(shaders::shader_layout::UniformName::Scene_Ambient); ambient)
		ambient->Get<glsl::vec4>() = ambient_color;

	if (auto count = shader_program.GetUniform(shaders::shader_layout::UniformName::Scene_LightCount); count)
		count->Get<int>() = light_count;
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

/*
	Rendering
*/

void SceneGraph::Render(render::Viewport &viewport, duration time) noexcept
{
	/*
		Camera
	*/

	auto camera = viewport.ConnectedCamera();

	//The camera connected to the given viewport needs to be active (visible) in this scene graph
	if (camera && camera->ParentNode() &&
		&camera->ParentNode()->RootNode() == &root_node_ &&
		camera->Visible() && camera->ParentNode()->Visible())

		camera->CaptureScene(viewport);

	else
		return;
	
	const auto &projection_mat = camera->ViewFrustum().ProjectionMatrix();
	const auto &view_mat = camera->ViewMatrix();


	/*
		Lights
	*/
	
	auto active_light_count = 0;

	if (lighting_enabled_)
	{
		//For each visible light
		for (auto &light : root_node_.AttachedLights())
		{
			if (light->Visible() && light->ParentNode()->Visible())
			{
				active_lights_[active_light_count++] = light;

				if (active_light_count == detail::max_light_count)
					break;
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

		//The node render started/ended events can be called without any attached objects
		//The visibility of the node is also used as a flag to enable/disable event notifications
		if (node.Visible())
		{
			NotifyNodeRenderStarted(node);

			if (!std::empty(node.AttachedObjects()))
			{
				auto model_view_mat = view_mat * node.FullTransformation();
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

					//Update uniforms
					if (object->Visible())
					{
						//For each shader program
						for (auto &shader_program : object->RenderPrograms())
						{
							//There is probably <= 10 distinct shader programs per scene
							//So std::find with its linear complexity will be the fastest method to make sure each added element is unique
							if (std::find(std::begin(shader_programs_), std::end(shader_programs_), shader_program) == std::end(shader_programs_))
								//One time per program per scene
							{
								detail::set_camera_uniforms(*camera, *shader_program);
								detail::set_fog_uniforms(fog_, *shader_program);
								detail::set_light_uniforms(active_lights_, active_light_count, *camera, *shader_program);
								detail::set_matrix_uniforms(projection_mat, *shader_program);
								detail::set_scene_uniforms(gamma_, ambient_color_, active_light_count, *shader_program);
								shader_programs_.push_back(shader_program); //Only distinct
							}

							//There is probably <= 5 distinct shader programs per node
							//So std::find with its linear complexity will be the fastest method to make sure each added element is unique
							if (std::find(std::begin(shader_programs_node_), std::end(shader_programs_node_), shader_program) == std::end(shader_programs_node_))
								//One time per program per node
							{
								detail::set_matrix_uniforms(projection_mat, model_view_mat, *shader_program);
								shader_programs_node_.push_back(shader_program); //Only distinct
							}
						}
					}

					//Render object
					object->Elapse(time);
					object->Render();
				}
			}

			NotifyNodeRenderEnded(node);
		}
	}
}

} //ion::graphics::scene::graph