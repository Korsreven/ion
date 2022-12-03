/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	engine
File:	main.cpp
-------------------------------------------
*/

#include "Ion.h"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ion::graphics::utilities::color::literals;
using namespace ion::graphics::utilities::vector2::literals;
using namespace ion::types::type_literals;
using namespace ion::utilities::file::literals;
using namespace ion::utilities::math::literals;


/*
	Initialize
	----------
	Set to true to init demo from script files
	Set to false to init demo programmatically
*/

constexpr auto init_from_script = false;

//--------------------------------------------


struct Game :
	ion::events::listeners::FrameListener,
	ion::events::listeners::WindowListener,
	ion::events::listeners::KeyListener,
	ion::events::listeners::MouseListener
{
	ion::NonOwningPtr<ion::graphics::scene::graph::SceneGraph> scene_graph;
	ion::NonOwningPtr<ion::graphics::render::Viewport> viewport;
	ion::gui::GuiController *gui_controller = nullptr;
	ion::sounds::SoundManager *sound_manager = nullptr;

	ion::NonOwningPtr<ion::sounds::Sound> night_runner;
	ion::NonOwningPtr<ion::graphics::scene::MovableSound> red_lamp_flicker;
	ion::NonOwningPtr<ion::graphics::scene::MovableSound> green_lamp_flicker;

	ion::NonOwningPtr<ion::graphics::scene::DrawableText> fps;
	ion::types::Cumulative<duration> fps_update_rate{1.0_sec};

	ion::NonOwningPtr<ion::graphics::scene::graph::SceneNode> intro_node;
	ion::NonOwningPtr<ion::graphics::scene::graph::SceneNode> level_node;
	ion::NonOwningPtr<ion::graphics::scene::graph::SceneNode> player_node;
	ion::NonOwningPtr<ion::graphics::scene::graph::SceneNode> light_node;
	ion::graphics::utilities::Vector2 move_model;
	bool rotate_model_left = false;
	bool rotate_model_right = false;

	ion::NonOwningPtr<ion::graphics::scene::Camera> camera;
	ion::NonOwningPtr<ion::graphics::scene::Camera> player_camera;
	ion::graphics::utilities::Vector2 move_camera;
	bool rotate_camera_left = false;
	bool rotate_camera_right = false;
	ion::types::Cumulative<duration> idle_time{2.0_sec};

	ion::NonOwningPtr<ion::graphics::scene::graph::animations::NodeAnimationTimeline> ship_idle_timeline;


	/*
		Frame listener
	*/

	bool FrameStarted(duration time) noexcept override
	{
		using namespace ion::utilities;
		using namespace ion::graphics::utilities;

		if (fps)
		{
			if (fps_update_rate += time)
				fps->Get()->Content(ion::utilities::convert::ToString(1.0_sec / time, 0));
		}

		//Level
		if (level_node && level_node->Visible())
		{
			if (ship_idle_timeline)
			{
				if (idle_time += time)
					ship_idle_timeline->Start();
			}

			if (player_node)
			{
				if (move_model != vector2::Zero ||
					rotate_model_left || rotate_model_right)
				{
					if (move_model != vector2::Zero)
						player_node->Translate(move_model.NormalizeCopy() * time.count());

					if (rotate_model_left)
						player_node->Rotate(math::ToRadians(180.0_r) * time.count());
					if (rotate_model_right)
						player_node->Rotate(math::ToRadians(-180.0_r) * time.count());

					if (ship_idle_timeline)
					{
						idle_time.Reset();
						ship_idle_timeline->Revert();
					}
				}	
			}

			if (camera && viewport &&
				viewport->ConnectedCamera() == camera)
			{
				if (move_camera != vector2::Zero)
					camera->ParentNode()->Translate(move_camera.NormalizeCopy() * time.count());

				if (rotate_camera_left)
					camera->ParentNode()->Rotate(math::ToRadians(180.0_r) * time.count());
				if (rotate_camera_right)
					camera->ParentNode()->Rotate(math::ToRadians(-180.0_r) * time.count());
			}
		
			if (player_camera && viewport &&
				viewport->ConnectedCamera() == player_camera)
			{
				if (rotate_camera_left)
					player_camera->ParentNode()->Rotate(math::ToRadians(180.0_r) * time.count());
				if (rotate_camera_right)
					player_camera->ParentNode()->Rotate(math::ToRadians(-180.0_r) * time.count());
			}
		}

		//GUI
		if (gui_controller && gui_controller->IsVisible())
			gui_controller->FrameStarted(time);

		return true;
	}

	bool FrameEnded(duration time) noexcept override
	{
		if (sound_manager)
			sound_manager->Update();

		//GUI
		if (gui_controller && gui_controller->IsVisible())
			gui_controller->FrameEnded(time);

		return true;
	}


	/*
		Window listener
	*/

	void WindowActionReceived(ion::events::listeners::WindowAction action) noexcept override
	{
		//GUI
		if (gui_controller && gui_controller->IsVisible())
			gui_controller->WindowActionReceived(action);
	}


	/*
		Key listener
	*/

	void KeyPressed(ion::events::listeners::KeyButton button) noexcept override
	{
		using namespace ion::graphics::utilities;

		//Level
		if (level_node && level_node->Visible() &&
			(!gui_controller || !gui_controller->IsVisible()))
		{
			switch (button)
			{
				case ion::events::listeners::KeyButton::W:
				move_model.Y(move_model.Y() + 1.0_r);
				break;

				case ion::events::listeners::KeyButton::A:
				move_model.X(move_model.X() - 1.0_r);
				break;

				case ion::events::listeners::KeyButton::S:
				move_model.Y(move_model.Y() - 1.0_r);
				break;

				case ion::events::listeners::KeyButton::D:
				move_model.X(move_model.X() + 1.0_r);
				break;

				case ion::events::listeners::KeyButton::Q:
				rotate_model_left = true;
				break;

				case ion::events::listeners::KeyButton::E:
				rotate_model_right = true;
				break;


				case ion::events::listeners::KeyButton::UpArrow:
				move_camera.Y(move_camera.Y() + 1.0_r);
				break;

				case ion::events::listeners::KeyButton::LeftArrow:
				move_camera.X(move_camera.X() - 1.0_r);
				break;

				case ion::events::listeners::KeyButton::DownArrow:
				move_camera.Y(move_camera.Y() - 1.0_r);
				break;

				case ion::events::listeners::KeyButton::RightArrow:
				move_camera.X(move_camera.X() + 1.0_r);
				break;

				case ion::events::listeners::KeyButton::Subtract:
				rotate_camera_left = true;
				break;

				case ion::events::listeners::KeyButton::Add:
				rotate_camera_right = true;
				break;
			}
		}

		//GUI
		if (gui_controller && gui_controller->IsVisible())
			gui_controller->KeyPressed(button);
	}

	void KeyReleased(ion::events::listeners::KeyButton button) noexcept override
	{
		using namespace ion::graphics::utilities;

		//Intro
		if (intro_node && intro_node->Visible())
		{
			//Press any key to continue
			intro_node->Visible(false);
			level_node->Visible(true);
			night_runner->Play()->Volume(0.2_r);
			red_lamp_flicker->Get()->Resume();
			green_lamp_flicker->Get()->Resume();
			return;
		}

		//Level
		if (level_node && level_node->Visible() &&
			(!gui_controller || !gui_controller->IsVisible()))
		{
			switch (button)
			{
				case ion::events::listeners::KeyButton::W:
				move_model.Y(move_model.Y() - 1.0_r);
				break;

				case ion::events::listeners::KeyButton::A:
				move_model.X(move_model.X() + 1.0_r);
				break;

				case ion::events::listeners::KeyButton::S:
				move_model.Y(move_model.Y() + 1.0_r);
				break;

				case ion::events::listeners::KeyButton::D:
				move_model.X(move_model.X() - 1.0_r);
				break;

				case ion::events::listeners::KeyButton::Q:
				rotate_model_left = false;
				break;

				case ion::events::listeners::KeyButton::E:
				rotate_model_right = false;
				break;


				case ion::events::listeners::KeyButton::UpArrow:
				move_camera.Y(move_camera.Y() - 1.0_r);
				break;

				case ion::events::listeners::KeyButton::LeftArrow:
				move_camera.X(move_camera.X() + 1.0_r);
				break;

				case ion::events::listeners::KeyButton::DownArrow:
				move_camera.Y(move_camera.Y() + 1.0_r);
				break;

				case ion::events::listeners::KeyButton::RightArrow:
				move_camera.X(move_camera.X() - 1.0_r);
				break;

				case ion::events::listeners::KeyButton::Subtract:
				rotate_camera_left = false;
				break;

				case ion::events::listeners::KeyButton::Add:
				rotate_camera_right = false;
				break;


				case ion::events::listeners::KeyButton::C:
				{
					if (viewport && camera && player_camera)
					{
						if (viewport->ConnectedCamera() == camera)
							viewport->ConnectedCamera(player_camera);
						else if (viewport->ConnectedCamera() == player_camera)
							viewport->ConnectedCamera(camera);
					}

					break;
				}

				case ion::events::listeners::KeyButton::F:
				{
					if (scene_graph)
						scene_graph->FogEnabled(!scene_graph->FogEnabled());
					break;
				}

				case ion::events::listeners::KeyButton::L:
				{
					if (light_node)
						light_node->Visible(!light_node->Visible());
					break;
				}

				case ion::events::listeners::KeyButton::Space:
				{
					//Intersection scene query
					ion::graphics::scene::query::IntersectionSceneQuery scene_query{scene_graph};
					scene_query.QueryMask(1 | 2 | 4);
					[[maybe_unused]] auto result = scene_query.Execute();

					//Ray scene query
					ion::graphics::scene::query::RaySceneQuery ray_scene_query{scene_graph,
						{player_node->DerivedPosition(), player_node->DerivedDirection()}};
					ray_scene_query.QueryMask(2 | 4);
					[[maybe_unused]] auto ray_result = ray_scene_query.Execute();

 					break;
				}
			}
		}

		//GUI
		if (gui_controller)
		{
			switch (button)
			{
				case ion::events::listeners::KeyButton::Escape:
				gui_controller->Visible(!gui_controller->IsVisible());
				break;
			}

			if (gui_controller->IsVisible())
				gui_controller->KeyReleased(button);
		}
  	}

	void CharacterPressed(char character) noexcept override
	{
		//GUI
		if (gui_controller && gui_controller->IsVisible())
			gui_controller->CharacterPressed(character);
	}


	/*
		Mouse listener
	*/

	void MousePressed(ion::events::listeners::MouseButton button, ion::graphics::utilities::Vector2 position) noexcept override
	{
		//GUI
		if (gui_controller && gui_controller->IsVisible())
			gui_controller->MousePressed(button, position);
	}

	void MouseReleased(ion::events::listeners::MouseButton button, ion::graphics::utilities::Vector2 position) noexcept override
	{
		//GUI
		if (gui_controller && gui_controller->IsVisible())
			gui_controller->MouseReleased(button, position);
	}

	void MouseMoved(ion::graphics::utilities::Vector2 position) noexcept override
	{
		//GUI
		if (gui_controller && gui_controller->IsVisible())
			gui_controller->MouseMoved(position);
	}

	void MouseWheelRolled(int delta, ion::graphics::utilities::Vector2 position) noexcept override
	{
		//GUI
		if (gui_controller && gui_controller->IsVisible())
			gui_controller->MouseWheelRolled(delta, position);
	}
};


#ifdef ION_WIN32
//Entry point for windows 32/64 bit
int WINAPI WinMain([[maybe_unused]] _In_ HINSTANCE instance,
				   [[maybe_unused]] _In_opt_ HINSTANCE prev_instance,
				   [[maybe_unused]] _In_ LPSTR cmd_line,
				   [[maybe_unused]] _In_ int cmd_show)
#else
//Entry point for non windows systems
int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
#endif
{
	ion::Engine engine;

	ion::Engine::PixelsPerUnit(360.0_r);
	ion::Engine::UnitsPerMeter(0.04_r);
	ion::Engine::ZLimit(-100.0_r, -1.0_r);

	auto &window = engine.RenderTo(
		ion::graphics::render::RenderWindow::Resizable("ION engine", {1280.0_r, 720.0_r}));
	window.MinSize(ion::graphics::utilities::Vector2{640.0_r, 360.0_r});
	window.Cursor(ion::graphics::render::render_window::WindowCursor::None);

	if (!engine.Initialize())
		return 1;

	engine.TargetFPS({});
	engine.VerticalSync(ion::engine::VSyncMode::Off);


	Game game;

	//Subscribe event listeners
	engine.FrameEvents().Subscribe(game);
	window.Events().Subscribe(game);

	if (auto input = engine.Input(); input)
	{
		input->KeyEvents().Subscribe(game);
		input->MouseEvents().Subscribe(game);
	}

	//Viewport
	auto viewport = engine.GetDefaultViewport();


	//Repositories
	ion::assets::repositories::AudioRepository audio_repository{ion::assets::repositories::file_repository::NamingConvention::FileName};
	ion::assets::repositories::FontRepository font_repository{ion::assets::repositories::file_repository::NamingConvention::FileName};
	ion::assets::repositories::ImageRepository image_repository{ion::assets::repositories::file_repository::NamingConvention::FileName};
	ion::assets::repositories::ScriptRepository script_repository{ion::assets::repositories::file_repository::NamingConvention::FileName};
	ion::assets::repositories::ShaderRepository shader_repository{ion::assets::repositories::file_repository::NamingConvention::FileName};
	ion::assets::repositories::VideoRepository video_repository{ion::assets::repositories::file_repository::NamingConvention::FileName};	

	ion::assets::AssetLoader asset_loader;
	asset_loader.Attach(audio_repository);
	asset_loader.Attach(font_repository);
	asset_loader.Attach(image_repository);
	asset_loader.Attach(script_repository);
	asset_loader.Attach(shader_repository);
	asset_loader.Attach(video_repository);

	asset_loader.LoadDirectory("bin", ion::utilities::file::DirectoryIteration::Recursive);
	//asset_loader.CompileDataFile("bin/resources.dat");


	//Managers
	auto textures = ion::make_owning<ion::graphics::textures::TextureManager>();
	textures->CreateRepository(std::move(image_repository));

	auto shaders = ion::make_owning<ion::graphics::shaders::ShaderManager>();
	shaders->CreateRepository(std::move(shader_repository));
	shaders->LogLevel(ion::graphics::shaders::shader_manager::InfoLogLevel::Error);	

	auto fonts = ion::make_owning<ion::graphics::fonts::FontManager>();
	fonts->CreateRepository(std::move(font_repository));

	auto sounds = ion::make_owning<ion::sounds::SoundManager>();
	sounds->CreateRepository(std::move(audio_repository));

	auto frame_sequences = ion::make_owning<ion::graphics::textures::FrameSequenceManager>();
	auto animations = ion::make_owning<ion::graphics::textures::AnimationManager>();
	auto materials = ion::make_owning<ion::graphics::materials::MaterialManager>();

	auto shader_programs = ion::make_owning<ion::graphics::shaders::ShaderProgramManager>();
	shader_programs->LogLevel(ion::graphics::shaders::shader_program_manager::InfoLogLevel::Error);

	auto type_faces = ion::make_owning<ion::graphics::fonts::TypeFaceManager>();
	auto texts = ion::make_owning<ion::graphics::fonts::TextManager>();
	auto particle_systems = ion::make_owning<ion::graphics::particles::ParticleSystemManager>();


	//Scene
	auto scene_graph = engine.CreateSceneGraph();
	scene_graph->Gamma(1.0_r);
	scene_graph->AmbientColor(ion::graphics::utilities::Color::RGB(50, 50, 50));
	scene_graph->FogEffect(ion::graphics::render::Fog::Linear(0.0_r, 2.25_r));
	scene_graph->FogEnabled(false);
	//scene_graph->LightingEnabled(false);
	auto scene_manager = scene_graph->CreateSceneManager();


	//GUI
	ion::gui::GuiController gui_controller{scene_graph->RootNode(),
		viewport, sounds->GetSoundChannelGroup("gui")};
	gui_controller.ZOrder(-2.0_r);
	auto gui_scene_manager = scene_graph->CreateSceneManager();
	

	//Initialize from script files
	if (init_from_script)
	{
		auto &script_managers = ion::script::interfaces::ScriptInterface::Managers();
		script_managers.Register(textures);
		script_managers.Register(shaders);
		script_managers.Register(fonts);
		script_managers.Register(sounds);
		script_managers.Register(frame_sequences);
		script_managers.Register(animations);
		script_managers.Register(materials);
		script_managers.Register(shader_programs);
		script_managers.Register(type_faces);
		script_managers.Register(texts);
		script_managers.Register(particle_systems);


		//Load scripts
		ion::script::interfaces::TextureScriptInterface texture_script;
		texture_script.CreateScriptRepository(script_repository);
		texture_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		texture_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		texture_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		texture_script.CreateTextures("textures.ion", *textures);
		textures->LoadAll(ion::resources::resource_manager::EvaluationStrategy::Eager);

		ion::script::interfaces::FontScriptInterface font_script;
		font_script.CreateScriptRepository(script_repository);
		font_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		font_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		font_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		font_script.CreateFonts("fonts.ion", *fonts);
		fonts->LoadAll(ion::resources::resource_manager::EvaluationStrategy::Eager);

		ion::script::interfaces::FrameSequenceScriptInterface frame_sequence_script;
		frame_sequence_script.CreateScriptRepository(script_repository);
		frame_sequence_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		frame_sequence_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		frame_sequence_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		frame_sequence_script.CreateFrameSequences("frame_sequences.ion", *frame_sequences);

		ion::script::interfaces::AnimationScriptInterface animation_script;
		animation_script.CreateScriptRepository(script_repository);
		animation_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		animation_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		animation_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		animation_script.CreateAnimations("animations.ion", *animations);

		ion::script::interfaces::ShaderScriptInterface shader_script;
		shader_script.CreateScriptRepository(script_repository);
		shader_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		shader_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		shader_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		shader_script.CreateShaders("shaders.ion", *shaders);
		shaders->LoadAll(ion::resources::resource_manager::EvaluationStrategy::Eager);

		ion::script::interfaces::ShaderProgramScriptInterface shader_program_script;
		shader_program_script.CreateScriptRepository(script_repository);
		shader_program_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		shader_program_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		shader_program_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		shader_program_script.CreateShaderPrograms("shader_programs.ion", *shader_programs);
		shader_programs->LoadAll(ion::resources::resource_manager::EvaluationStrategy::Eager);

		ion::script::interfaces::TypeFaceScriptInterface type_face_script;
		type_face_script.CreateScriptRepository(script_repository);
		type_face_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		type_face_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		type_face_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		type_face_script.CreateTypeFaces("type_faces.ion", *type_faces);

		ion::script::interfaces::SoundScriptInterface sound_script;
		sound_script.CreateScriptRepository(script_repository);
		sound_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		sound_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		sound_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		sound_script.CreateSounds("sounds.ion", *sounds);
		sounds->LoadAll(ion::resources::resource_manager::EvaluationStrategy::Eager);

		ion::script::interfaces::MaterialScriptInterface material_script;
		material_script.CreateScriptRepository(script_repository);
		material_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		material_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		material_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		material_script.CreateMaterials("materials.ion", *materials);

		ion::script::interfaces::ParticleSystemScriptInterface particle_system_script;
		particle_system_script.CreateScriptRepository(script_repository);
		particle_system_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		particle_system_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		particle_system_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		particle_system_script.CreateParticleSystems("particle_systems.ion", *particle_systems);

		ion::script::interfaces::TextScriptInterface text_script;
		text_script.CreateScriptRepository(script_repository);
		text_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		text_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		text_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		text_script.CreateTexts("texts.ion", *texts);
		
			
		//Default shader programs
		scene_manager->AddDefaultShaderProgram(ion::graphics::scene::query::scene_query::QueryType::Model,
			shader_programs->GetShaderProgram("model_prog"));
		scene_manager->AddDefaultShaderProgram(ion::graphics::scene::query::scene_query::QueryType::ParticleSystem,
			shader_programs->GetShaderProgram("particle_prog"));
		scene_manager->AddDefaultShaderProgram(ion::graphics::scene::query::scene_query::QueryType::Text,
			shader_programs->GetShaderProgram("flat_text_prog"));

		//Default shader programs (for GUI)
		gui_scene_manager->AddDefaultShaderProgram(ion::graphics::scene::query::scene_query::QueryType::Model,
			shader_programs->GetShaderProgram("flat_model_prog"));
		gui_scene_manager->AddDefaultShaderProgram(ion::graphics::scene::query::scene_query::QueryType::ParticleSystem,
			shader_programs->GetShaderProgram("flat_particle_prog"));
		gui_scene_manager->AddDefaultShaderProgram(ion::graphics::scene::query::scene_query::QueryType::Text,
			shader_programs->GetShaderProgram("flat_text_prog"));


		//Load scripts
		ion::script::interfaces::SceneScriptInterface scene_script;
		scene_script.CreateScriptRepository(script_repository);
		scene_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		scene_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		scene_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		scene_script.CreateScene("scene.ion", scene_graph->RootNode(), *scene_manager);
			

		//Load scripts
		ion::script::interfaces::GuiThemeScriptInterface gui_theme_script;
		gui_theme_script.CreateScriptRepository(script_repository);
		gui_theme_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		gui_theme_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		gui_theme_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		gui_theme_script.CreateGuiThemes("gui_themes.ion", gui_controller, gui_scene_manager);

		ion::script::interfaces::GuiScriptInterface gui_script;
		gui_script.CreateScriptRepository(script_repository);
		gui_script.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		gui_script.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		gui_script.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		gui_script.CreateGui("gui.ion", gui_controller, *gui_scene_manager);
	}
	else //Initialize programmatically
	{
		/*
			Textures
		*/

		//Aura
		auto aura_atlas = textures->CreateTextureAtlas("aura", "aura.png", 1, 2);
		auto aura_diffuse = textures->GetTexture("aura_1");
		auto aura_emissive = textures->GetTexture("aura_2");

		//Brick wall
		auto brick_wall_atlas = textures->CreateTextureAtlas("brick_wall", "brick_wall.jpg", 2, 2, 3);
		auto brick_wall_diffuse = textures->GetTexture("brick_wall_1");
		auto brick_wall_normal = textures->GetTexture("brick_wall_2");
		auto brick_wall_specular = textures->GetTexture("brick_wall_3");
		
		//Cloud
		auto cloud_diffuse = textures->CreateTexture("cloud_diffuse", "cloud.png");

		//Logo frames
		auto logo_frames_atlas = textures->CreateTextureAtlas("logo_frames", "logo_frames.png", 4, 4);
		auto logo_first_frame = textures->GetTexture("logo_frames_1");

		//Pyramid (egyptian)
		auto pyramid_egyptian_atlas = textures->CreateTextureAtlas("pyramid_egyptian", "pyramid_egyptian.png", 2, 2, 3);
		auto pyramid_egyptian_diffuse = textures->GetTexture("pyramid_egyptian_1");
		auto pyramid_egyptian_normal = textures->GetTexture("pyramid_egyptian_2");
		auto pyramid_egyptian_specular = textures->GetTexture("pyramid_egyptian_3");

		//Pyramid (mayan)
		auto pyramid_mayan_atlas = textures->CreateTextureAtlas("pyramid_mayan", "pyramid_mayan.png", 2, 2, 3);
		auto pyramid_mayan_diffuse = textures->GetTexture("pyramid_mayan_1");
		auto pyramid_mayan_normal = textures->GetTexture("pyramid_mayan_2");
		auto pyramid_mayan_specular = textures->GetTexture("pyramid_mayan_3");

		//Raindrop
		auto raindrop_diffuse = textures->CreateTexture("raindrop_diffuse", "raindrop.png");

		//Ship
		auto ship_atlas = textures->CreateTextureAtlas("ship", "ship.png", 2, 2, 3);
		auto ship_diffuse = textures->GetTexture("ship_1");
		auto ship_normal = textures->GetTexture("ship_2");
		auto ship_specular = textures->GetTexture("ship_3");

		//Star
		auto star_diffuse = textures->CreateTexture("star_diffuse", "star.png");


		/*
			Textures (GUI)
		*/

		//Button
		auto button_center_enabled_diffuse = textures->CreateTexture("button_center_enabled_diffuse", "button_center_enabled.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
		auto button_center_disabled_diffuse = textures->CreateTexture("button_center_disabled_diffuse", "button_center_disabled.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
		auto button_center_pressed_diffuse = textures->CreateTexture("button_center_pressed_diffuse", "button_center_pressed.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
		auto button_center_hovered_diffuse = textures->CreateTexture("button_center_hovered_diffuse", "button_center_hovered.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);

		auto button_top_enabled_diffuse = textures->CreateTexture("button_top_enabled_diffuse", "button_top_enabled.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
		auto button_top_focused_diffuse = textures->CreateTexture("button_top_focused_diffuse", "button_top_focused.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
		auto button_left_enabled_diffuse = textures->CreateTexture("button_left_enabled_diffuse", "button_left_enabled.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
		auto button_left_focused_diffuse = textures->CreateTexture("button_left_focused_diffuse", "button_left_focused.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);

		auto button_top_left_enabled_diffuse = textures->CreateTexture("button_top_left_enabled_diffuse", "button_top_left_enabled.png");
		auto button_top_left_focused_diffuse = textures->CreateTexture("button_top_left_focused_diffuse", "button_top_left_focused.png");

		//Check box
		auto check_box_center_enabled_diffuse = textures->CreateTexture("check_box_center_enabled_diffuse", "check_box_center_enabled.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
		auto check_box_center_hovered_diffuse = textures->CreateTexture("check_box_center_hovered_diffuse", "check_box_center_hovered.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);

		auto check_box_mark_enabled_diffuse = textures->CreateTexture("check_box_mark_enabled_diffuse", "check_box_mark_enabled.png");
		auto check_box_mark_disabled_diffuse = textures->CreateTexture("check_box_mark_disabled_diffuse", "check_box_mark_disabled.png");		
		auto check_box_mark_pressed_diffuse = textures->CreateTexture("check_box_mark_pressed_diffuse", "check_box_mark_pressed.png");
		auto check_box_mark_hovered_diffuse = textures->CreateTexture("check_box_mark_hovered_diffuse", "check_box_mark_hovered.png");

		//Mouse cursor
		auto mouse_cursor_diffuse = textures->CreateTexture("mouse_cursor_diffuse", "mouse_cursor.png");
		
		//Progress bar
		auto progress_bar_bar_enabled_diffuse = textures->CreateTexture("progress_bar_bar_enabled_diffuse", "progress_bar_bar_enabled.png");

		//Radio button
		auto radio_button_select_enabled_diffuse = textures->CreateTexture("radio_button_select_enabled_diffuse", "radio_button_select_enabled.png");
		auto radio_button_select_disabled_diffuse = textures->CreateTexture("radio_button_select_disabled_diffuse", "radio_button_select_disabled.png");
		auto radio_button_select_pressed_diffuse = textures->CreateTexture("radio_button_select_pressed_diffuse", "radio_button_select_pressed.png");
		auto radio_button_select_hovered_diffuse = textures->CreateTexture("radio_button_select_hovered_diffuse", "radio_button_select_hovered.png");

		//Text box
		auto text_box_cursor_enabled_diffuse = textures->CreateTexture("text_box_cursor_enabled_diffuse", "text_box_cursor_enabled.png");

		//Tooltip
		auto tooltip_center_diffuse = textures->CreateTexture("tooltip_center_diffuse", "tooltip_center.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
		auto tooltip_top_diffuse = textures->CreateTexture("tooltip_top_diffuse", "tooltip_top.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
		auto tooltip_left_diffuse = textures->CreateTexture("tooltip_left_diffuse", "tooltip_left.png",
			ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
		auto tooltip_top_left_diffuse = textures->CreateTexture("tooltip_top_left_diffuse", "tooltip_top_left.png");	


		textures->LoadAll(ion::resources::resource_manager::EvaluationStrategy::Eager);
		//while (!textures->Loaded());


		/*
			Frame sequences
		*/

		//Logo sequence
		auto logo_sequence = frame_sequences->CreateFrameSequence("logo_sequence", logo_first_frame, 16);


		/*
			Animations
		*/

		//Logo animation
		auto logo_animation = animations->CreateAnimation("logo_animation", logo_sequence, 2.0_sec);
		logo_animation->Direction(ion::graphics::textures::animation::PlaybackDirection::Alternate);


		/*
			Shaders
		*/

		//Model
		auto model_vert = shaders->CreateShader("model_vert", "IonModelShader.vert");
		auto model_frag = shaders->CreateShader("model_frag", "IonModelShader.frag");

		//Particle
		auto particle_vert = shaders->CreateShader("particle_vert", "IonParticleShader.vert");
		auto particle_frag = shaders->CreateShader("particle_frag", "IonParticleShader.frag");

		//Text
		auto text_vert = shaders->CreateShader("text_vert", "IonTextShader.vert");
		auto text_frag = shaders->CreateShader("text_frag", "IonTextShader.frag");


		//Flat model
		auto flat_model_vert = shaders->CreateShader("flat_model_vert", "IonFlatModelShader.vert");
		auto flat_model_frag = shaders->CreateShader("flat_model_frag", "IonFlatModelShader.frag");
		
		//Flat particle
		auto flat_particle_vert = shaders->CreateShader("flat_particle_vert", "IonFlatParticleShader.vert");
		auto flat_particle_frag = shaders->CreateShader("flat_particle_frag", "IonFlatParticleShader.frag");
		
		//Flat text
		auto flat_text_vert = shaders->CreateShader("flat_text_vert", "IonFlatTextShader.vert");
		auto flat_text_frag = shaders->CreateShader("flat_text_frag", "IonFlatTextShader.frag");
		

		shaders->LoadAll(ion::resources::resource_manager::EvaluationStrategy::Eager);
		//while (!shaders->Loaded());


		/*
			Shader programs
		*/

		auto model_prog = shader_programs->CreateShaderProgram("model_prog", model_vert, model_frag);
		auto particle_prog = shader_programs->CreateShaderProgram("particle_prog", particle_vert, particle_frag);
		auto text_prog = shader_programs->CreateShaderProgram("text_prog", text_vert, text_frag);

		auto flat_model_prog = shader_programs->CreateShaderProgram("flat_model_prog", flat_model_vert, flat_model_frag);
		auto flat_particle_prog = shader_programs->CreateShaderProgram("flat_particle_prog", flat_particle_vert, flat_particle_frag);
		auto flat_text_prog = shader_programs->CreateShaderProgram("flat_text_prog", flat_text_vert, flat_text_frag);


		shader_programs->LoadAll(ion::resources::resource_manager::EvaluationStrategy::Eager);
		//while (!shader_programs.Loaded());


		using namespace ion::graphics::shaders::variables;

		//Model
		{
			//Shader structs
			auto matrix_struct = model_prog->CreateStruct("matrix");
			auto scene_struct = model_prog->CreateStruct("scene");
			auto camera_struct = model_prog->CreateStruct("camera");
			auto primitive_struct = model_prog->CreateStruct("primitive");
			auto material_struct = model_prog->CreateStruct("material");
			auto fog_struct = model_prog->CreateStruct("fog");


			//Shader variables
			//Vertex
			model_prog->CreateAttribute<glsl::vec3>("vertex_position");
			model_prog->CreateAttribute<glsl::vec3>("vertex_normal");
			model_prog->CreateAttribute<glsl::vec4>("vertex_color");
			model_prog->CreateAttribute<glsl::vec3>("vertex_tex_coord");

			//Matrices			
			matrix_struct->CreateUniform<glsl::mat4>("model_view");
			matrix_struct->CreateUniform<glsl::mat4>("model_view_projection");
			matrix_struct->CreateUniform<glsl::mat3>("normal");

			//Scene
			scene_struct->CreateUniform<glsl::vec4>("ambient");
			scene_struct->CreateUniform<float>("gamma");
			scene_struct->CreateUniform<bool>("has_fog");
			scene_struct->CreateUniform<glsl::sampler1DArray>("lights");
			scene_struct->CreateUniform<glsl::sampler1DArray>("emissive_lights");
			scene_struct->CreateUniform<int>("light_count");
			scene_struct->CreateUniform<int>("emissive_light_count");

			//Camera
			camera_struct->CreateUniform<glsl::vec3>("position");

			//Primitive
			primitive_struct->CreateUniform<bool>("has_material");

			//Material
			material_struct->CreateUniform<glsl::vec4>("ambient");
			material_struct->CreateUniform<glsl::vec4>("diffuse");
			material_struct->CreateUniform<glsl::vec4>("specular");
			material_struct->CreateUniform<glsl::vec4>("emissive");
			material_struct->CreateUniform<float>("shininess");
			material_struct->CreateUniform<glsl::sampler2D>("diffuse_map");
			material_struct->CreateUniform<glsl::sampler2D>("normal_map");
			material_struct->CreateUniform<glsl::sampler2D>("specular_map");
			material_struct->CreateUniform<glsl::sampler2D>("emissive_map");
			material_struct->CreateUniform<bool>("has_diffuse_map");
			material_struct->CreateUniform<bool>("has_normal_map");
			material_struct->CreateUniform<bool>("has_specular_map");
			material_struct->CreateUniform<bool>("has_emissive_map");
			material_struct->CreateUniform<bool>("lighting_enabled");

			//Fog
			fog_struct->CreateUniform<int>("mode");
			fog_struct->CreateUniform<float>("density");
			fog_struct->CreateUniform<float>("near");
			fog_struct->CreateUniform<float>("far");
			fog_struct->CreateUniform<glsl::vec4>("color");

			shader_programs->LoadShaderVariableLocations(*model_prog);
		}

		//Particle
		{
			//Shader structs
			auto matrix_struct = particle_prog->CreateStruct("matrix");
			auto scene_struct = particle_prog->CreateStruct("scene");
			auto camera_struct = particle_prog->CreateStruct("camera");
			auto primitive_struct = particle_prog->CreateStruct("primitive");
			auto material_struct = particle_prog->CreateStruct("material");
			auto fog_struct = particle_prog->CreateStruct("fog");


			//Shader variables
			//Vertex
			particle_prog->CreateAttribute<glsl::vec3>("vertex_position");
			particle_prog->CreateAttribute<float>("vertex_rotation");
			particle_prog->CreateAttribute<float>("vertex_point_size");
			particle_prog->CreateAttribute<glsl::vec4>("vertex_color");

			//Matrices
			matrix_struct->CreateUniform<glsl::mat4>("model_view");
			matrix_struct->CreateUniform<glsl::mat4>("model_view_projection");
			matrix_struct->CreateUniform<glsl::mat3>("normal");

			//Scene
			scene_struct->CreateUniform<glsl::vec4>("ambient");
			scene_struct->CreateUniform<float>("gamma");
			scene_struct->CreateUniform<bool>("has_fog");
			scene_struct->CreateUniform<glsl::sampler1DArray>("lights");
			scene_struct->CreateUniform<glsl::sampler1DArray>("emissive_lights");
			scene_struct->CreateUniform<int>("light_count");
			scene_struct->CreateUniform<int>("emissive_light_count");

			//Camera
			camera_struct->CreateUniform<glsl::vec3>("position");
			camera_struct->CreateUniform<float>("rotation");

			//Primitive
			primitive_struct->CreateUniform<bool>("has_material");

			//Material
			material_struct->CreateUniform<glsl::vec4>("ambient");
			material_struct->CreateUniform<glsl::vec4>("diffuse");
			material_struct->CreateUniform<glsl::vec4>("specular");
			material_struct->CreateUniform<glsl::vec4>("emissive");
			material_struct->CreateUniform<float>("shininess");
			material_struct->CreateUniform<glsl::sampler2D>("diffuse_map");
			material_struct->CreateUniform<glsl::sampler2D>("normal_map");
			material_struct->CreateUniform<glsl::sampler2D>("specular_map");
			material_struct->CreateUniform<glsl::sampler2D>("emissive_map");
			material_struct->CreateUniform<bool>("has_diffuse_map");
			material_struct->CreateUniform<bool>("has_normal_map");
			material_struct->CreateUniform<bool>("has_specular_map");
			material_struct->CreateUniform<bool>("has_emissive_map");
			material_struct->CreateUniform<bool>("lighting_enabled");

			//Fog
			fog_struct->CreateUniform<int>("mode");
			fog_struct->CreateUniform<float>("density");
			fog_struct->CreateUniform<float>("near");
			fog_struct->CreateUniform<float>("far");
			fog_struct->CreateUniform<glsl::vec4>("color");

			shader_programs->LoadShaderVariableLocations(*particle_prog);
		}

		//Text
		{
			//Shader structs
			auto matrix_struct = text_prog->CreateStruct("matrix");
			auto scene_struct = text_prog->CreateStruct("scene");
			auto camera_struct = text_prog->CreateStruct("camera");
			auto primitive_struct = text_prog->CreateStruct("primitive");
			auto fog_struct = text_prog->CreateStruct("fog");


			//Shader variables
			//Vertex
			text_prog->CreateAttribute<glsl::vec3>("vertex_position");
			text_prog->CreateAttribute<glsl::vec4>("vertex_color");
			text_prog->CreateAttribute<glsl::vec3>("vertex_tex_coord");

			//Matrices
			matrix_struct->CreateUniform<glsl::mat4>("model_view");
			matrix_struct->CreateUniform<glsl::mat4>("model_view_projection");

			//Scene
			scene_struct->CreateUniform<glsl::vec4>("ambient");
			scene_struct->CreateUniform<float>("gamma");
			scene_struct->CreateUniform<bool>("has_fog");
			scene_struct->CreateUniform<glsl::sampler1DArray>("lights");
			scene_struct->CreateUniform<glsl::sampler1DArray>("emissive_lights");
			scene_struct->CreateUniform<int>("light_count");
			scene_struct->CreateUniform<int>("emissive_light_count");

			//Camera
			camera_struct->CreateUniform<glsl::vec3>("position");	

			//Primitive
			primitive_struct->CreateUniform<glsl::sampler2DArray>("texture");
			primitive_struct->CreateUniform<bool>("has_texture");

			//Fog
			fog_struct->CreateUniform<int>("mode");
			fog_struct->CreateUniform<float>("density");
			fog_struct->CreateUniform<float>("near");
			fog_struct->CreateUniform<float>("far");
			fog_struct->CreateUniform<glsl::vec4>("color");

			shader_programs->LoadShaderVariableLocations(*text_prog);
		}


		//Flat model
		{
			//Shader structs
			auto matrix_struct = flat_model_prog->CreateStruct("matrix");
			auto scene_struct = flat_model_prog->CreateStruct("scene");
			auto primitive_struct = flat_model_prog->CreateStruct("primitive");
			auto material_struct = flat_model_prog->CreateStruct("material");


			//Shader variables
			//Vertex
			flat_model_prog->CreateAttribute<glsl::vec3>("vertex_position");
			flat_model_prog->CreateAttribute<glsl::vec3>("vertex_normal");
			flat_model_prog->CreateAttribute<glsl::vec4>("vertex_color");
			flat_model_prog->CreateAttribute<glsl::vec3>("vertex_tex_coord");

			//Matrices
			matrix_struct->CreateUniform<glsl::mat4>("model_view_projection");

			//Scene
			scene_struct->CreateUniform<float>("gamma");

			//Primitive
			primitive_struct->CreateUniform<bool>("has_material");

			//Material
			material_struct->CreateUniform<glsl::vec4>("diffuse");
			material_struct->CreateUniform<glsl::sampler2D>("diffuse_map");
			material_struct->CreateUniform<bool>("has_diffuse_map");

			shader_programs->LoadShaderVariableLocations(*flat_model_prog);
		}

		//Flat particle
		{
			//Shader structs
			auto matrix_struct = flat_particle_prog->CreateStruct("matrix");
			auto scene_struct = flat_particle_prog->CreateStruct("scene");
			auto camera_struct = flat_particle_prog->CreateStruct("camera");
			auto primitive_struct = flat_particle_prog->CreateStruct("primitive");
			auto material_struct = flat_particle_prog->CreateStruct("material");


			//Shader variables
			//Vertex
			flat_particle_prog->CreateAttribute<glsl::vec3>("vertex_position");
			flat_particle_prog->CreateAttribute<float>("vertex_rotation");
			flat_particle_prog->CreateAttribute<float>("vertex_point_size");
			flat_particle_prog->CreateAttribute<glsl::vec4>("vertex_color");

			//Matrices
			matrix_struct->CreateUniform<glsl::mat4>("model_view_projection");

			//Scene
			scene_struct->CreateUniform<float>("gamma");

			//Camera
			camera_struct->CreateUniform<float>("rotation");

			//Primitive
			primitive_struct->CreateUniform<bool>("has_material");

			//Material
			material_struct->CreateUniform<glsl::vec4>("diffuse");
			material_struct->CreateUniform<glsl::sampler2D>("diffuse_map");
			material_struct->CreateUniform<bool>("has_diffuse_map");

			shader_programs->LoadShaderVariableLocations(*flat_particle_prog);
		}

		//Flat text
		{
			//Shader structs
			auto matrix_struct = flat_text_prog->CreateStruct("matrix");
			auto scene_struct = flat_text_prog->CreateStruct("scene");
			auto primitive_struct = flat_text_prog->CreateStruct("primitive");


			//Shader variables
			//Vertex
			flat_text_prog->CreateAttribute<glsl::vec3>("vertex_position");
			flat_text_prog->CreateAttribute<glsl::vec4>("vertex_color");
			flat_text_prog->CreateAttribute<glsl::vec3>("vertex_tex_coord");

			//Matrices
			matrix_struct->CreateUniform<glsl::mat4>("model_view_projection");

			//Scene
			scene_struct->CreateUniform<float>("gamma");

			//Primitive
			primitive_struct->CreateUniform<glsl::sampler2DArray>("texture");
			primitive_struct->CreateUniform<bool>("has_texture");

			shader_programs->LoadShaderVariableLocations(*flat_text_prog);
		}


		/*
			Fonts
		*/
		
		//Verdana 36px
		auto verdana_regular_36 = fonts->CreateFont("verdana_regular_36", "verdana.ttf", 36);
		auto verdana_bold_36 = fonts->CreateFont("verdana_bold_36", "verdanab.ttf", 36);
		auto verdana_italic_36 = fonts->CreateFont("verdana_italic_36", "verdanai.ttf", 36);
		auto verdana_bold_italic_36 = fonts->CreateFont("verdana_bold_italic_36", "verdanaz.ttf", 36);

		//Verdana 24px
		auto verdana_regular_24 = fonts->CreateFont("verdana_regular_24", "verdana.ttf", 24);
		auto verdana_bold_24 = fonts->CreateFont("verdana_bold_24", "verdanab.ttf", 24);
		auto verdana_italic_24 = fonts->CreateFont("verdana_italic_24", "verdanai.ttf", 24);
		auto verdana_bold_italic_24 = fonts->CreateFont("verdana_bold_italic_24", "verdanaz.ttf", 24);


		/*
			Fonts (GUI)
		*/

		//Verdana 12px
		auto verdana_regular_12 = fonts->CreateFont("verdana_regular_12", "verdana.ttf", 12);
		auto verdana_bold_12 = fonts->CreateFont("verdana_bold_12", "verdanab.ttf", 12);
		auto verdana_italic_12 = fonts->CreateFont("verdana_italic_12", "verdanai.ttf", 12);
		auto verdana_bold_italic_12 = fonts->CreateFont("verdana_bold_italic_12", "verdanaz.ttf", 12);


		fonts->LoadAll(ion::resources::resource_manager::EvaluationStrategy::Eager);
		//while (!fonts.Loaded());


		/*
			Type faces
		*/

		//Verdana 36px
		auto verdana_36 = 
			type_faces->CreateTypeFace(
				"verdana_36",
				verdana_regular_36,
				verdana_bold_36,
				verdana_italic_36,
				verdana_bold_italic_36);

		//Verdana 24px
		auto verdana_24 =
			type_faces->CreateTypeFace(
				"verdana_24",
				verdana_regular_24,
				verdana_bold_24,
				verdana_italic_24,
				verdana_bold_italic_24);


		/*
			Type faces (GUI)
		*/

		//Verdana 12px
		auto verdana_12 =
			type_faces->CreateTypeFace(
				"verdana_12",
				verdana_regular_12,
				verdana_bold_12,
				verdana_italic_12,
				verdana_bold_italic_12);

		
		/*
			Sounds
		*/

		auto flicker = sounds->CreateSound(ion::sounds::Sound::Positional("flicker", "flicker.wav",
			ion::sounds::sound::SoundType::Sample, ion::sounds::sound::SoundLoopingMode::Forward));
		flicker->Distance(0.4_r); //Min distance of 10 meters

		auto night_runner = sounds->CreateSound("night_runner", "night_runner.mp3",
			ion::sounds::sound::SoundType::Stream, ion::sounds::sound::SoundLoopingMode::Forward);

		
		auto gui_sound_channel_group = sounds->CreateSoundChannelGroup("gui");
		gui_sound_channel_group->Volume(0.2_r);

		auto sound_listener = sounds->CreateSoundListener("listener");


		/*
			Sounds (GUI)
		*/

		auto click = sounds->CreateSound("click", "click.wav", ion::sounds::sound::SoundType::Sample);	
		

		sounds->LoadAll(ion::resources::resource_manager::EvaluationStrategy::Eager);
		//while (!sounds->Loaded());


		/*
			Material
		*/

		//Aura
		auto aura = materials->CreateMaterial("aura",
			aura_diffuse, nullptr, nullptr, aura_emissive);
		aura->EmissiveColor(ion::graphics::utilities::color::Pink);

		//Brick wall
		auto brick_wall = materials->CreateMaterial("brick_wall",
			brick_wall_diffuse, brick_wall_normal, brick_wall_specular, nullptr);
		brick_wall->DiffuseColor(ion::graphics::utilities::color::BurlyWood);

		//Cloud
		auto cloud = materials->CreateMaterial("cloud", cloud_diffuse);

		//Logo
		auto logo = materials->CreateMaterial("logo", logo_animation);
		logo->LightingEnabled(false);

		//Pyramid (egyptian)
		auto pyramid_egyptian = materials->CreateMaterial("pyramid_egyptian",
			pyramid_egyptian_diffuse, pyramid_egyptian_normal, pyramid_egyptian_specular, nullptr);

		//Pyramid (mayan)
		auto pyramid_mayan = materials->CreateMaterial("pyramid_mayan",
			pyramid_mayan_diffuse, pyramid_mayan_normal, pyramid_mayan_specular, nullptr);

		//Raindrop
		auto raindrop = materials->CreateMaterial("raindrop", raindrop_diffuse);
		
		//Ship
		auto ship = materials->CreateMaterial("ship",
			ship_diffuse, ship_normal, ship_specular, nullptr);
		
		//Star
		auto star = materials->CreateMaterial("star", star_diffuse);
		star->LightingEnabled(false);

		auto star_red = materials->CreateMaterial("star_red", star_diffuse, ion::graphics::utilities::color::Red);
		star_red->LightingEnabled(false);

		auto star_green = materials->CreateMaterial("star_green", star_diffuse, ion::graphics::utilities::color::Green);
		star_green->LightingEnabled(false);

		auto star_blue = materials->CreateMaterial("star_blue", star_diffuse, ion::graphics::utilities::color::Blue);
		star_blue->LightingEnabled(false);

		/*
			Material (GUI)
		*/

		//Button
		auto button_center_enabled = materials->CreateMaterial("button_center_enabled", button_center_enabled_diffuse);
		button_center_enabled->LightingEnabled(false);

		auto button_center_disabled = materials->CreateMaterial("button_center_disabled", button_center_disabled_diffuse);
		button_center_disabled->LightingEnabled(false);

		auto button_center_pressed = materials->CreateMaterial("button_center_pressed", button_center_pressed_diffuse);
		button_center_pressed->LightingEnabled(false);

		auto button_center_hovered = materials->CreateMaterial("button_center_hovered", button_center_hovered_diffuse);
		button_center_hovered->LightingEnabled(false);
		

		auto button_top_enabled = materials->CreateMaterial("button_top_enabled", button_top_enabled_diffuse);
		button_top_enabled->LightingEnabled(false);

		auto button_top_focused = materials->CreateMaterial("button_top_focused", button_top_focused_diffuse);
		button_top_focused->LightingEnabled(false);

		auto button_left_enabled = materials->CreateMaterial("button_left_enabled", button_left_enabled_diffuse);
		button_left_enabled->LightingEnabled(false);

		auto button_left_focused = materials->CreateMaterial("button_left_focused", button_left_focused_diffuse);
		button_left_focused->LightingEnabled(false);


		auto button_top_left_enabled = materials->CreateMaterial("button_top_left_enabled", button_top_left_enabled_diffuse);
		button_top_left_enabled->LightingEnabled(false);

		auto button_top_left_focused = materials->CreateMaterial("button_top_left_focused", button_top_left_focused_diffuse);
		button_top_left_focused->LightingEnabled(false);

		//Check box
		auto check_box_center_enabled = materials->CreateMaterial("check_box_center_enabled", check_box_center_enabled_diffuse);
		check_box_center_enabled->LightingEnabled(false);

		auto check_box_center_hovered = materials->CreateMaterial("check_box_center_hovered", check_box_center_hovered_diffuse);
		check_box_center_hovered->LightingEnabled(false);


		auto check_box_mark_enabled = materials->CreateMaterial("check_box_mark_enabled", check_box_mark_enabled_diffuse);
		check_box_mark_enabled->LightingEnabled(false);

		auto check_box_mark_disabled = materials->CreateMaterial("check_box_mark_disabled", check_box_mark_disabled_diffuse);
		check_box_mark_disabled->LightingEnabled(false);

		auto check_box_mark_pressed = materials->CreateMaterial("check_box_mark_pressed", check_box_mark_pressed_diffuse);
		check_box_mark_pressed->LightingEnabled(false);

		auto check_box_mark_hovered = materials->CreateMaterial("check_box_mark_hovered", check_box_mark_hovered_diffuse);
		check_box_mark_hovered->LightingEnabled(false);

		//Mouse cursor
		auto mouse_cursor_enabled = materials->CreateMaterial("mouse_cursor", mouse_cursor_diffuse);
		mouse_cursor_enabled->LightingEnabled(false);

		//Progress bar
		auto progress_bar_bar_enabled = materials->CreateMaterial("progress_bar_bar_enabled", progress_bar_bar_enabled_diffuse);
		progress_bar_bar_enabled->LightingEnabled(false);

		//Radio buttons
		auto radio_button_select_enabled = materials->CreateMaterial("radio_button_select_enabled", radio_button_select_enabled_diffuse);
		radio_button_select_enabled->LightingEnabled(false);

		auto radio_button_select_disabled = materials->CreateMaterial("radio_button_select_disabled", radio_button_select_disabled_diffuse);
		radio_button_select_disabled->LightingEnabled(false);

		auto radio_button_select_pressed = materials->CreateMaterial("radio_button_select_pressed", radio_button_select_pressed_diffuse);
		radio_button_select_pressed->LightingEnabled(false);

		auto radio_button_select_hovered = materials->CreateMaterial("radio_button_select_hovered", radio_button_select_hovered_diffuse);
		radio_button_select_hovered->LightingEnabled(false);

		//Text box
		auto text_box_cursor_enabled = materials->CreateMaterial("text_box_cursor_enabled", text_box_cursor_enabled_diffuse);
		text_box_cursor_enabled->LightingEnabled(false);

		//Tooltip
		auto tooltip_center_enabled = materials->CreateMaterial("tooltip_center", tooltip_center_diffuse);
		tooltip_center_enabled->LightingEnabled(false);


		auto tooltip_top_enabled = materials->CreateMaterial("tooltip_top", tooltip_top_diffuse);
		tooltip_top_enabled->LightingEnabled(false);

		auto tooltip_left_enabled = materials->CreateMaterial("tooltip_left", tooltip_left_diffuse);
		tooltip_left_enabled->LightingEnabled(false);


		auto tooltip_top_left_enabled = materials->CreateMaterial("tooltip_top_left", tooltip_top_left_diffuse);
		tooltip_top_left_enabled->LightingEnabled(false);


		/*
			Particle system
		*/

		using namespace ion::graphics::utilities;


		//Rain
		auto rain = particle_systems->CreateParticleSystem("rain");

		auto emitter = rain->CreateEmitter(
			ion::graphics::particles::Emitter::Box(
				"spawner", vector3::Zero, vector2::NegativeUnitY, {3.56_r, 0.1_r}, {}, 50.0_r, 0.0_r, {}, 100
			));

		emitter->ParticleVelocity(1.5_r, 2.0_r);
		emitter->ParticleSize(8.0_r, 24.0_r);
		emitter->ParticleMass(1.0_r, 1.0_r);
		emitter->ParticleColor(Color{255, 255, 255, 0.75_r}, color::White);
		emitter->ParticleLifetime(1.4_sec, 1.4_sec);
		emitter->ParticleMaterial(raindrop);


		/*
			Text
		*/

		//FPS
		auto fps_text =
			texts->CreateText(
				"fps",
				"",
				verdana_36);
		fps_text->Formatting(ion::graphics::fonts::text::TextFormatting::None);
		fps_text->DefaultForegroundColor(color::White);

		//Header
		auto header_text =
			texts->CreateText(
				"header",
				"Powered by <b>the ION game engine</b>",
				verdana_36);
		header_text->Alignment(ion::graphics::fonts::text::TextAlignment::Center);
		header_text->DefaultForegroundColor(color::White);

		//Sub header
		auto sub_header_text =
			texts->CreateText(
				"sub_header",
				"Press any key to continue...",
				verdana_24);
		sub_header_text->Alignment(ion::graphics::fonts::text::TextAlignment::Center);
		sub_header_text->DefaultForegroundColor(color::DimGray);


		/*
			Text (GUI)
		*/

		//Caption
		auto caption_text =
			texts->CreateText(
				"caption",
				"",
				verdana_12);

		caption_text->DefaultForegroundColor(color::White);


		using namespace ion::utilities;

		//Default shader programs
		scene_manager->AddDefaultShaderProgram(ion::graphics::scene::query::scene_query::QueryType::Model,
			shader_programs->GetShaderProgram("model_prog"));
		scene_manager->AddDefaultShaderProgram(ion::graphics::scene::query::scene_query::QueryType::ParticleSystem,
			shader_programs->GetShaderProgram("particle_prog"));
		scene_manager->AddDefaultShaderProgram(ion::graphics::scene::query::scene_query::QueryType::Text,
			shader_programs->GetShaderProgram("flat_text_prog"));

		//Default shader programs (GUI)
		gui_scene_manager->AddDefaultShaderProgram(ion::graphics::scene::query::scene_query::QueryType::Model,
			shader_programs->GetShaderProgram("flat_model_prog"));
		gui_scene_manager->AddDefaultShaderProgram(ion::graphics::scene::query::scene_query::QueryType::ParticleSystem,
			shader_programs->GetShaderProgram("flat_particle_prog"));
		gui_scene_manager->AddDefaultShaderProgram(ion::graphics::scene::query::scene_query::QueryType::Text,
			shader_programs->GetShaderProgram("flat_text_prog"));

		
		/*
			Scene
		*/

		//Aura
		auto aura_model = scene_manager->CreateModel("ship_aura_model");
		auto aura_sprite = aura_model->CreateMesh(ion::graphics::scene::shapes::Sprite{
			vector3::Zero, {0.432_r, 0.45_r}, aura});
		aura_sprite->FillOpacity(0.65_r);

		//Brick wall
		auto background_model = scene_manager->CreateModel("background_model");
		background_model->CreateMesh(ion::graphics::scene::shapes::Sprite{
			vector3::Zero, {1.75_r, 1.75_r}, brick_wall}); //Center
		background_model->CreateMesh(ion::graphics::scene::shapes::Sprite{
			{-1.75_r, 0.0_r, 0.0_r}, {1.75_r, 1.75_r}, brick_wall}); //Left
		background_model->CreateMesh(ion::graphics::scene::shapes::Sprite{
			{1.75_r, 0.0_r, 0.0_r}, {1.75_r, 1.75_r}, brick_wall}); //Right

		//Camera
		auto frustum = ion::graphics::render::Frustum::Orthographic(
			Aabb{-1.0_r, 1.0_r}, 1.0_r, 100.0_r, 16.0_r / 9.0_r);
		auto main_camera = scene_manager->CreateCamera("main_camera", frustum);
		auto player_camera = scene_manager->CreateCamera("player_camera", frustum);
		viewport->ConnectedCamera(main_camera);

		//Cloud
		auto cloud_model = scene_manager->CreateModel("cloud_model");
		cloud_model->CreateMesh(ion::graphics::scene::shapes::Sprite{
			{-1.0_r, 0.4_r, 0.0_r}, {1.1627182_r, 1.25_r}, cloud}); //Left
		cloud_model->CreateMesh(ion::graphics::scene::shapes::Sprite{
			{1.0_r, -0.4_r, 0.0_r}, {1.1627182_r, 1.25_r}, cloud}); //Right

		//FPS
		auto fps = scene_manager->CreateText("fps", fps_text);

		//Header
		auto header = scene_manager->CreateText("header", header_text);

		//Sub header
		auto sub_header = scene_manager->CreateText("sub_header", sub_header_text);

		//Light (red)
		auto red_light = scene_manager->CreateLight("red_light");
		red_light->Type(ion::graphics::scene::light::LightType::Point);
		red_light->DiffuseColor(color::Red);
		red_light->AmbientColor(color::DarkRed);
		//red_light->Attenuation(1.0_r, 0.09_r, 0.032_r);
		red_light->Radius(1.5_r);

		//Lamp flicker
		auto red_lamp_flicker = scene_manager->CreateSound("red_lamp_flicker", flicker);
		auto green_lamp_flicker = scene_manager->CreateSound("green_lamp_flicker", flicker);

		//Light (green)
		auto green_light = scene_manager->CreateLight("green_light");
		green_light->Type(ion::graphics::scene::light::LightType::Point);
		green_light->DiffuseColor(color::Green);
		green_light->AmbientColor(color::DarkGreen);
		//green_light->Attenuation(1.0_r, 0.09_r, 0.032_r);
		green_light->Radius(1.5_r);

		//Light (ship)
		auto ship_light = scene_manager->CreateLight("ship_light");
		ship_light->Type(ion::graphics::scene::light::LightType::Spot);
		ship_light->Direction(Vector3{0.0_r, 0.83_r, -0.55_r});
		ship_light->DiffuseColor(color::White);
		ship_light->Attenuation(1.0_r, 0.09_r, 0.032_r);
		ship_light->Cutoff(math::ToRadians(20.0_r), math::ToRadians(30.0_r));

		//Logo
		auto logo_model = scene_manager->CreateModel("logo_model");
		logo_model->CreateMesh(ion::graphics::scene::shapes::Sprite{
			vector3::Zero, {1.0_r, 0.5_r}, logo});
		logo_model->AddRenderPass(ion::graphics::render::RenderPass{});

		//Player ear
		auto player_ear = scene_manager->CreateSoundListener({}, sound_listener);

		//Pyramid (egyptian)
		auto pyramid_egyptian_model = scene_manager->CreateModel();
		pyramid_egyptian_model->CreateMesh(ion::graphics::scene::shapes::Sprite{
			vector3::Zero, {0.5_r, 0.5_r}, pyramid_egyptian});

		//Pyramid (mayan)
		auto pyramid_mayan_model = scene_manager->CreateModel();
		pyramid_mayan_model->CreateMesh(ion::graphics::scene::shapes::Sprite{
			vector3::Zero, {0.5_r, 0.5_r}, pyramid_mayan});

		//Rain
		auto rain_particles = scene_manager->CreateParticleSystem("rain", rain);
		rain_particles->Get()->StartAll();

		//Ship
		auto ship_model = scene_manager->CreateModel("ship_model");
		ship_model->CreateMesh(ion::graphics::scene::shapes::Sprite{
			vector3::Zero, {0.4_r, 0.4_r}, ship});
		ship_model->BoundingVolumeExtent({{0.3_r, 0.2_r}, {0.7_r, 0.8_r}});
		ship_model->QueryFlags(1);
		ship_model->QueryMask(2 | 4);
		//ship_model->ShowBoundingVolumes(true);

		//Star
		auto star_model = scene_manager->CreateModel("ship_star_model");
		star_model->CreateMesh(ion::graphics::scene::shapes::Sprite{
			vector3::Zero, {0.05_r, 0.05_r}, star});
		star_model->AddRenderPass(ion::graphics::render::RenderPass{});


		/*
			Scene nodes
		*/

		//FPS
		auto fps_node = scene_graph->RootNode().CreateChildNode({}, {-1.75_r, 0.98_r, -1.5_r});
		fps_node->Scaling({0.5_r, 0.5_r});
		fps_node->AttachObject(*fps);

		//Main camera
		auto main_camera_node = scene_graph->RootNode().CreateChildNode("main_camera_node");
		main_camera_node->AttachObject(*main_camera);


		//Intro node
		auto intro_node = scene_graph->RootNode().CreateChildNode("intro_node");

		//Logo
		auto logo_node = intro_node->CreateChildNode({}, {0.0_r, 0.25_r, -2.0_r});
		logo_node->AttachObject(*logo_model);

		//Header
		auto header_node = intro_node->CreateChildNode({}, {0.0_r, -0.1_r, -2.0_r});
		header_node->Scaling({0.75_r, 0.75_r});
		header_node->AttachObject(*header);

		//Header
		auto sub_header_node = header_node->CreateChildNode({}, {0.0_r, -0.25_r, 0.0_r});
		sub_header_node->AttachObject(*sub_header);


		//Level node
		auto level_node = scene_graph->RootNode().CreateChildNode("level_node");

		//Brick wall
		auto background_node = level_node->CreateChildNode({}, {0.0_r, 0.0_r, -2.25_r});
		background_node->AttachObject(*background_model);

		//Cloud
		auto cloud_node = level_node->CreateChildNode({}, {0.0_r, 0.0_r, -1.6_r});
		cloud_node->AttachObject(*cloud_model);	

		//Light (red)
		auto red_light_node = level_node->CreateChildNode({}, {-1.5_r, -0.75_r, -1.0_r});
		red_light_node->AttachObject(*red_light);

		//Lamp (red)
		auto red_lamp_node = red_light_node->CreateChildNode({}, {0.0_r, 0.0_r, -0.8_r});
		red_lamp_node->AttachObject(*red_lamp_flicker);

		//Light (green)
		auto green_light_node = level_node->CreateChildNode({}, {1.5_r, 0.75_r, -1.0_r});
		green_light_node->AttachObject(*green_light);

		//Lamp (green)
		auto green_lamp_node = green_light_node->CreateChildNode({}, {0.0_r, 0.0_r, -0.8_r});
		green_lamp_node->AttachObject(*green_lamp_flicker);

		//Pyramid (egyptian)
		auto pyramid_egyptian_node = level_node->CreateChildNode({}, {1.0_r, 0.5_r, -2.0_r});
		pyramid_egyptian_node->AttachObject(*pyramid_egyptian_model);

		//Pyramid (mayan)
		auto pyramid_mayan_node = level_node->CreateChildNode({}, {-1.0_r, -0.5_r, -2.0_r});
		pyramid_mayan_node->AttachObject(*pyramid_mayan_model);

		//Rain
		auto particle_node = level_node->CreateChildNode({}, {0.0_r, 1.0_r, -1.75_r}, vector2::NegativeUnitY);
		particle_node->AttachObject(*rain_particles);


		//Player
		auto player_node = level_node->CreateChildNode("player_node", {0.0_r, -0.65_r, -1.8_r});

		//Player camera
		auto player_cam_node = player_node->CreateChildNode({}, {0.0_r, 0.0_r, 1.8_r});
		player_cam_node->AttachObject(*player_camera);

		//Ship
		auto ship_node = player_node->CreateChildNode("ship_node");
		ship_node->AttachObject(*ship_model);
		ship_node->AttachObject(*player_ear);

		//Aura
		auto aura_node = ship_node->CreateChildNode({}, {0.0_r, -0.05_r, -0.1_r});
		aura_node->InheritRotation(false);
		aura_node->AttachObject(*aura_model);

		//Ship light
		auto light_node = ship_node->CreateChildNode("ship_light_node", {0.0_r, -0.15_r, -0.05_r}, vector2::UnitY, false);
		light_node->AttachObject(*ship_light);

		//Star
		auto star_node = ship_node->CreateChildNode({}, {0.15_r, 0.2_r, 0.1_r});
		star_node->AttachObject(*star_model);


		/*
			Node animations
		*/

		using namespace ion::graphics::scene::graph::animations;

		auto aura_rotator = aura_node->CreateAnimation("aura_rotator");
		aura_rotator->AddRotation(math::ToRadians(-90.0_r), 1.0_sec);
		aura_rotator->Start();

		auto cloud_scaler = cloud_node->CreateAnimation("cloud_scaler");
		cloud_scaler->AddScaling(0.25_r, 10.0_sec, 0.0_sec, node_animation::MotionTechniqueType::Sigmoid);
		cloud_scaler->AddScaling(-0.25_r, 10.0_sec, 10.0_sec, node_animation::MotionTechniqueType::Sigmoid);
		cloud_scaler->Start();

		auto ship_idle_mover = ship_node->CreateAnimation("ship_idle_mover");
		ship_idle_mover->AddTranslation({0.0_r, 0.02_r, 0.0_r}, 2.0_sec);
		ship_idle_mover->AddTranslation({0.02_r, -0.02_r, 0.0_r}, 2.0_sec, 2.0_sec);
		ship_idle_mover->AddTranslation({-0.02_r, -0.02_r, 0.0_r}, 2.0_sec, 4.0_sec);
		ship_idle_mover->AddTranslation({-0.02_r, 0.02_r, 0.0_r}, 2.0_sec, 6.0_sec);
		ship_idle_mover->AddTranslation({0.02_r, 0.02_r, 0.0_r}, 2.0_sec, 8.0_sec);
		ship_idle_mover->AddTranslation({0.0_r, -0.02_r, 0.0_r}, 2.0_sec, 10.0_sec);

		auto ship_idle_rotator = ship_node->CreateAnimation("ship_idle_rotator");
		ship_idle_rotator->AddRotation(math::ToRadians(-2.5_r), 2.0_sec, 2.0_sec);
		ship_idle_rotator->AddRotation(math::ToRadians(2.5_r), 2.0_sec, 4.0_sec);
		ship_idle_rotator->AddRotation(math::ToRadians(2.5_r), 2.0_sec, 6.0_sec);
		ship_idle_rotator->AddRotation(math::ToRadians(-2.5_r), 2.0_sec, 8.0_sec);

		auto ship_idle = ship_node->CreateAnimationGroup("ship_idle");
		ship_idle->Add(ship_idle_mover);
		ship_idle->Add(ship_idle_rotator);

		auto ship_idle_timeline = ship_node->CreateTimeline("ship_idle_timeline", 1.0_r, false);
		ship_idle_timeline->Attach(ship_idle);


		/*
			GUI themes
		*/

		//Caption styles
		ion::graphics::fonts::text::TextBlockStyle caption_style_enabled;
		caption_style_enabled.ForegroundColor = caption_text->DefaultForegroundColor();

		ion::graphics::fonts::text::TextBlockStyle caption_style_disabled;
		caption_style_disabled.ForegroundColor = color::DarkGray;

		ion::graphics::fonts::text::TextBlockStyle caption_style_hovered;
		caption_style_hovered.ForegroundColor = caption_text->DefaultForegroundColor();
		caption_style_hovered.Decoration = ion::graphics::fonts::text::TextDecoration::Underline;

		//Placeholder text styles
		ion::graphics::fonts::text::TextBlockStyle placeholder_text_style_enabled;
		placeholder_text_style_enabled.ForegroundColor = color::Gray;
		placeholder_text_style_enabled.FontStyle = ion::graphics::fonts::text::TextFontStyle::Italic;

		ion::graphics::fonts::text::TextBlockStyle placeholder_text_style_disabled;
		placeholder_text_style_disabled.ForegroundColor = color::DarkGray;
		placeholder_text_style_disabled.FontStyle = ion::graphics::fonts::text::TextFontStyle::Italic;


		//Theme
		auto theme = gui_controller.CreateTheme("default", gui_scene_manager);

		//Mouse cursor skin
		ion::gui::skins::gui_skin::SkinParts parts;
		parts.Center.Enabled = mouse_cursor_enabled;

		auto mouse_cursor_skin = theme->CreateSkin<ion::gui::controls::GuiMouseCursor>(parts);

		//Tooltip skin
		parts = {};
		parts.Center.Enabled = tooltip_center_enabled;
		parts.Center.FillColor.A(0.9_r);
		parts.Border.Sides.Top.Enabled = tooltip_top_enabled;
		parts.Border.Sides.Bottom.Enabled = tooltip_top_enabled;
		parts.Border.Sides.Bottom.FlipVertical = true;
		parts.Border.Sides.Left.Enabled = tooltip_left_enabled;
		parts.Border.Sides.Right.Enabled = tooltip_left_enabled;
		parts.Border.Sides.Right.FlipHorizontal = true;
		parts.Border.Corners.TopLeft.Enabled = tooltip_top_left_enabled;
		parts.Border.Corners.TopRight.Enabled = tooltip_top_left_enabled;
		parts.Border.Corners.TopRight.FlipHorizontal = true;
		parts.Border.Corners.BottomLeft.Enabled = tooltip_top_left_enabled;
		parts.Border.Corners.BottomLeft.FlipVertical = true;
		parts.Border.Corners.BottomRight.Enabled = tooltip_top_left_enabled;
		parts.Border.Corners.BottomRight.FlipHorizontal = true;
		parts.Border.Corners.BottomRight.FlipVertical = true;
			
		ion::gui::skins::gui_skin::SkinTextPart caption_part;
		caption_part.Base = caption_text;

		auto tooltip_skin = theme->CreateSkin<ion::gui::controls::GuiTooltip>(parts, caption_part);

		//Button skin
		parts = {};
		parts.Center.Enabled = button_center_enabled;
		parts.Center.Disabled = button_center_disabled;
		parts.Center.Pressed = button_center_pressed;
		parts.Center.Hovered = button_center_hovered;
		parts.Border.Sides.Top.Enabled = button_top_enabled;
		parts.Border.Sides.Top.Focused = button_top_focused;
		parts.Border.Sides.Bottom.Enabled = button_top_enabled;
		parts.Border.Sides.Bottom.Focused = button_top_focused;
		parts.Border.Sides.Bottom.FlipVertical = true;
		parts.Border.Sides.Left.Enabled = button_left_enabled;
		parts.Border.Sides.Left.Focused = button_left_focused;
		parts.Border.Sides.Right.Enabled = button_left_enabled;		
		parts.Border.Sides.Right.Focused = button_left_focused;
		parts.Border.Sides.Right.FlipHorizontal = true;
		parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopRight.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.FlipHorizontal = true;
		parts.Border.Corners.BottomLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomLeft.Focused = button_top_left_focused;
		parts.Border.Corners.BottomLeft.FlipVertical = true;
		parts.Border.Corners.BottomRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomRight.Focused = button_top_left_focused;
		parts.Border.Corners.BottomRight.FlipHorizontal = true;
		parts.Border.Corners.BottomRight.FlipVertical = true;
			
		caption_part = {};
		caption_part.Base = caption_text;
		caption_part.Enabled = caption_style_enabled;
		caption_part.Disabled = caption_style_disabled;

		ion::gui::skins::gui_skin::SkinSoundParts sound_parts;
		sound_parts.Clicked.Base = click;

		auto button_skin = theme->CreateSkin<ion::gui::controls::GuiButton>(parts, caption_part, sound_parts);

		//Check box skin
		parts = {};
		parts.Center.Enabled = check_box_center_enabled;
		parts.Center.Disabled = check_box_center_enabled;
		parts.Center.Pressed = check_box_center_enabled;
		parts.Center.Hovered = check_box_center_hovered;
		parts.Border.Sides.Top.Enabled = button_top_enabled;
		parts.Border.Sides.Top.Focused = button_top_focused;
		parts.Border.Sides.Bottom.Enabled = button_top_enabled;
		parts.Border.Sides.Bottom.Focused = button_top_focused;
		parts.Border.Sides.Bottom.FlipVertical = true;
		parts.Border.Sides.Left.Enabled = button_left_enabled;
		parts.Border.Sides.Left.Focused = button_left_focused;
		parts.Border.Sides.Right.Enabled = button_left_enabled;
		parts.Border.Sides.Right.Focused = button_left_focused;
		parts.Border.Sides.Right.FlipHorizontal = true;
		parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopRight.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.FlipHorizontal = true;
		parts.Border.Corners.BottomLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomLeft.Focused = button_top_left_focused;
		parts.Border.Corners.BottomLeft.FlipVertical = true;
		parts.Border.Corners.BottomRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomRight.Focused = button_top_left_focused;
		parts.Border.Corners.BottomRight.FlipHorizontal = true;
		parts.Border.Corners.BottomRight.FlipVertical = true;
			
		caption_part = {};
		caption_part.Base = caption_text;
		caption_part.Enabled = caption_style_enabled;
		caption_part.Disabled = caption_style_disabled;

		sound_parts = {};
		sound_parts.Clicked.Base = click;

		ion::gui::skins::gui_skin::SkinPart check_mark_part;
		check_mark_part.Enabled = check_box_mark_enabled;
		check_mark_part.Disabled = check_box_mark_disabled;
		check_mark_part.Pressed = check_box_mark_pressed;
		check_mark_part.Hovered = check_box_mark_hovered;
		check_mark_part.Scaling = 0.6_r;

		auto check_box_skin = theme->CreateSkin<ion::gui::controls::GuiCheckBox>(parts, caption_part, sound_parts);
		check_box_skin->AddPart("check-mark", check_mark_part); //Additional

		//Group box skin
		parts = {};
		parts.Border.Sides.Top.Enabled = button_top_enabled;
		parts.Border.Sides.Bottom.Enabled = button_top_enabled;
		parts.Border.Sides.Bottom.FlipVertical = true;
		parts.Border.Sides.Left.Enabled = button_left_enabled;
		parts.Border.Sides.Right.Enabled = button_left_enabled;
		parts.Border.Sides.Right.FlipHorizontal = true;
		parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopRight.FlipHorizontal = true;
		parts.Border.Corners.BottomLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomLeft.FlipVertical = true;
		parts.Border.Corners.BottomRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomRight.FlipHorizontal = true;
		parts.Border.Corners.BottomRight.FlipVertical = true;
			
		caption_part = {};
		caption_part.Base = caption_text;
		caption_part.Enabled = caption_style_enabled;
		caption_part.Disabled = caption_style_disabled;

		auto group_box_skin = theme->CreateSkin<ion::gui::controls::GuiGroupBox>(parts, caption_part);

		//Label skin
		caption_part = {};
		caption_part.Base = caption_text;
		caption_part.Enabled = caption_style_enabled;
		caption_part.Hovered = caption_style_hovered;

		auto label_skin = theme->CreateSkin<ion::gui::controls::GuiLabel>(caption_part);

		//List box skin
		parts = {};
		parts.Center.Enabled = check_box_center_enabled;
		parts.Center.Hovered = check_box_center_hovered;
		parts.Border.Sides.Top.Enabled = button_top_enabled;
		parts.Border.Sides.Top.Focused = button_top_focused;
		parts.Border.Sides.Bottom.Enabled = button_top_enabled;
		parts.Border.Sides.Bottom.Focused = button_top_focused;
		parts.Border.Sides.Bottom.FlipVertical = true;
		parts.Border.Sides.Left.Enabled = button_left_enabled;
		parts.Border.Sides.Left.Focused = button_left_focused;
		parts.Border.Sides.Right.Enabled = button_left_enabled;
		parts.Border.Sides.Right.Focused = button_left_focused;
		parts.Border.Sides.Right.FlipHorizontal = true;
		parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopRight.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.FlipHorizontal = true;
		parts.Border.Corners.BottomLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomLeft.Focused = button_top_left_focused;
		parts.Border.Corners.BottomLeft.FlipVertical = true;
		parts.Border.Corners.BottomRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomRight.Focused = button_top_left_focused;
		parts.Border.Corners.BottomRight.FlipHorizontal = true;
		parts.Border.Corners.BottomRight.FlipVertical = true;
			
		caption_part = {};
		caption_part.Base = caption_text;
		caption_part.Enabled = caption_style_enabled;
		caption_part.Disabled = caption_style_disabled;

		sound_parts = {};
		sound_parts.Changed.Base = click;

		ion::gui::skins::gui_skin::SkinPart selection_part;
		selection_part.Enabled = button_center_hovered;
		selection_part.FillColor.A(0.5_r);

		ion::gui::skins::gui_skin::SkinTextPart lines_part;
		lines_part.Base = caption_text;
		lines_part.Enabled = caption_style_enabled;
		lines_part.Disabled = caption_style_disabled;

		auto list_box_skin = theme->CreateSkin<ion::gui::controls::GuiListBox>(parts, caption_part, sound_parts);
		list_box_skin->AddPart("selection", selection_part); //Additional
		list_box_skin->AddTextPart("lines", lines_part); //Additional

		//Progress bar skin
		parts = {};
		parts.Center.Enabled = check_box_center_enabled;
		parts.Border.Sides.Top.Enabled = button_top_enabled;
		parts.Border.Sides.Bottom.Enabled = button_top_enabled;
		parts.Border.Sides.Bottom.FlipVertical = true;
		parts.Border.Sides.Left.Enabled = button_left_enabled;
		parts.Border.Sides.Right.Enabled = button_left_enabled;
		parts.Border.Sides.Right.FlipHorizontal = true;
		parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopRight.FlipHorizontal = true;
		parts.Border.Corners.BottomLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomLeft.FlipVertical = true;
		parts.Border.Corners.BottomRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomRight.FlipHorizontal = true;
		parts.Border.Corners.BottomRight.FlipVertical = true;
			
		caption_part = {};
		caption_part.Base = caption_text;
		caption_part.Enabled = caption_style_enabled;
		caption_part.Disabled = caption_style_disabled;

		ion::gui::skins::gui_skin::SkinPart bar_part;
		bar_part.Enabled = progress_bar_bar_enabled;
		bar_part.FillColor.A(0.35_r);

		ion::gui::skins::gui_skin::SkinPart bar_interpolated_part;
		bar_interpolated_part.Enabled = progress_bar_bar_enabled;
		bar_interpolated_part.FillColor.A(0.65_r);

		auto progress_bar_skin = theme->CreateSkin<ion::gui::controls::GuiProgressBar>(parts, caption_part);
		progress_bar_skin->AddPart("bar", bar_part); //Additional
		progress_bar_skin->AddPart("bar-interpolated", bar_interpolated_part); //Additional

		//Radio button skin
		parts = {};
		parts.Center.Enabled = check_box_center_enabled;
		parts.Center.Pressed = check_box_center_enabled;
		parts.Center.Hovered = check_box_center_hovered;
		parts.Border.Sides.Top.Enabled = button_top_enabled;
		parts.Border.Sides.Top.Focused = button_top_focused;
		parts.Border.Sides.Bottom.Enabled = button_top_enabled;
		parts.Border.Sides.Bottom.Focused = button_top_focused;
		parts.Border.Sides.Bottom.FlipVertical = true;
		parts.Border.Sides.Left.Enabled = button_left_enabled;
		parts.Border.Sides.Left.Focused = button_left_focused;
		parts.Border.Sides.Right.Enabled = button_left_enabled;
		parts.Border.Sides.Right.Focused = button_left_focused;
		parts.Border.Sides.Right.FlipHorizontal = true;
		parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopRight.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.FlipHorizontal = true;
		parts.Border.Corners.BottomLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomLeft.Focused = button_top_left_focused;
		parts.Border.Corners.BottomLeft.FlipVertical = true;
		parts.Border.Corners.BottomRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomRight.Focused = button_top_left_focused;
		parts.Border.Corners.BottomRight.FlipHorizontal = true;
		parts.Border.Corners.BottomRight.FlipVertical = true;
			
		caption_part = {};
		caption_part.Base = caption_text;
		caption_part.Enabled = caption_style_enabled;
		caption_part.Disabled = caption_style_disabled;

		sound_parts = {};
		sound_parts.Clicked.Base = click;

		check_mark_part = {};
		check_mark_part.Enabled = radio_button_select_enabled;
		check_mark_part.Disabled = radio_button_select_disabled;
		check_mark_part.Pressed = radio_button_select_pressed;
		check_mark_part.Hovered = radio_button_select_hovered;
		check_mark_part.Scaling = 0.6_r;

		auto radio_button_skin = theme->CreateSkin<ion::gui::controls::GuiRadioButton>(parts, caption_part, sound_parts);
		radio_button_skin->AddPart("check-mark", check_mark_part); //Additional

		//Slider skin
		parts = {};
		parts.Center.Enabled = check_box_center_enabled;
		parts.Center.Disabled = check_box_center_enabled;
		parts.Center.Pressed = check_box_center_enabled;
		parts.Center.Hovered = check_box_center_hovered;
		parts.Border.Sides.Top.Enabled = button_top_enabled;
		parts.Border.Sides.Top.Focused = button_top_focused;
		parts.Border.Sides.Bottom.Enabled = button_top_enabled;
		parts.Border.Sides.Bottom.Focused = button_top_focused;
		parts.Border.Sides.Bottom.FlipVertical = true;
		parts.Border.Sides.Left.Enabled = button_left_enabled;
		parts.Border.Sides.Left.Focused = button_left_focused;
		parts.Border.Sides.Right.Enabled = button_left_enabled;
		parts.Border.Sides.Right.Focused = button_left_focused;
		parts.Border.Sides.Right.FlipHorizontal = true;
		parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopRight.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.FlipHorizontal = true;
		parts.Border.Corners.BottomLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomLeft.Focused = button_top_left_focused;
		parts.Border.Corners.BottomLeft.FlipVertical = true;
		parts.Border.Corners.BottomRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomRight.Focused = button_top_left_focused;
		parts.Border.Corners.BottomRight.FlipHorizontal = true;
		parts.Border.Corners.BottomRight.FlipVertical = true;
			
		caption_part = {};
		caption_part.Base = caption_text;
		caption_part.Enabled = caption_style_enabled;
		caption_part.Disabled = caption_style_disabled;

		sound_parts = {};
		sound_parts.Changed.Base = click;

		ion::gui::skins::gui_skin::SkinPart handle_part;
		handle_part.Enabled = radio_button_select_enabled;
		handle_part.Disabled = radio_button_select_disabled;
		handle_part.Pressed = radio_button_select_pressed;
		handle_part.Hovered = radio_button_select_hovered;
		handle_part.Scaling = {0.75_r, 1.5_r};

		auto slider_skin = theme->CreateSkin<ion::gui::controls::GuiSlider>(parts, caption_part, sound_parts);
		slider_skin->AddPart("handle", handle_part); //Additional

		//Scroll bar skin
		parts = {};
		parts.Center.Enabled = check_box_center_enabled;
		parts.Center.Disabled = check_box_center_enabled;
		parts.Center.Pressed = check_box_center_enabled;
		parts.Center.Hovered = check_box_center_hovered;
		parts.Border.Sides.Top.Enabled = button_top_enabled;
		parts.Border.Sides.Top.Focused = button_top_focused;
		parts.Border.Sides.Bottom.Enabled = button_top_enabled;
		parts.Border.Sides.Bottom.Focused = button_top_focused;
		parts.Border.Sides.Bottom.FlipVertical = true;
		parts.Border.Sides.Left.Enabled = button_left_enabled;
		parts.Border.Sides.Left.Focused = button_left_focused;
		parts.Border.Sides.Right.Enabled = button_left_enabled;
		parts.Border.Sides.Right.Focused = button_left_focused;
		parts.Border.Sides.Right.FlipHorizontal = true;
		parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopRight.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.FlipHorizontal = true;
		parts.Border.Corners.BottomLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomLeft.Focused = button_top_left_focused;
		parts.Border.Corners.BottomLeft.FlipVertical = true;
		parts.Border.Corners.BottomRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomRight.Focused = button_top_left_focused;
		parts.Border.Corners.BottomRight.FlipHorizontal = true;
		parts.Border.Corners.BottomRight.FlipVertical = true;
			
		caption_part = {};
		caption_part.Base = caption_text;
		caption_part.Enabled = caption_style_enabled;
		caption_part.Disabled = caption_style_disabled;

		handle_part = {};
		handle_part.Enabled = button_center_enabled;
		handle_part.Disabled = button_center_disabled;
		handle_part.Pressed = button_center_pressed;
		handle_part.Hovered = button_center_hovered;

		auto scroll_bar_skin = theme->CreateSkin<ion::gui::controls::GuiScrollBar>(parts, caption_part);
		scroll_bar_skin->AddPart("handle", handle_part); //Additional

		//Text box skin
		parts = {};
		parts.Center.Enabled = check_box_center_enabled;
		parts.Center.Disabled = check_box_center_enabled;
		parts.Center.Hovered = check_box_center_hovered;
		parts.Border.Sides.Top.Enabled = button_top_enabled;
		parts.Border.Sides.Top.Focused = button_top_focused;
		parts.Border.Sides.Bottom.Enabled = button_top_enabled;
		parts.Border.Sides.Bottom.Focused = button_top_focused;
		parts.Border.Sides.Bottom.FlipVertical = true;
		parts.Border.Sides.Left.Enabled = button_left_enabled;
		parts.Border.Sides.Left.Focused = button_left_focused;
		parts.Border.Sides.Right.Enabled = button_left_enabled;
		parts.Border.Sides.Right.Focused = button_left_focused;
		parts.Border.Sides.Right.FlipHorizontal = true;
		parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.TopRight.Focused = button_top_left_focused;
		parts.Border.Corners.TopRight.FlipHorizontal = true;
		parts.Border.Corners.BottomLeft.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomLeft.Focused = button_top_left_focused;
		parts.Border.Corners.BottomLeft.FlipVertical = true;
		parts.Border.Corners.BottomRight.Enabled = button_top_left_enabled;
		parts.Border.Corners.BottomRight.Focused = button_top_left_focused;
		parts.Border.Corners.BottomRight.FlipHorizontal = true;
		parts.Border.Corners.BottomRight.FlipVertical = true;
			
		caption_part = {};
		caption_part.Base = caption_text;
		caption_part.Enabled = caption_style_enabled;
		caption_part.Disabled = caption_style_disabled;

		ion::gui::skins::gui_skin::SkinPart cursor_part;
		cursor_part.Enabled = text_box_cursor_enabled;

		ion::gui::skins::gui_skin::SkinTextPart text_part;
		text_part.Base = caption_text;
		text_part.Enabled = caption_style_enabled;
		text_part.Disabled = caption_style_disabled;

		ion::gui::skins::gui_skin::SkinTextPart placeholder_text_part;
		placeholder_text_part.Base = caption_text;
		placeholder_text_part.Enabled = placeholder_text_style_enabled;
		placeholder_text_part.Disabled = placeholder_text_style_disabled;

		auto text_box_skin = theme->CreateSkin<ion::gui::controls::GuiTextBox>(parts, caption_part);
		text_box_skin->AddPart("cursor", cursor_part); //Additional
		text_box_skin->AddTextPart("text", text_part); //Additional
		text_box_skin->AddTextPart("placeholder-text", placeholder_text_part); //Additional


		/*
			GUI
		*/

		auto mouse_cursor = gui_controller.CreateMouseCursor("mouse_cursor", {});
		mouse_cursor->ZOrder(1.0_r);

		auto tooltip = gui_controller.CreateTooltip("tooltip", {});
		tooltip->ZOrder(0.9_r);

		auto main_frame = gui_controller.CreateFrame("main");
		auto base_panel = main_frame->CreatePanel("base");
		base_panel->ZOrder(0.1_r);

		auto slider = base_panel->CreateSlider("slider", Vector2{1.0_r, 0.077_r}, "My slider", "My slider tooltip");
		slider->Node()->Position({0.0_r, 0.6_r});
		slider->Range(0, 20);
		slider->StepByAmount(5);

		auto label = base_panel->CreateLabel("label", {}, "My label");
		label->Node()->Position({0.0_r, 0.5_r});
		label->Tooltip("My label tooltip");

		auto button = base_panel->CreateButton("button", Vector2{0.5_r, 0.11_r}, "My button", "My button tooltip");
		button->Node()->Position({0.0_r, 0.4_r});

		auto check_box = base_panel->CreateCheckBox("check_box", Vector2{0.077_r, 0.077_r}, "My check box", "My check box tooltip");
		check_box->Node()->Position({0.0_r, 0.3_r});

		auto group_box = base_panel->CreateGroupBox("group_box", Vector2{1.0_r, 0.15_r}, "My group box");
		group_box->Node()->Position({0.0_r, 0.1_r});
		group_box->Tooltip("My group box tooltip");

		auto radio_button = base_panel->CreateRadioButton("radio_button", Vector2{0.077_r, 0.077_r}, "My radio button", "My radio button tooltip");
		radio_button->Node()->Position({-0.05_r, 0.0_r});
		radio_button->CaptionLayout(ion::gui::controls::gui_control::ControlCaptionLayout::OutsideLeftCenter);

		auto radio_button2 = base_panel->CreateRadioButton("radio_button2", Vector2{0.077_r, 0.077_r}, "My radio button", "My radio button tooltip");
		radio_button2->Node()->Position({0.05_r, 0.0_r});

		group_box->AddControl(radio_button);
		group_box->AddControl(radio_button2);

		auto progress_bar = base_panel->CreateProgressBar("progress_bar", Vector2{1.0_r, 0.077_r}, "My progress bar");
		progress_bar->Node()->Position({0.0_r, -0.1_r});
		progress_bar->Tooltip("My progress bar tooltip");
		progress_bar->Range(0.0_r, 100.0_r);
		progress_bar->Position(75.0_r);

		auto list_box = base_panel->CreateListBox("list_box", Vector2{0.5_r, 0.5_r}, "My list box");
		list_box->Node()->Position({0.8_r, 0.25_r});
		list_box->Tooltip("My list box tooltip");
		list_box->ItemHeightFactor(3.5_r);
		list_box->IconPadding(10.0_r);
		list_box->ItemLayout(ion::gui::controls::gui_list_box::ListBoxItemLayout::Left);
		list_box->IconLayout(ion::gui::controls::gui_list_box::ListBoxIconLayout::Left);
		list_box->ShowIcons(true);
		list_box->AddItems({
			{"My <b>1st</b> <font color='purple'>item</font>"s, star},
			{"My <b>2nd</b> <font color='purple'>item</font>"s, star_red},
			{"My <b>3rd</b> <font color='purple'>item</font>"s, star_green},
			{"My <b>4th</b> <font color='purple'>item</font>"s, star_blue},
			{"My <b>5th</b> <font color='purple'>item</font>"s, star},
			{"My <b>6th</b> <font color='purple'>item</font>"s, star_blue},
			{"My <b>7th</b> <font color='purple'>item</font>"s, star_green},
			{"My <b>8th</b> <font color='purple'>item</font>"s, star_red},
			{"My <b>9th</b> <font color='purple'>item</font>"s, star}
		});

		auto scroll_bar = base_panel->CreateScrollBar("scroll_bar", Vector2{0.077_r, 0.5_r}, "My scroll bar");
		scroll_bar->Node()->Position({1.1_r, 0.25_r});
		scroll_bar->Tooltip("My scroll bar tooltip");
		scroll_bar->Range(0, 50);
		scroll_bar->StepByAmount(3);
		scroll_bar->AttachedScrollable(list_box);

		auto text_box = base_panel->CreateTextBox("text_box", Vector2{0.5_r, 0.11_r}, "My text box");		
		text_box->Node()->Position({0.0_r, -0.3_r});
		text_box->Tooltip("My text box tooltip");
		text_box->PlaceholderContent("<i><b>Type</b></i> something...");	
		text_box->TextLayout(ion::gui::controls::gui_text_box::TextBoxTextLayout::Left);
		text_box->TextMode(ion::gui::controls::gui_text_box::TextBoxTextMode::Printable);
		text_box->CharacterSet(ion::gui::controls::gui_text_box::TextBoxCharacterSet::ASCII);

		auto sub_panel = base_panel->CreatePanel("sub");
		sub_panel->ZOrder(0.1_r);
		auto &grid = sub_panel->GridLayout({3.54_r, 2.0_r}, 3, 3);
		auto &cell = grid[{2, 0}];
		cell.Alignment(ion::gui::gui_panel::GridCellAlignment::Left);
		cell.VerticalAlignment(ion::gui::gui_panel::GridCellVerticalAlignment::Bottom);
			
		auto label2 = sub_panel->CreateLabel("label", {}, "My label");		
		cell.AttachControl(label2);		
		label2->Tooltip("My label tooltip");

		auto main_frame2 = gui_controller.CreateFrame("main2");
		auto base_panel2 = main_frame2->CreatePanel("base");
		base_panel2->ZOrder(0.1_r);
		auto base_control2 = base_panel2->CreateControl<ion::gui::controls::GuiControl>("control");
		auto sub_panel2 = base_panel2->CreatePanel("sub");
		sub_panel2->ZOrder(0.1_r);
		auto sub_control2 = sub_panel2->CreateControl<ion::gui::controls::GuiControl>("control");

		main_frame->Activate();
		main_frame->Focus();
	}

	gui_controller.Visible(false);

	
	//Pointers
	auto camera = scene_manager->GetCamera("main_camera");
	auto player_camera = scene_manager->GetCamera("player_camera");

	if (viewport && camera)
		viewport->ConnectedCamera(camera);

	auto night_runner = sounds->GetSound("night_runner");
	auto red_lamp_flicker = scene_manager->GetSound("red_lamp_flicker");
	auto green_lamp_flicker = scene_manager->GetSound("green_lamp_flicker");
	auto fps = scene_manager->GetText("fps");
	auto intro_node = scene_graph->RootNode().GetChildNode("intro_node");
	auto level_node = scene_graph->RootNode().GetChildNode("level_node");
	auto player_node = level_node ? level_node->GetDescendantNode("player_node") : nullptr;
	auto ship_node = player_node ? player_node->GetChildNode("ship_node") : nullptr;
	auto light_node = ship_node ? ship_node->GetChildNode("ship_light_node") : nullptr;
	auto ship_idle_timeline = ship_node ? ship_node->GetTimeline("ship_idle_timeline") : nullptr;

	red_lamp_flicker->Get()->Pause();
	green_lamp_flicker->Get()->Pause();
	level_node->Visible(false);


	//Initialize game
	game.scene_graph = scene_graph;
	game.viewport = viewport;
	game.gui_controller = &gui_controller;
	game.sound_manager = sounds.get();
	game.night_runner = night_runner;
	game.red_lamp_flicker = red_lamp_flicker;
	game.green_lamp_flicker = green_lamp_flicker;
	game.fps = fps;
	game.intro_node = intro_node;
	game.level_node = level_node;
	game.player_node = player_node;
	game.light_node = light_node;
	game.camera = camera;
	game.player_camera = player_camera;
	game.ship_idle_timeline = ship_idle_timeline;

	return engine.Start();
}