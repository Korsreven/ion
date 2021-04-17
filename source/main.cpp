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

#include "IonEngine.h"

#include "adaptors/IonFlatMap.h"
#include "adaptors/IonFlatSet.h"
#include "adaptors/iterators/IonDereferenceIterator.h"
#include "adaptors/iterators/IonFlatMapIterator.h"
#include "adaptors/iterators/IonFlatSetIterator.h"
#include "adaptors/ranges/IonDereferenceIterable.h"
#include "adaptors/ranges/IonIterable.h"

#include "assets/IonAssetLoader.h"
#include "assets/repositories/IonAudioRepository.h"
#include "assets/repositories/IonFileRepository.h"
#include "assets/repositories/IonFontRepository.h"
#include "assets/repositories/IonImageRepository.h"
#include "assets/repositories/IonScriptRepository.h"
#include "assets/repositories/IonShaderRepository.h"
#include "assets/repositories/IonVideoRepository.h"

#include "events/IonCallback.h"
#include "events/IonEventChannel.h"
#include "events/IonEventGenerator.h"
#include "events/IonListenable.h"
#include "events/IonRecurringCallback.h"
#include "events/IonInputController.h"
#include "events/listeners/IonCameraListener.h"
#include "events/listeners/IonFrameListener.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonListener.h"
#include "events/listeners/IonMouseListener.h"
#include "events/listeners/IonRenderTargetListener.h"
#include "events/listeners/IonResourceListener.h"
#include "events/listeners/IonViewportListener.h"
#include "events/listeners/IonWindowListener.h"

#include "graphics/IonGraphicsAPI.h"
#include "graphics/fonts/IonFont.h"
#include "graphics/fonts/IonFontManager.h"
#include "graphics/fonts/IonText.h"
#include "graphics/fonts/IonTextManager.h"
#include "graphics/fonts/IonTypeFace.h"
#include "graphics/fonts/IonTypeFaceManager.h"
#include "graphics/fonts/utilities/IonFontUtility.h"
#include "graphics/materials/IonMaterial.h"
#include "graphics/materials/IonMaterialManager.h"
#include "graphics/particles/IonEmitter.h"
#include "graphics/particles/IonEmitterManager.h"
#include "graphics/particles/IonParticle.h"
#include "graphics/particles/IonParticleSystem.h"
#include "graphics/particles/IonParticleSystemManager.h"
#include "graphics/particles/affectors/IonAffector.h"
#include "graphics/particles/affectors/IonAffectorManager.h"
#include "graphics/particles/affectors/IonColorFader.h"
#include "graphics/particles/affectors/IonDirectionRandomizer.h"
#include "graphics/particles/affectors/IonGravitation.h"
#include "graphics/particles/affectors/IonLinearForce.h"
#include "graphics/particles/affectors/IonScaler.h"
#include "graphics/particles/affectors/IonSineForce.h"
#include "graphics/particles/affectors/IonVelocityRandomizer.h"
#include "graphics/render/IonFrustum.h"
#include "graphics/render/IonRenderTarget.h"
#include "graphics/render/IonRenderWindow.h"
#include "graphics/render/IonViewport.h"
#include "graphics/render/vertex/IonVertexArrayObject.h"
#include "graphics/render/vertex/IonVertexBatch.h"
#include "graphics/render/vertex/IonVertexBufferObject.h"
#include "graphics/render/vertex/IonVertexBufferView.h"
#include "graphics/render/vertex/IonVertexDataView.h"
#include "graphics/render/vertex/IonVertexDeclaration.h"
#include "graphics/scene/IonCamera.h"
#include "graphics/scene/IonLight.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonMovableAnimation.h"
#include "graphics/scene/IonMovableObject.h"
#include "graphics/scene/IonMovableParticleSystem.h"
#include "graphics/scene/IonMovableText.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/shapes/IonBorder.h"
#include "graphics/scene/shapes/IonCurve.h"
#include "graphics/scene/shapes/IonEllipse.h"
#include "graphics/scene/shapes/IonLine.h"
#include "graphics/scene/shapes/IonMesh.h"
#include "graphics/scene/shapes/IonRectangle.h"
#include "graphics/scene/shapes/IonShape.h"
#include "graphics/scene/shapes/IonSprite.h"
#include "graphics/scene/shapes/IonTriangle.h"
#include "graphics/shaders/IonShader.h"
#include "graphics/shaders/IonShaderLayout.h"
#include "graphics/shaders/IonShaderManager.h"
#include "graphics/shaders/IonShaderProgram.h"
#include "graphics/shaders/IonShaderProgramManager.h"
#include "graphics/shaders/variables/IonShaderAttribute.h"
#include "graphics/shaders/variables/IonShaderTypes.h"
#include "graphics/shaders/variables/IonShaderUniform.h"
#include "graphics/shaders/variables/IonShaderVariable.h"
#include "graphics/textures/IonAnimation.h"
#include "graphics/textures/IonAnimationManager.h"
#include "graphics/textures/IonFrameSequence.h"
#include "graphics/textures/IonFrameSequenceManager.h"
#include "graphics/textures/IonTexture.h"
#include "graphics/textures/IonTextureManager.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonMatrix3.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"

#include "managed/IonManagedObject.h"
#include "managed/IonObjectManager.h"
#include "managed/IonObjectObserver.h"
#include "managed/IonObservedObject.h"

#include "memory/IonNonOwningPtr.h"
#include "memory/IonOwningPtr.h"

#include "parallel/IonWorker.h"
#include "parallel/IonWorkerPool.h"

#include "resources/IonResource.h"
#include "resources/IonResourceManager.h"
#include "resources/IonFileResource.h"

#include "script/IonScriptBuilder.h"
#include "script/IonScriptCompiler.h"
#include "script/IonScriptError.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptTypes.h"
#include "script/IonScriptValidator.h"
#include "script/utilities/IonParseUtility.h"

#include "system/IonSystemAPI.h"
#include "system/IonSystemUtility.h"
#include "system/IonSystemWindow.h"
#include "system/events/IonSystemInput.h"
#include "system/events/listeners/IonSystemInputListener.h"
#include "system/events/listeners/IonSystemMessageListener.h"

#include "timers/IonAsyncTimerManager.h"
#include "timers/IonTimer.h"
#include "timers/IonTimerManager.h"
#include "timers/IonStopwatch.h"

#include "types/IonCumulative.h"
#include "types/IonProgress.h"
#include "types/IonSingleton.h"
#include "types/IonStrongType.h"
#include "types/IonTypes.h"
#include "types/IonTypeCasts.h"
#include "types/IonTypeTraits.h"

#include "unmanaged/IonObjectFactory.h"

#include "utilities/IonCodec.h"
#include "utilities/IonConvert.h"
#include "utilities/IonCrypto.h"
#include "utilities/IonFileUtility.h"
#include "utilities/IonMath.h"
#include "utilities/IonRandom.h"
#include "utilities/IonStringUtility.h"


using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ion::types::type_literals;
using namespace ion::graphics::utilities::color::literals;
using namespace ion::graphics::utilities::vector2::literals;
using namespace ion::utilities::file::literals;
using namespace ion::utilities::math::literals;


struct Game :
	ion::events::listeners::FrameListener,
	ion::events::listeners::WindowListener,
	ion::events::listeners::KeyListener,
	ion::events::listeners::MouseListener
{
	std::vector<ion::NonOwningPtr<ion::graphics::scene::Model>> models;
	std::vector<ion::NonOwningPtr<ion::graphics::scene::MovableAnimation>> animations;
	std::vector<ion::NonOwningPtr<ion::graphics::scene::MovableParticleSystem>> particle_systems;
	std::vector<ion::NonOwningPtr<ion::graphics::scene::MovableText>> texts;
	ion::NonOwningPtr<ion::graphics::render::Viewport> viewport;

	ion::NonOwningPtr<ion::graphics::shaders::variables::Uniform<ion::graphics::shaders::variables::glsl::mat4>> matrix_projection;
	ion::NonOwningPtr<ion::graphics::shaders::variables::Uniform<ion::graphics::shaders::variables::glsl::mat4>> matrix_model_view_projection;

	ion::NonOwningPtr<ion::graphics::shaders::variables::Uniform<int>> light_type;	
	ion::NonOwningPtr<ion::graphics::shaders::variables::Uniform<ion::graphics::shaders::variables::glsl::vec3>> light_position;
	ion::NonOwningPtr<ion::graphics::shaders::variables::Uniform<ion::graphics::shaders::variables::glsl::vec3>> light_direction;
	ion::NonOwningPtr<ion::graphics::shaders::variables::Uniform<ion::graphics::shaders::variables::glsl::vec4>> light_diffuse;

	/*
		Frame listener
	*/

	bool FrameStarted(duration time) noexcept override
	{
		for (auto &model : models)
		{
			model->Elapse(time);
			model->Prepare();			
		}

		for (auto &animation : animations)
		{
			animation->Elapse(time);	
			animation->Prepare();			
		}

		for (auto &particle_system : particle_systems)
		{
			particle_system->Elapse(time);	
			particle_system->Prepare();			
		}

		for (auto &text : texts)
		{
			text->Elapse(time);	
			text->Prepare();			
		}

		return true;
	}

	bool FrameEnded(duration time) noexcept override
	{
		time;
		return true;
	}


	/*
		Window listener
	*/

	void WindowActionReceived(ion::events::listeners::WindowAction action) noexcept override
	{
		if (action == ion::events::listeners::WindowAction::Resize)
		{
			if (viewport)
			{
				viewport->RenderTo();

				auto camera = viewport->ConnectedCamera();
				auto &proj_mat = camera->ViewFrustum().ProjectionMatrix();
				auto &view_mat = camera->ViewMatrix();

				matrix_projection->Get() = proj_mat.TransposeCopy();		
				matrix_model_view_projection->Get() = (proj_mat * view_mat).TransposeCopy();
			}
		}
	}


	/*
		Key listener
	*/

	void KeyPressed(ion::events::listeners::KeyButton button) noexcept override
	{
		button;
	}

	void KeyReleased(ion::events::listeners::KeyButton button) noexcept override
	{
		button;
  	}

	void CharacterPressed(char character) noexcept override
	{
		character;
	}


	/*
		Mouse listener
	*/

	void MousePressed(ion::events::listeners::MouseButton button, ion::graphics::utilities::Vector2 position) noexcept override
	{
		button;
		position;
	}

	void MouseReleased(ion::events::listeners::MouseButton button, ion::graphics::utilities::Vector2 position) noexcept override
	{
		static auto left_click_count = 0;
		static auto right_click_count = 0;
		position;

		if (button == ion::events::listeners::MouseButton::Left)
		{
			if (light_type)
				light_type->Get() = ++left_click_count % 3;
		}
		else if (button == ion::events::listeners::MouseButton::Right)
		{
			if (light_diffuse)
			{
				switch (++right_click_count % 4)
				{
					case 0: light_diffuse->Get() = ion::graphics::utilities::color::White; break;
					case 1: light_diffuse->Get() = ion::graphics::utilities::color::Red; break;
					case 2: light_diffuse->Get() = ion::graphics::utilities::color::Green; break;
					case 3: light_diffuse->Get() = ion::graphics::utilities::color::Blue; break;
				}
			}
		}
	}

	void MouseMoved(ion::graphics::utilities::Vector2 position) noexcept override
	{
		if (light_position)
		{
			auto [x, y] = position.XY();
			light_position->Get() = ion::graphics::utilities::Vector3{x, y, -1.0_r};

			if (light_direction)
				light_direction->Get() = (ion::graphics::utilities::Vector3{0.0, 0.0, -1.0_r} -
										  ion::graphics::utilities::Vector3{x, y, 0.0_r}).NormalizeCopy();
		}
	}

	void MouseWheelRolled(int delta, ion::graphics::utilities::Vector2 position) noexcept override
	{
		delta;
		position;
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
	/*
		Test code
	*/

	Game game;

	auto exit_code = 0;
	{	
		ion::Engine engine;

		auto &window = engine.RenderTo(
			ion::graphics::render::RenderWindow::Resizable("ION engine", {1280.0_r, 720.0_r}),
			ion::graphics::utilities::Aabb{-1.0_r, 1.0_r}, 1.0_r, 100.0_r, 16.0_r / 9.0_r);
		window.MinSize(ion::graphics::utilities::Vector2{640.0_r, 360.0_r});

		if (engine.Initialize())
		{
			//Check API support
			{
				[[maybe_unused]] auto newest_gl = ion::graphics::gl::HasGL(ion::graphics::gl::Version::v4_6);
				[[maybe_unused]] auto blend_func_separate = ion::graphics::gl::BlendFuncSeparate_Support();
				[[maybe_unused]] auto frame_buffer_object = ion::graphics::gl::FrameBufferObject_Support();
				[[maybe_unused]] auto multi_texture = ion::graphics::gl::MultiTexture_Support();
				[[maybe_unused]] auto point_sprite = ion::graphics::gl::PointSprite_Support();
				[[maybe_unused]] auto shader = ion::graphics::gl::Shader_Support();
				[[maybe_unused]] auto npot = ion::graphics::gl::TextureNonPowerOfTwo_Support();
				[[maybe_unused]] auto vertex_array_object = ion::graphics::gl::VertexArrayObject_Support();
				[[maybe_unused]] auto vertex_buffer_object = ion::graphics::gl::VertexBufferObject_Support();
				[[maybe_unused]] auto max_fragment_uniform_components = ion::graphics::gl::MaxFragmentUniformComponents();
				[[maybe_unused]] auto max_vertex_uniform_components = ion::graphics::gl::MaxVertexUniformComponents();
				[[maybe_unused]] auto max_texture_size = ion::graphics::gl::MaxTextureSize();
				[[maybe_unused]] auto max_texture_units = ion::graphics::gl::MaxTextureUnits();
				[[maybe_unused]] auto break_point = false;
			}


			//EXAMPLE begin

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


			ion::types::Progress<int> progress;

			//Textures
			ion::graphics::textures::TextureManager textures;
			textures.CreateRepository(std::move(image_repository));

			auto asteroid_diffuse = textures.CreateTexture("asteroid_diffuse", "asteroid_diffuse.png");
			auto asteroid_normal = textures.CreateTexture("asteroid_normal", "asteroid_normal.png");
			auto asteroid_specular = textures.CreateTexture("asteroid_specular", "asteroid_specular.png");
			
			auto brick_wall_diffuse = textures.CreateTexture("brick_wall_diffuse", "brick_wall_diffuse.jpg");
			auto brick_wall_normal = textures.CreateTexture("brick_wall_normal", "brick_wall_normal.jpg");
			auto brick_wall_specular = textures.CreateTexture("brick_wall_specular", "brick_wall_specular.jpg");
			
			auto pebbles_diffuse = textures.CreateTexture("pebbles_diffuse", "pebbles_diffuse.jpg");
			auto pebbles_normal = textures.CreateTexture("pebbles_normal", "pebbles_normal.jpg");
			auto pebbles_specular = textures.CreateTexture("pebbles_specular", "pebbles_specular.jpg");

			auto tifa_diffuse = textures.CreateTexture("tifa", "tifa.png");
			auto cat_first_frame = textures.CreateTexture("cat01", "cat01.png");
			textures.CreateTexture("cat02", "cat02.png");
			textures.CreateTexture("cat03", "cat03.png");
			textures.CreateTexture("cat04", "cat04.png");
			textures.CreateTexture("cat05", "cat05.png");
			textures.CreateTexture("cat06", "cat06.png");
			textures.CreateTexture("cat07", "cat07.png");
			textures.CreateTexture("cat08", "cat08.png");

			textures.LoadAll(/*ion::resources::resource_manager::EvaluationStrategy::Lazy*/);

			//while (!textures.Loaded());

			//Frame sequences
			ion::graphics::textures::FrameSequenceManager frame_sequences;
			auto cat_sequence = frame_sequences.CreateFrameSequence("cat_sequence", cat_first_frame, 8);

			//Animation
			ion::graphics::textures::AnimationManager animations;
			auto cat_running = animations.CreateAnimation(
				ion::graphics::textures::Animation::Looping("cat_running", cat_sequence, 0.8_sec));

			//Shaders
			ion::graphics::shaders::ShaderManager shaders;
			shaders.CreateRepository(std::move(shader_repository));
			shaders.LogLevel(ion::graphics::shaders::shader_manager::InfoLogLevel::Error);		
			auto mesh_vert_shader = shaders.CreateShader("default_mesh_vert", "default_mesh.vert");
			auto mesh_frag_shader = shaders.CreateShader("default_mesh_frag", "default_mesh.frag");
			//auto particle_vert_shader = shaders.CreateShader("default_particle_vert", "default_particle.vert");
			//auto particle_frag_shader = shaders.CreateShader("default_particle_frag", "default_particle.frag");
			//auto text_vert_shader = shaders.CreateShader("default_text_vert", "default_text.vert");
			//auto text_frag_shader = shaders.CreateShader("default_text_frag", "default_text.frag");
			//auto gui_text_vert_shader = shaders.CreateShader("default_gui_text_vert", "default_gui_text.vert");
			//auto gui_text_frag_shader = shaders.CreateShader("default_gui_text_frag", "default_gui_text.frag");
			shaders.LoadAll(/*ion::resources::resource_manager::EvaluationStrategy::Lazy*/);

			//while (!shaders.Loaded());

			//Shader programs
			ion::graphics::shaders::ShaderProgramManager shader_programs;
			shader_programs.LogLevel(ion::graphics::shaders::shader_program_manager::InfoLogLevel::Error);
			auto shader_prog = shader_programs.CreateShaderProgram("default_mesh_prog", mesh_vert_shader, mesh_frag_shader);
			//auto shader_prog = shader_programs.CreateShaderProgram("default_particle_prog", particle_vert_shader, particle_frag_shader);
			//auto shader_prog = shader_programs.CreateShaderProgram("default_text_prog", text_vert_shader, text_frag_shader);
			//auto shader_prog = shader_programs.CreateShaderProgram("default_gui_text_prog", gui_text_vert_shader, gui_text_frag_shader);
			shader_programs.LoadAll(/*ion::resources::resource_manager::EvaluationStrategy::Lazy*/);

			//while (!shader_programs.Loaded());

			using namespace ion::graphics::shaders::variables;

			//Shader structs
			auto matrix_struct = shader_prog->CreateStruct("matrix");
			auto scene_struct = shader_prog->CreateStruct("scene");
			auto camera_struct = shader_prog->CreateStruct("camera");
			auto primitive_struct = shader_prog->CreateStruct("primitive");
			auto material_struct = shader_prog->CreateStruct("material");
			auto light_struct = shader_prog->CreateStruct("light", 8);

			//Shader variables
			//Vertex
			shader_prog->CreateAttribute<glsl::vec3>("vertex_position");
			shader_prog->CreateAttribute<glsl::vec3>("vertex_normal");
			shader_prog->CreateAttribute<glsl::vec4>("vertex_color");
			shader_prog->CreateAttribute<glsl::vec2>("vertex_tex_coord");

			//shader_prog->CreateAttribute<glsl::vec3>("vertex_position");
			//shader_prog->CreateAttribute<float>("vertex_point_size");
			//shader_prog->CreateAttribute<glsl::vec4>("vertex_color");

			//shader_prog->CreateAttribute<glsl::vec3>("vertex_position");
			//shader_prog->CreateAttribute<glsl::vec4>("vertex_color");
			//shader_prog->CreateAttribute<glsl::vec2>("vertex_tex_coord");

			//Matrices			
			auto matrix_model_view = matrix_struct->CreateUniform<glsl::mat4>("model_view");
			auto matrix_projection = matrix_struct->CreateUniform<glsl::mat4>("projection");
			auto matrix_model_view_projection = matrix_struct->CreateUniform<glsl::mat4>("model_view_projection");

			//Scene
			auto scene_ambient = scene_struct->CreateUniform<glsl::vec4>("ambient");
			auto scene_gamma = scene_struct->CreateUniform<float>("gamma");
			auto scene_light_count = scene_struct->CreateUniform<int>("light_count");

			//Camera
			auto camera_position = camera_struct->CreateUniform<glsl::vec3>("position");	

			//Primitive
			auto primitive_texture = primitive_struct->CreateUniform<glsl::sampler2D>("texture");
			auto primitive_has_texture = primitive_struct->CreateUniform<bool>("has_texture");
			auto primitive_has_material = primitive_struct->CreateUniform<bool>("has_material");

			//Material			
			auto material_ambient = material_struct->CreateUniform<glsl::vec4>("ambient");
			auto material_diffuse = material_struct->CreateUniform<glsl::vec4>("diffuse");
			auto material_specular = material_struct->CreateUniform<glsl::vec4>("specular");
			auto material_emissive = material_struct->CreateUniform<glsl::vec4>("emissive");
			auto material_shininess = material_struct->CreateUniform<float>("shininess");
			auto material_diffuse_map = material_struct->CreateUniform<glsl::sampler2D>("diffuse_map");	
			auto material_specular_map = material_struct->CreateUniform<glsl::sampler2D>("specular_map");
			auto material_normal_map = material_struct->CreateUniform<glsl::sampler2D>("normal_map");
			auto material_has_diffuse_map = material_struct->CreateUniform<bool>("has_diffuse_map");
			auto material_has_specular_map = material_struct->CreateUniform<bool>("has_specular_map");
			auto material_has_normal_map = material_struct->CreateUniform<bool>("has_normal_map");

			//Light
			auto light_type = light_struct->CreateUniform<int>("type");
			auto light_position = light_struct->CreateUniform<glsl::vec3>("position");
			auto light_direction = light_struct->CreateUniform<glsl::vec3>("direction");
			auto light_ambient = light_struct->CreateUniform<glsl::vec4>("ambient");
			auto light_diffuse = light_struct->CreateUniform<glsl::vec4>("diffuse");
			auto light_specular = light_struct->CreateUniform<glsl::vec4>("specular");
			auto light_constant = light_struct->CreateUniform<float>("constant");
			auto light_linear = light_struct->CreateUniform<float>("linear");
			auto light_quadratic = light_struct->CreateUniform<float>("quadratic");
			auto light_cutoff = light_struct->CreateUniform<float>("cutoff");
			auto light_outer_cutoff = light_struct->CreateUniform<float>("outer_cutoff");

			shader_programs.LoadShaderVariableLocations(*shader_prog);


			//Font
			ion::graphics::fonts::FontManager fonts;
			fonts.CreateRepository(std::move(font_repository));
			auto verdana_regular_12 = fonts.CreateFont("verdana_regular_12", "verdana.ttf", 12);
			auto verdana_bold_12 = fonts.CreateFont("verdana_bold_12", "verdanab.ttf", 12);
			auto verdana_italic_12 = fonts.CreateFont("verdana_italic_12", "verdanai.ttf", 12);
			auto verdana_bold_italic_12 = fonts.CreateFont("verdana_bold_italic_12", "verdanaz.ttf", 12);
			fonts.LoadAll(/*ion::resources::resource_manager::EvaluationStrategy::Lazy*/);

			//while (!fonts.Loaded());

			//Type face
			ion::graphics::fonts::TypeFaceManager type_faces;
			[[maybe_unused]] auto verdana_12 = 
				type_faces.CreateTypeFace(
					"verdana_12",
					verdana_regular_12,
					verdana_bold_12,
					verdana_italic_12,
					verdana_bold_italic_12);

			//Material
			ion::graphics::materials::MaterialManager materials;
			auto asteroid =
				materials.CreateMaterial("asteroid",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{0.0_r, 0.0_r, 0.0_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r, asteroid_diffuse, asteroid_specular, asteroid_normal);

			auto brick =
				materials.CreateMaterial("brick",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{0.0_r, 0.0_r, 0.0_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r, brick_wall_diffuse, brick_wall_specular, brick_wall_normal);

			auto pebbles =
				materials.CreateMaterial("pebbles",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{0.6_r, 0.6_r, 0.6_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r, pebbles_diffuse, pebbles_specular, pebbles_normal);
			
			auto ruby =
				materials.CreateMaterial("ruby",
					{1.0_r, 0.0_r, 0.0_r},
					{1.0_r, 0.0_r, 0.0_r},
					{0.0_r, 0.0_r, 0.0_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r);

			auto emerald =
				materials.CreateMaterial("emerald",
					{0.0_r, 1.0_r, 0.0_r},
					{0.0_r, 1.0_r, 0.0_r},
					{0.0_r, 0.0_r, 0.0_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r);

			auto sapphire =
				materials.CreateMaterial("sapphire",
					{0.0_r, 0.0_r, 1.0_r},
					{0.0_r, 0.0_r, 1.0_r},
					{0.0_r, 0.0_r, 0.0_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r);

			auto tifa =
				materials.CreateMaterial("tifa",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{0.6_r, 0.6_r, 0.6_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r, tifa_diffuse, nullptr, nullptr);

			auto cat =
				materials.CreateMaterial("cat",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{0.6_r, 0.6_r, 0.6_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r, cat_running, nullptr, nullptr);

			using namespace ion::graphics::utilities;

			//Particle system
			ion::graphics::particles::ParticleSystemManager particle_systems;
			auto particle_system = particle_systems.CreateParticleSystem("asteroids");

			auto emitter = particle_system->CreateEmitter(
				ion::graphics::particles::Emitter::Point(
					"spawner", {-0.75_r, 0.5_r, -1.3_r}, {-0.5_r, -0.5_r}, 2.5_r, ion::utilities::math::ToRadians(2.5_r), {}, 25
				));

			emitter->ParticleVelocity(0.4_r, 0.6_r);
			emitter->ParticleSize(24.0_r, 32.0_r);
			emitter->ParticleMass(0.4_r, 0.6_r);
			emitter->ParticleColor(color::White);
			emitter->ParticleLifeTime(50.0_sec, 60.0_sec);
			emitter->ParticleMaterial(asteroid);

			particle_system->CreateAffector(
				ion::graphics::particles::affectors::Gravitation{"gravity", {0.0_r, 0.0_r}, 2.0_r, 0.5_r});

			//Text
			ion::graphics::fonts::TextManager texts;
			[[maybe_unused]] auto text =
				texts.CreateText(
					"pangram",
					"The <i>quick</i> <del><font color='saddlebrown'>brown</font></del> fox <b>jumps</b> <sup>over</sup> the <i>lazy</i> dog",
					verdana_12);

			text->AppendLine("How <del><font color='olivedrab'>vexingly</font></del> <u>quick</u> <sub>daft</sub> zebras <b>jump</b>!");
			//text->AreaSize(ion::graphics::utilities::Vector2{300.0_r, 100.0_r});
			text->Alignment(ion::graphics::fonts::text::TextAlignment::Left);
			text->VerticalAlignment(ion::graphics::fonts::text::TextVerticalAlignment::Top);
			text->DefaultForegroundColor(color::White);

			//Mesh vertices
			/*ion::graphics::scene::shapes::mesh::Vertices gray_vertices;
			gray_vertices.push_back({{-1.7778_r, 1.0_r, -4.0_r}, vector3::UnitZ, color::LightGray});
			gray_vertices.push_back({{-1.7778_r, -1.0_r, -4.0_r}, vector3::UnitZ, color::LightGray});
			gray_vertices.push_back({{1.7778_r, -1.0_r, -4.0_r}, vector3::UnitZ, color::LightGray});
			gray_vertices.push_back({{1.7778_r, -1.0_r, -4.0_r}, vector3::UnitZ, color::LightGray});
			gray_vertices.push_back({{1.7778_r, 1.0_r, -4.0_r}, vector3::UnitZ, color::LightGray});
			gray_vertices.push_back({{-1.7778_r, 1.0_r, -4.0_r}, vector3::UnitZ, color::LightGray});

			ion::graphics::scene::shapes::mesh::Vertices red_vertices;
			red_vertices.push_back({{-1.7778_r, 1.0_r, -1.25_r}, vector3::UnitZ, color::Red});
			red_vertices.push_back({{-1.7778_r, 0.8_r, -1.25_r}, vector3::UnitZ, color::Red});
			red_vertices.push_back({{-1.57788_r, 0.8_r, -1.25_r}, vector3::UnitZ, color::Red});
			red_vertices.push_back({{-1.57788_r, 0.8_r, -1.25_r}, vector3::UnitZ, color::Red});
			red_vertices.push_back({{-1.5778_r, 1.0_r, -1.25_r}, vector3::UnitZ, color::Red});
			red_vertices.push_back({{-1.7778_r, 1.0_r, -1.25_r}, vector3::UnitZ, color::Red});

			ion::graphics::scene::shapes::mesh::Vertices green_vertices;
			green_vertices.push_back({{-0.1_r, 0.1_r, -1.5_r}, vector3::UnitZ, color::Green});
			green_vertices.push_back({{-0.1_r, -0.1_r, -1.5_r}, vector3::UnitZ, color::Green});
			green_vertices.push_back({{0.1_r, -0.1_r, -1.5_r}, vector3::UnitZ, color::Green});
			green_vertices.push_back({{0.1_r, -0.1_r, -1.5_r}, vector3::UnitZ, color::Green});
			green_vertices.push_back({{0.1_r, 0.1_r, -1.5_r}, vector3::UnitZ, color::Green});
			green_vertices.push_back({{-0.1_r, 0.1_r, -1.5_r}, vector3::UnitZ, color::Green});

			ion::graphics::scene::shapes::mesh::Vertices blue_vertices;
			blue_vertices.push_back({{1.5778_r, -0.8_r, -1.25_r}, vector3::UnitZ, color::Blue});
			blue_vertices.push_back({{1.5778_r, -1.0_r, -1.25_r}, vector3::UnitZ, color::Blue});
			blue_vertices.push_back({{1.7778_r, -1.0_r, -1.25_r}, vector3::UnitZ, color::Blue});
			blue_vertices.push_back({{1.7778_r, -1.0_r, -1.25_r}, vector3::UnitZ, color::Blue});
			blue_vertices.push_back({{1.7778_r, -0.8_r, -1.25_r}, vector3::UnitZ, color::Blue});
			blue_vertices.push_back({{1.5778_r, -0.8_r, -1.25_r}, vector3::UnitZ, color::Blue});*/

			Vector3 scale_factor{0.001953125_r, 0.001953125_r, 1.0_r};

			ion::graphics::scene::shapes::mesh::Vertices sprite_vertices;
			sprite_vertices.push_back({{-0.38867_r, 1.0_r, -1.3_r}, vector3::UnitZ, {0.0_r, 1.0_r}, color::White});
			sprite_vertices.push_back({{-0.38867_r, -1.0_r, -1.3_r}, vector3::UnitZ, {0.0_r, 0.0_r}, color::White});
			sprite_vertices.push_back({{0.38867_r, -1.0_r, -1.3_r}, vector3::UnitZ, {1.0_r, 0.0_r}, color::White});
			sprite_vertices.push_back({{0.38867_r, -1.0_r, -1.3_r}, vector3::UnitZ, {1.0_r, 0.0_r}, color::White});
			sprite_vertices.push_back({{0.38867_r, 1.0_r, -1.3_r}, vector3::UnitZ, {1.0_r, 1.0_r}, color::White});
			sprite_vertices.push_back({{-0.38867_r, 1.0_r, -1.3_r}, vector3::UnitZ, {0.0_r, 1.0_r}, color::White});


			ion::graphics::scene::shapes::mesh::Vertices head_vertices_crop; //140x140
			Vector3 head_pos_crop{-47.0_r, 440.0_r, 0.0_r};
			head_pos_crop *= scale_factor;
			head_vertices_crop.push_back({Vector3{-0.1367_r, 0.1367_r, -1.3_r} + head_pos_crop, vector3::UnitZ, color::Red});
			head_vertices_crop.push_back({Vector3{-0.1367_r, -0.1367_r, -1.3_r} + head_pos_crop, vector3::UnitZ, color::Red});
			head_vertices_crop.push_back({Vector3{0.1367_r, -0.1367_r, -1.3_r} + head_pos_crop, vector3::UnitZ, color::Red});
			head_vertices_crop.push_back({Vector3{0.1367_r, -0.1367_r, -1.3_r} + head_pos_crop, vector3::UnitZ, color::Red});
			head_vertices_crop.push_back({Vector3{0.1367_r, 0.1367_r, -1.3_r} + head_pos_crop, vector3::UnitZ, color::Red});
			head_vertices_crop.push_back({Vector3{-0.1367_r, 0.1367_r, -1.3_r} + head_pos_crop, vector3::UnitZ, color::Red});

			ion::graphics::scene::shapes::mesh::Vertices breast_vertices_crop; //160x115
			Vector3 breast_pos_crop{-56.0_r, 278.0_r, 0.0_r};
			breast_pos_crop *= scale_factor;
			breast_vertices_crop.push_back({Vector3{-0.1562_r, 0.1123_r, -1.3_r} + breast_pos_crop, vector3::UnitZ, color::Red});
			breast_vertices_crop.push_back({Vector3{-0.1562_r, -0.1123_r, -1.3_r} + breast_pos_crop, vector3::UnitZ, color::Red});
			breast_vertices_crop.push_back({Vector3{0.1562_r, -0.1123_r, -1.3_r} + breast_pos_crop, vector3::UnitZ, color::Red});
			breast_vertices_crop.push_back({Vector3{0.1562_r, -0.1123_r, -1.3_r} + breast_pos_crop, vector3::UnitZ, color::Red});
			breast_vertices_crop.push_back({Vector3{0.1562_r, 0.1123_r, -1.3_r} + breast_pos_crop, vector3::UnitZ, color::Red});
			breast_vertices_crop.push_back({Vector3{-0.1562_r, 0.1123_r, -1.3_r} + breast_pos_crop, vector3::UnitZ, color::Red});

			ion::graphics::scene::shapes::mesh::Vertices elbow_vertices_crop; //100x100
			Vector3 elbow_pos_crop{150.0_r, 253.0_r, 0.0_r};
			elbow_pos_crop *= scale_factor;
			elbow_vertices_crop.push_back({Vector3{-0.0976_r, 0.0976_r, -1.3_r} + elbow_pos_crop, vector3::UnitZ, color::Red});
			elbow_vertices_crop.push_back({Vector3{-0.0976_r, -0.0976_r, -1.3_r} + elbow_pos_crop, vector3::UnitZ, color::Red});
			elbow_vertices_crop.push_back({Vector3{0.0976_r, -0.0976_r, -1.3_r} + elbow_pos_crop, vector3::UnitZ, color::Red});
			elbow_vertices_crop.push_back({Vector3{0.0976_r, -0.0976_r, -1.3_r} + elbow_pos_crop, vector3::UnitZ, color::Red});
			elbow_vertices_crop.push_back({Vector3{0.0976_r, 0.0976_r, -1.3_r} + elbow_pos_crop, vector3::UnitZ, color::Red});
			elbow_vertices_crop.push_back({Vector3{-0.0976_r, 0.0976_r, -1.3_r} + elbow_pos_crop, vector3::UnitZ, color::Red});

			ion::graphics::scene::shapes::mesh::Vertices belly_vertices_crop; //70x70
			Vector3 belly_pos_crop{-66.0_r, 139.0_r, 0.0_r};
			belly_pos_crop *= scale_factor;
			belly_vertices_crop.push_back({Vector3{-0.0683_r, 0.0683_r, -1.3_r} + belly_pos_crop, vector3::UnitZ, color::Red});
			belly_vertices_crop.push_back({Vector3{-0.0683_r, -0.0683_r, -1.3_r} + belly_pos_crop, vector3::UnitZ, color::Red});
			belly_vertices_crop.push_back({Vector3{0.0683_r, -0.0683_r, -1.3_r} + belly_pos_crop, vector3::UnitZ, color::Red});
			belly_vertices_crop.push_back({Vector3{0.0683_r, -0.0683_r, -1.3_r} + belly_pos_crop, vector3::UnitZ, color::Red});
			belly_vertices_crop.push_back({Vector3{0.0683_r, 0.0683_r, -1.3_r} + belly_pos_crop, vector3::UnitZ, color::Red});
			belly_vertices_crop.push_back({Vector3{-0.0683_r, 0.0683_r, -1.3_r} + belly_pos_crop, vector3::UnitZ, color::Red});

			ion::graphics::scene::shapes::mesh::Vertices hand_vertices_crop; //70x140
			Vector3 hand_pos_crop{-165.0_r, 0.0_r, 0.0_r};
			hand_pos_crop *= scale_factor;
			hand_vertices_crop.push_back({Vector3{-0.0683_r, 0.1367_r, -1.3_r} + hand_pos_crop, vector3::UnitZ, color::Red});
			hand_vertices_crop.push_back({Vector3{-0.0683_r, -0.1367_r, -1.3_r} + hand_pos_crop, vector3::UnitZ, color::Red});
			hand_vertices_crop.push_back({Vector3{0.0683_r, -0.1367_r, -1.3_r} + hand_pos_crop, vector3::UnitZ, color::Red});
			hand_vertices_crop.push_back({Vector3{0.0683_r, -0.1367_r, -1.3_r} + hand_pos_crop, vector3::UnitZ, color::Red});
			hand_vertices_crop.push_back({Vector3{0.0683_r, 0.1367_r, -1.3_r} + hand_pos_crop, vector3::UnitZ, color::Red});
			hand_vertices_crop.push_back({Vector3{-0.0683_r, 0.1367_r, -1.3_r} + hand_pos_crop, vector3::UnitZ, color::Red});

			ion::graphics::scene::shapes::mesh::Vertices knees_vertices_crop; //170x100
			Vector3 knees_pos_crop{-65.0_r, -207.0_r, 0.0_r};
			knees_pos_crop *= scale_factor;
			knees_vertices_crop.push_back({Vector3{-0.166_r, 0.0976_r, -1.3_r} + knees_pos_crop, vector3::UnitZ, color::Red});
			knees_vertices_crop.push_back({Vector3{-0.166_r, -0.0976_r, -1.3_r} + knees_pos_crop, vector3::UnitZ, color::Red});
			knees_vertices_crop.push_back({Vector3{0.166_r, -0.0976_r, -1.3_r} + knees_pos_crop, vector3::UnitZ, color::Red});
			knees_vertices_crop.push_back({Vector3{0.166_r, -0.0976_r, -1.3_r} + knees_pos_crop, vector3::UnitZ, color::Red});
			knees_vertices_crop.push_back({Vector3{0.166_r, 0.0976_r, -1.3_r} + knees_pos_crop, vector3::UnitZ, color::Red});
			knees_vertices_crop.push_back({Vector3{-0.166_r, 0.0976_r, -1.3_r} + knees_pos_crop, vector3::UnitZ, color::Red});


			ion::graphics::scene::shapes::mesh::Vertices head_vertices; //140x140
			Vector3 head_pos{-547.0_r, 440.0_r, 0.0_r};
			head_pos *= scale_factor;
			head_vertices.push_back({Vector3{-0.1367_r, 0.1367_r, -1.3_r} + head_pos, vector3::UnitZ, {0.2035_r, 0.998_r}, color::White});
			head_vertices.push_back({Vector3{-0.1367_r, -0.1367_r, -1.3_r} + head_pos, vector3::UnitZ, {0.2035_r, 0.8623_r}, color::White});
			head_vertices.push_back({Vector3{0.1367_r, -0.1367_r, -1.3_r} + head_pos, vector3::UnitZ, {0.5552_r, 0.8623_r}, color::White});
			head_vertices.push_back({Vector3{0.1367_r, -0.1367_r, -1.3_r} + head_pos, vector3::UnitZ, {0.5552_r, 0.8623_r}, color::White});
			head_vertices.push_back({Vector3{0.1367_r, 0.1367_r, -1.3_r} + head_pos, vector3::UnitZ, {0.5552_r, 0.998_r}, color::White});
			head_vertices.push_back({Vector3{-0.1367_r, 0.1367_r, -1.3_r} + head_pos, vector3::UnitZ, {0.2035_r, 0.998_r}, color::White});

			ion::graphics::scene::shapes::mesh::Vertices breast_vertices; //160x115
			Vector3 breast_pos{-556.0_r, 278.0_r, 0.0_r};
			breast_pos *= scale_factor;
			breast_vertices.push_back({Vector3{-0.1562_r, 0.1123_r, -1.3_r} + breast_pos, vector3::UnitZ, {0.1582_r, 0.8281_r}, color::White});
			breast_vertices.push_back({Vector3{-0.1562_r, -0.1123_r, -1.3_r} + breast_pos, vector3::UnitZ, {0.1582_r, 0.7158_r}, color::White});
			breast_vertices.push_back({Vector3{0.1562_r, -0.1123_r, -1.3_r} + breast_pos, vector3::UnitZ, {0.5577_r, 0.7158_r}, color::White});
			breast_vertices.push_back({Vector3{0.1562_r, -0.1123_r, -1.3_r} + breast_pos, vector3::UnitZ, {0.5577_r, 0.7158_r}, color::White});
			breast_vertices.push_back({Vector3{0.1562_r, 0.1123_r, -1.3_r} + breast_pos, vector3::UnitZ, {0.5577_r, 0.8281_r}, color::White});
			breast_vertices.push_back({Vector3{-0.1562_r, 0.1123_r, -1.3_r} + breast_pos, vector3::UnitZ, {0.1582_r, 0.8281_r}, color::White});

			ion::graphics::scene::shapes::mesh::Vertices elbow_vertices; //100x100
			Vector3 elbow_pos{-350.0_r, 253.0_r, 0.0_r};
			elbow_pos *= scale_factor;
			elbow_vertices.push_back({Vector3{-0.0976_r, 0.0976_r, -1.3_r} + elbow_pos, vector3::UnitZ, {0.7512_r, 0.7958_r}, color::White});
			elbow_vertices.push_back({Vector3{-0.0976_r, -0.0976_r, -1.3_r} + elbow_pos, vector3::UnitZ, {0.7512_r, 0.6992_r}, color::White});
			elbow_vertices.push_back({Vector3{0.0976_r, -0.0976_r, -1.3_r} + elbow_pos, vector3::UnitZ, {1.0_r, 0.6992_r}, color::White});
			elbow_vertices.push_back({Vector3{0.0976_r, -0.0976_r, -1.3_r} + elbow_pos, vector3::UnitZ, {1.0_r, 0.6992_r}, color::White});
			elbow_vertices.push_back({Vector3{0.0976_r, 0.0976_r, -1.3_r} + elbow_pos, vector3::UnitZ, {1.0_r, 0.7958_r}, color::White});
			elbow_vertices.push_back({Vector3{-0.0976_r, 0.0976_r, -1.3_r} + elbow_pos, vector3::UnitZ, {0.7512_r, 0.7958_r}, color::White});

			ion::graphics::scene::shapes::mesh::Vertices belly_vertices; //70x70
			Vector3 belly_pos{-566.0_r, 139.0_r, 0.0_r};
			belly_pos *= scale_factor;
			belly_vertices.push_back({Vector3{-0.0683_r, 0.0683_r, -1.3_r} + belly_pos, vector3::UnitZ, {0.2462_r, 0.6699_r}, color::White});
			belly_vertices.push_back({Vector3{-0.0683_r, -0.0683_r, -1.3_r} + belly_pos, vector3::UnitZ, {0.2462_r, 0.6025_r}, color::White});
			belly_vertices.push_back({Vector3{0.0683_r, -0.0683_r, -1.3_r} + belly_pos, vector3::UnitZ, {0.4195_r, 0.6025_r}, color::White});
			belly_vertices.push_back({Vector3{0.0683_r, -0.0683_r, -1.3_r} + belly_pos, vector3::UnitZ, {0.4195_r, 0.6025_r}, color::White});
			belly_vertices.push_back({Vector3{0.0683_r, 0.0683_r, -1.3_r} + belly_pos, vector3::UnitZ, {0.4195_r, 0.6699_r}, color::White});
			belly_vertices.push_back({Vector3{-0.0683_r, 0.0683_r, -1.3_r} + belly_pos, vector3::UnitZ, {0.2462_r, 0.6699_r}, color::White});

			ion::graphics::scene::shapes::mesh::Vertices hand_vertices; //70x140
			Vector3 hand_pos{-665.0_r, 0.0_r, 0.0_r};
			hand_pos *= scale_factor;
			hand_vertices.push_back({Vector3{-0.0683_r, 0.1367_r, -1.3_r} + hand_pos, vector3::UnitZ, {0.0_r, 0.5683_r}, color::White});
			hand_vertices.push_back({Vector3{-0.0683_r, -0.1367_r, -1.3_r} + hand_pos, vector3::UnitZ, {0.0_r, 0.4316_r}, color::White});
			hand_vertices.push_back({Vector3{0.0683_r, -0.1367_r, -1.3_r} + hand_pos, vector3::UnitZ, {0.1708_r, 0.4316_r}, color::White});
			hand_vertices.push_back({Vector3{0.0683_r, -0.1367_r, -1.3_r} + hand_pos, vector3::UnitZ, {0.1708_r, 0.4316_r}, color::White});
			hand_vertices.push_back({Vector3{0.0683_r, 0.1367_r, -1.3_r} + hand_pos, vector3::UnitZ, {0.1708_r, 0.5683_r}, color::White});
			hand_vertices.push_back({Vector3{-0.0683_r, 0.1367_r, -1.3_r} + hand_pos, vector3::UnitZ, {0.0_r, 0.5683_r}, color::White});

			ion::graphics::scene::shapes::mesh::Vertices knees_vertices; //170x100
			Vector3 knees_pos{-565.0_r, -207.0_r, 0.0_r};
			knees_pos *= scale_factor;
			knees_vertices.push_back({Vector3{-0.166_r, 0.0976_r, -1.3_r} + knees_pos, vector3::UnitZ, {0.1231_r, 0.3466_r}, color::White});
			knees_vertices.push_back({Vector3{-0.166_r, -0.0976_r, -1.3_r} + knees_pos, vector3::UnitZ, {0.1231_r, 0.25_r}, color::White});
			knees_vertices.push_back({Vector3{0.166_r, -0.0976_r, -1.3_r} + knees_pos, vector3::UnitZ, {0.5477_r, 0.25_r}, color::White});
			knees_vertices.push_back({Vector3{0.166_r, -0.0976_r, -1.3_r} + knees_pos, vector3::UnitZ, {0.5477_r, 0.25_r}, color::White});
			knees_vertices.push_back({Vector3{0.166_r, 0.0976_r, -1.3_r} + knees_pos, vector3::UnitZ, {0.5477_r, 0.3466_r}, color::White});
			knees_vertices.push_back({Vector3{-0.166_r, 0.0976_r, -1.3_r} + knees_pos, vector3::UnitZ, {0.1231_r, 0.3466_r}, color::White});

			//Models
			//auto gray_rectangle = engine.Scene().CreateModel();
			//gray_rectangle->CreateMesh(std::move(gray_vertices));

			//auto red_square = engine.Scene().CreateModel();
			//red_square->CreateMesh(std::move(red_vertices), ruby);

			//auto green_square = engine.Scene().CreateModel();
			//green_square->CreateMesh(std::move(green_vertices), emerald);

			//auto blue_square = engine.Scene().CreateModel();
			//blue_square->CreateMesh(std::move(blue_vertices), sapphire);

			auto model = engine.Scene().CreateModel();
			model->CreateMesh(std::move(sprite_vertices), tifa,
				ion::graphics::scene::shapes::mesh::MeshTexCoordMode::Manual);
			model->CreateMesh(std::move(head_vertices_crop))->ShowWireframe(true);
			model->CreateMesh(std::move(breast_vertices_crop))->ShowWireframe(true);
			model->CreateMesh(std::move(elbow_vertices_crop))->ShowWireframe(true);
			model->CreateMesh(std::move(belly_vertices_crop))->ShowWireframe(true);
			model->CreateMesh(std::move(hand_vertices_crop))->ShowWireframe(true);
			model->CreateMesh(std::move(knees_vertices_crop))->ShowWireframe(true);

			model->CreateMesh(std::move(head_vertices), tifa,
				ion::graphics::scene::shapes::mesh::MeshTexCoordMode::Manual);
			model->CreateMesh(std::move(breast_vertices), tifa,
				ion::graphics::scene::shapes::mesh::MeshTexCoordMode::Manual);
			model->CreateMesh(std::move(elbow_vertices), tifa,
				ion::graphics::scene::shapes::mesh::MeshTexCoordMode::Manual);
			model->CreateMesh(std::move(belly_vertices), tifa,
				ion::graphics::scene::shapes::mesh::MeshTexCoordMode::Manual);
			model->CreateMesh(std::move(hand_vertices), tifa,
				ion::graphics::scene::shapes::mesh::MeshTexCoordMode::Manual);
			model->CreateMesh(std::move(knees_vertices), tifa,
				ion::graphics::scene::shapes::mesh::MeshTexCoordMode::Manual);

			/*auto triangle = model->CreateMesh(ion::graphics::scene::shapes::Triangle{
				{1.0_r, 0.8_r, -1.3_r}, {0.5_r, 0.4_r, -1.3_r}, {1.5_r, 0.4_r, -1.3_r}, color::DarkGreen});
			auto rectangle = model->CreateMesh(ion::graphics::scene::shapes::Rectangle{
				{1.0_r, 0.0_r, -1.3_r}, {0.6_r, 0.5_r}, color::Coral});
			auto ellipse = model->CreateMesh(ion::graphics::scene::shapes::Ellipse{
				{1.0_r, 0.0_r, -1.3_r}, {0.5_r, 0.4_r}, color::Crimson});
			auto line = model->CreateMesh(ion::graphics::scene::shapes::Line{
				{0.5_r, -0.4_r, -1.3_r}, {1.5_r, -0.4_r, -1.3_r}, color::Goldenrod, 2.0_r});
			auto curve = model->CreateMesh(ion::graphics::scene::shapes::Curve{
				{{0.5_r, 0.4_r, -1.3_r}, {1.0_r, 0.8_r, -1.3_r}, {1.5_r, 0.4_r, -1.3_r}}, color::Indigo, 2.0_r});*/

			auto sprite = model->CreateMesh(ion::graphics::scene::shapes::Sprite{
				{1.0_r, 0.1_r, -1.3_r}, {0.3886_r, 1.0_r}, tifa});
			sprite->Crop(Aabb{{0.0251_r, 0.3359_r}, {0.6884_r, 1.0_r}});
			sprite->FlipHorizontal();

			auto border = model->CreateMesh(ion::graphics::scene::shapes::Border{
				{1.0_r, 0.1_r, -1.3_r}, {0.3886_r, 1.0_r}, {0.02_r, 0.02_r},
				ion::graphics::scene::shapes::border::BorderCornerStyle::Oblique, color::DarkGray});

			/*model->CreateMesh(ion::graphics::scene::shapes::Sprite{
				{1.0_r, -0.7_r, -1.3_r}, {1.0_r, 0.5_r}, cat});*/

			//Animations
			//auto running = engine.Scene().CreateAnimation(
			//	{0.0_r, 0.0_r, -1.3_r}, {1.0_r, 0.5_r}, cat_running);
			//running->Get()->Start();

			//Particle systems
			//auto asteroids = engine.Scene().CreateParticleSystem(particle_system);
			//asteroids->Get()->StartAll();

			//Texts
			auto hello_world = engine.Scene().CreateText(
				{0.0_r, 0.0_r, -1.3_r}, ion::utilities::math::ToRadians(0.0_r), text);

			//Camera, projection and view matrix
			auto camera = engine.Target()->GetViewport("")->ConnectedCamera();
			camera->Position({0.0_r, 0.0_r, 0.0_r});
			camera->Rotation(ion::utilities::math::ToRadians(0.0_r));

			engine.Target()->GetViewport("")->RenderTo();
			auto &proj_mat = camera->ViewFrustum().ProjectionMatrix();
			auto &view_mat = camera->ViewMatrix();

			//Uniforms
			scene_ambient->Get() = Color{1.0_r, 1.0_r, 1.0_r, 1.0};
			scene_gamma->Get() = 1.0_r;
			scene_light_count->Get() = 1;

			camera_position->Get() = camera->Position();

			light_type->Get() = 0;
			light_position->Get().XYZ(0.0_r, 0.0_r, -1.0_r);
			light_direction->Get().XYZ(0.0_r, 0.0_r, -1.0_r);
			light_ambient->Get().XYZW(1.0_r, 1.0_r, 1.0_r, 1.0_r);
			light_diffuse->Get().XYZW(1.0_r, 1.0_r, 1.0_r, 1.0_r);
			light_specular->Get().XYZW(1.0_r, 1.0_r, 1.0_r, 1.0_r);
			light_constant->Get() = 1.0_r;
			light_linear->Get() = 0.09_r;
			light_quadratic->Get() = 0.032_r;
			light_cutoff->Get() = ion::utilities::math::Cos(ion::utilities::math::ToRadians(45.0_r));
			light_outer_cutoff->Get() = ion::utilities::math::Cos(ion::utilities::math::ToRadians(55.0_r));
			
			matrix_projection->Get() = proj_mat.TransposeCopy();
			matrix_model_view->Get() = view_mat.TransposeCopy();
			matrix_model_view_projection->Get() = (proj_mat * view_mat).TransposeCopy();


			//Demo setup
			engine.shader_program = shader_prog.get();

			//game.models.push_back(gray_rectangle);
			//game.models.push_back(red_square);
			//game.models.push_back(green_square);
			//game.models.push_back(blue_square);
			game.models.push_back(model);

			//game.animations.push_back(running);
			//game.particle_systems.push_back(asteroids);
			//game.texts.push_back(hello_world);

			game.viewport = engine.Target()->GetViewport("");
			game.matrix_projection = matrix_projection;
			game.matrix_model_view_projection = matrix_model_view_projection;
			game.light_type = light_type;
			game.light_position = light_position;
			game.light_direction = light_direction;
			game.light_diffuse = light_diffuse;


			//EXAMPLE end

			engine.Subscribe(game);
			window.Events().Subscribe(game);

			if (auto input = engine.Input(); input)
			{
				input->KeyEvents().Subscribe(game);
				input->MouseEvents().Subscribe(game);
			}

			engine.VerticalSync(false);
			exit_code = engine.Start();
		}
	}

	//Compile script
	{
		/*ion::assets::repositories::ScriptRepository script_repository{ion::assets::repositories::file_repository::NamingConvention::FilePath};
		ion::assets::AssetLoader loader;
		loader.Attach(script_repository);
		loader.LoadDirectory("bin", ion::utilities::file::DirectoryIteration::Recursive);*/
		//loader.CompileDataFile("bin/resources.dat");
		//loader.LoadFile("bin/resources.dat");

		auto basic = ion::script::script_validator::ClassDefinition::Create("basic")
			.AddRequiredProperty("resolution", {ion::script::script_validator::ParameterType::Integer, ion::script::script_validator::ParameterType::Integer})
			.AddRequiredProperty("fullscreen", ion::script::script_validator::ParameterType::Boolean);
		auto advanced = ion::script::script_validator::ClassDefinition::Create("advanced")
			.AddRequiredProperty("color-depth", ion::script::script_validator::ParameterType::Integer)
			.AddRequiredProperty("vertical-sync", ion::script::script_validator::ParameterType::Boolean)
			.AddRequiredProperty("frame-limit", ion::script::script_validator::ParameterType::FloatingPoint);
		auto settings = ion::script::script_validator::ClassDefinition::Create("settings")
			.AddRequiredClass(std::move(basic))
			.AddRequiredClass(std::move(advanced));
		auto engine = ion::script::script_validator::ClassDefinition::Create("engine")
			.AddRequiredClass(std::move(settings))
			.AddRequiredProperty("window-title", ion::script::script_validator::ParameterType::String)
			.AddRequiredProperty("window-position", ion::script::script_validator::ParameterType::Vector2)
			.AddRequiredProperty("clear-color", ion::script::script_validator::ParameterType::Color);

		auto validator = ion::script::ScriptValidator::Create()
			.AddRequiredClass(std::move(engine));

		ion::script::ScriptBuilder builder;
		builder.Validator(std::move(validator));
		builder.Output(ion::script::script_builder::OutputOptions::HeaderAndSummary);
		builder.CompilerOutput(ion::script::script_compiler::OutputOptions::SummaryAndUnits);
		builder.ValidatorOutput(ion::script::script_validator::OutputOptions::SummaryAndErrors);
		builder.TreeOutput(ion::script::script_tree::PrintOptions::Arguments);
		builder.BuildFile("bin/main.ion");
		//builder.BuildString("@import 'bin/main.ion';");
		auto &tree = builder.Tree();

		//Serialize tree
		if (tree)
		{
			[[maybe_unused]] auto color_depth =
				tree->Search("settings")
				.Find("advanced")
				.Property("color-depth")[0]
				.Get<ion::script::ScriptType::Integer>()
				.value_or(8)
				.As<int>();

			/*auto component = ion::script::script_validator::ClassDefinition::Create("component")
				.AddRequiredProperty("name", ion::script::script_validator::ParameterType::String);

			auto container = ion::script::script_validator::ClassDefinition::Create("container", "component")
				.AddProperty("name", ion::script::script_validator::ParameterType::String)
				.AddProperty("size", ion::script::script_validator::ParameterType::Vector2);

			auto button = ion::script::script_validator::ClassDefinition::Create("button", "component")
				.AddProperty("name", ion::script::script_validator::ParameterType::String)
				.AddClass(ion::script::script_validator::ClassDefinition::Create("over"))
				.AddClass(ion::script::script_validator::ClassDefinition::Create("press"));

			auto check_box = ion::script::script_validator::ClassDefinition::Create("check_box", "component")
				.AddClass(ion::script::script_validator::ClassDefinition::Create("over"))
				.AddClass(ion::script::script_validator::ClassDefinition::Create("press"))
				.AddClass(ion::script::script_validator::ClassDefinition::Create("check")
						.AddClass(ion::script::script_validator::ClassDefinition::Create("over"))
						.AddClass(ion::script::script_validator::ClassDefinition::Create("press")));

			auto group_box = ion::script::script_validator::ClassDefinition::Create("group_box", "container")
				.AddClass("button")
				.AddClass("check_box")
				.AddClass("group_box");

			auto validator = ion::script::ScriptValidator::Create()
				.AddAbstractClass(std::move(component))
				.AddAbstractClass(std::move(container))
				.AddClass(std::move(button))
				.AddClass(std::move(check_box))
				.AddClass(std::move(group_box));

			ion::script::ValidateError validate_error;
			auto okay = validator.Validate(*tree, validate_error);

			if (!okay && validate_error)
			{
				auto what = validate_error.Condition.message();
				auto fully_qualified_name = validate_error.FullyQualifiedName;
			}*/


			auto tree_bytes = tree->Serialize();
			ion::utilities::file::Save("bin/main.obj",
				{reinterpret_cast<char*>(std::data(tree_bytes)), std::size(tree_bytes)},
				ion::utilities::file::FileSaveMode::Binary);

			//Load object file

			std::string bytes;
			ion::utilities::file::Load("bin/main.obj", bytes,
				ion::utilities::file::FileLoadMode::Binary);

			//Deserialize
			auto deserialized_tree = ion::script::ScriptTree::Deserialize(bytes);
		}
	}

	/*{
		auto encoded = ion::utilities::codec::EncodeTo(5050, 16);
		auto decoded = ion::utilities::codec::DecodeFrom<int>(*encoded, 16);
	}

	{
		auto encoded = ion::utilities::codec::EncodeToHex("Hello World!");
		auto decoded = ion::utilities::codec::DecodeFromHex(encoded);
	}

	{
		auto encoded = ion::utilities::codec::EncodeToBase32("Hello World!");
		auto decoded = ion::utilities::codec::DecodeFromBase32(encoded);
	}

	{
		auto encoded = ion::utilities::codec::EncodeToBase64("Hello World!");
		auto decoded = ion::utilities::codec::DecodeFromBase64(encoded);
	}

	{
		auto encoded = ion::utilities::codec::EncodeToBase64_URL("Hello World!");
		auto decoded = ion::utilities::codec::DecodeFromBase64_URL(encoded);
	}

	{
		auto empty_checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::Keccak_224(""));
		assert(empty_checksum == "f71837502ba8e10837bdd8d365adb85591895602fc552b48b7390abd");
	}

	{
		auto empty_checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::Keccak_256(""));
		assert(empty_checksum == "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470");
	}

	{
		auto empty_checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::Keccak_384(""));
		assert(empty_checksum == "2c23146a63a29acf99e73b88f8c24eaa7dc60aa771780ccc006afbfa8fe2479b2dd2b21362337441ac12b515911957ff");
	}

	{
		auto empty_checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::Keccak_512(""));
		assert(empty_checksum == "0eab42de4c3ceb9235fc91acffe746b29c29a8c366b7c60e4e67c466f36a4304c00fa9caf9d87976ba469bcbe06713b435f091ef2769fb160cdab33d3670680e");
	}

	{
		auto empty_checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::SHA3_224(""));
		assert(empty_checksum == "6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7");

		auto checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::SHA3_224("The quick brown fox jumps over the lazy dog"));
		assert(checksum == "d15dadceaa4d5d7bb3b48f446421d542e08ad8887305e28d58335795");
	}

	{
		auto empty_checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::SHA3_256(""));
		assert(empty_checksum == "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a");

		auto checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::SHA3_256("The quick brown fox jumps over the lazy dog"));
		assert(checksum == "69070dda01975c8c120c3aada1b282394e7f032fa9cf32f4cb2259a0897dfc04");
	}

	{
		auto empty_checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::SHA3_384(""));
		assert(empty_checksum == "0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004");

		auto checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::SHA3_384("The quick brown fox jumps over the lazy dog"));
		assert(checksum == "7063465e08a93bce31cd89d2e3ca8f602498696e253592ed26f07bf7e703cf328581e1471a7ba7ab119b1a9ebdf8be41");
	}

	{
		auto empty_checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::SHA3_512(""));
		assert(empty_checksum == "a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26");

		auto checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::SHA3_512("The quick brown fox jumps over the lazy dog"));
		assert(checksum == "01dedd5de4ef14642445ba5f5b97c15e47b9ad931326e4b0727cd94cefc44fff23f07bf543139939b49128caf436dc1bdee54fcb24023a08d9403f9b4bf0d450");
	}

	{
		auto empty_checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::SHAKE_128("", 32));
		assert(empty_checksum == "7f9c2ba4e88f827d616045507605853ed73b8093f6efbc88eb1a6eacfa66ef26");

		auto checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::SHAKE_128("The quick brown fox jumps over the lazy dog", 32));
		assert(checksum == "f4202e3c5852f9182a0430fd8144f0a74b95e7417ecae17db0f8cfeed0e3e66e");
	}

	{
		auto empty_checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::SHAKE_256("", 64));
		assert(empty_checksum == "46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762fd75dc4ddd8c0f200cb05019d67b592f6fc821c49479ab48640292eacb3b7c4be");

		auto checksum = ion::utilities::codec::EncodeToHex(ion::utilities::crypto::SHAKE_256("The quick brown fox jumps over the lazy dog", 64));
		assert(checksum == "2f671343d9b2e1604dc9dcf0753e5fe15c7c64a0d283cbbf722d411a0e36f6ca1d01d1369a23539cd80f7c054b6e5daf9c962cad5b8ed5bd11998b40d5734442");
	}*/
	
	
	/*{
		auto concat_result = ion::utilities::string::Concat(10, ","sv, 'A', ","s, 3.14, ",");
		auto join_result = ion::utilities::string::Join(","sv, 10, 'A', 3.14, "string"s, "char[]");
		auto format_result = ion::utilities::string::Format("{0}, {1} and {2 : 00.0000} + {3}, {4} and {5}", 10, 'A', 3.14, "string_view"sv, "string"s, "char[]");
	}*/

	return exit_code;
}