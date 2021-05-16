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

#include <variant>

#include "graphics/render/IonViewport.h"
#include "graphics/scene/IonCamera.h"
#include "graphics/scene/IonLight.h"
#include "graphics/scene/IonMovableObject.h"
#include "graphics/shaders/IonShaderProgram.h"

namespace ion::graphics::scene::graph
{

using namespace scene_graph;

namespace scene_graph::detail
{

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
	
	auto &projection_mat = camera->ViewFrustum().ProjectionMatrix();
	auto &view_mat = camera->ViewMatrix();
	

	/*
		Lights
	*/
	
	auto active_light_count = 0;

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


	/*
		Scene nodes
	*/

	//For each visible node
	for (auto &node : root_node_.OrderedSceneNodes())
	{
		//The node render started/ended events can be called without any attached objects
		//The visibility of the node is also used as a flag to enable/disable event notifications
		if (node.Visible())
		{
			NotifyNodeRenderStarted(node);

			//For each attached object
			for (auto &object : node.AttachedObjects())
			{
				std::visit(
					[=](auto &&object) noexcept
					{
						object->Elapse(time);
						object->Render();
					}, object);
			}

			NotifyNodeRenderEnded(node);
		}
	}
}

} //ion::graphics::scene::graph