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
#include "events/listeners/IonSceneNodeListener.h"
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
#include "graphics/render/IonFog.h"
#include "graphics/render/IonFrustum.h"
#include "graphics/render/IonPass.h"
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
#include "graphics/scene/IonDrawableAnimation.h"
#include "graphics/scene/IonDrawableObject.h"
#include "graphics/scene/IonDrawableParticleSystem.h"
#include "graphics/scene/IonDrawableText.h"
#include "graphics/scene/IonLight.h"
#include "graphics/scene/IonModel.h"
#include "graphics/scene/IonMovableObject.h"
#include "graphics/scene/IonMovableSound.h"
#include "graphics/scene/IonMovableSoundListener.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/scene/graph/IonSceneGraph.h"
#include "graphics/scene/graph/IonSceneNode.h"
#include "graphics/scene/graph/animations/IonAttachableNodeAnimation.h"
#include "graphics/scene/graph/animations/IonAttachableNodeAnimationGroup.h"
#include "graphics/scene/graph/animations/IonNodeAnimation.h"
#include "graphics/scene/graph/animations/IonNodeAnimationGroup.h"
#include "graphics/scene/graph/animations/IonNodeAnimationManager.h"
#include "graphics/scene/graph/animations/IonNodeAnimationTimeline.h"
#include "graphics/scene/query/IonSceneQuery.h"
#include "graphics/scene/query/IonIntersectionSceneQuery.h"
#include "graphics/scene/query/IonRaySceneQuery.h"
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
#include "graphics/textures/IonTextureAtlas.h"
#include "graphics/textures/IonTextureManager.h"
#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonMatrix2.h"
#include "graphics/utilities/IonMatrix3.h"
#include "graphics/utilities/IonMatrix4.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonRay.h"
#include "graphics/utilities/IonSphere.h"
#include "graphics/utilities/IonVector2.h"
#include "graphics/utilities/IonVector3.h"

#include "gui/IonGuiComponent.h"
#include "gui/IonGuiContainer.h"
#include "gui/IonGuiController.h"
#include "gui/IonGuiFrame.h"
#include "gui/IonGuiPanel.h"
#include "gui/IonGuiPanelContainer.h"
#include "gui/controls/IonGuiButton.h"
#include "gui/controls/IonGuiCheckBox.h"
#include "gui/controls/IonGuiControl.h"
#include "gui/controls/IonGuiGroupBox.h"
#include "gui/controls/IonGuiLabel.h"
#include "gui/controls/IonGuiListBox.h"
#include "gui/controls/IonGuiMouseCursor.h"
#include "gui/controls/IonGuiProgressBar.h"
#include "gui/controls/IonGuiRadioButton.h"
#include "gui/controls/IonGuiScrollBar.h"
#include "gui/controls/IonGuiSlider.h"
#include "gui/controls/IonGuiTextBox.h"
#include "gui/controls/IonGuiTooltip.h"
#include "gui/skins/IonGuiSkin.h"
#include "gui/skins/IonGuiTheme.h"

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

#include "sounds/IonSound.h"
#include "sounds/IonSoundChannel.h"
#include "sounds/IonSoundChannelGroup.h"
#include "sounds/IonSoundListener.h"
#include "sounds/IonSoundManager.h"

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
#include "types/IonUniqueVal.h"

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
	ion::NonOwningPtr<ion::graphics::scene::graph::SceneGraph> scene_graph;
	ion::NonOwningPtr<ion::graphics::render::Viewport> viewport;
	ion::gui::GuiController *controller = nullptr;
	ion::sounds::SoundManager *sound_manager = nullptr;

	ion::NonOwningPtr<ion::graphics::scene::DrawableText> fps;
	ion::types::Cumulative<duration> fps_update_rate{1.0_sec};

	ion::NonOwningPtr<ion::graphics::scene::graph::SceneNode> player_node;
	ion::NonOwningPtr<ion::graphics::scene::graph::SceneNode> light_node;
	ion::NonOwningPtr<ion::graphics::scene::graph::SceneNode> aura_node;
	ion::graphics::utilities::Vector2 move_model;
	bool rotate_model_left = false;
	bool rotate_model_right = false;

	ion::NonOwningPtr<ion::graphics::scene::Camera> camera;
	ion::NonOwningPtr<ion::graphics::scene::Camera> player_camera;
	ion::graphics::utilities::Vector2 move_camera;
	bool rotate_camera_left = false;
	bool rotate_camera_right = false;
	ion::types::Cumulative<duration> idle_time{2.0_sec};

	ion::NonOwningPtr<ion::graphics::scene::graph::animations::NodeAnimationTimeline> idle;


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

		if (idle)
		{
			if (idle_time += time)
				idle->Start();
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

				if (idle)
				{
					idle_time.Reset();
					idle->Revert();
				}
			}	
		}

		if (aura_node)
			aura_node->Rotate(math::ToRadians(-90.0_r) * time.count());

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

		if (controller && controller->IsVisible())
			controller->FrameStarted(time);

		return true;
	}

	bool FrameEnded(duration time) noexcept override
	{
		if (sound_manager)
			sound_manager->Update();

		if (controller && controller->IsVisible())
			controller->FrameEnded(time);

		return true;
	}


	/*
		Window listener
	*/

	void WindowActionReceived(ion::events::listeners::WindowAction action) noexcept override
	{
		if (controller && controller->IsVisible())
			controller->WindowActionReceived(action);
	}


	/*
		Key listener
	*/

	void KeyPressed(ion::events::listeners::KeyButton button) noexcept override
	{
		using namespace ion::graphics::utilities;

		if (!controller || !controller->IsVisible())
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

		if (controller && controller->IsVisible())
			controller->KeyPressed(button);
	}

	void KeyReleased(ion::events::listeners::KeyButton button) noexcept override
	{
		using namespace ion::graphics::utilities;

		if (!controller || !controller->IsVisible())
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

		if (controller)
		{
			switch (button)
			{
				case ion::events::listeners::KeyButton::Escape:
				controller->Visible(!controller->IsVisible());
				break;
			}

			if (controller->IsVisible())
				controller->KeyReleased(button);
		}
  	}

	void CharacterPressed(char character) noexcept override
	{
		if (controller && controller->IsVisible())
			controller->CharacterPressed(character);
	}


	/*
		Mouse listener
	*/

	void MousePressed(ion::events::listeners::MouseButton button, ion::graphics::utilities::Vector2 position) noexcept override
	{
		if (controller && controller->IsVisible())
			controller->MousePressed(button, position);
	}

	void MouseReleased(ion::events::listeners::MouseButton button, ion::graphics::utilities::Vector2 position) noexcept override
	{
		if (controller && controller->IsVisible())
			controller->MouseReleased(button, position);
	}

	void MouseMoved(ion::graphics::utilities::Vector2 position) noexcept override
	{
		if (controller && controller->IsVisible())
			controller->MouseMoved(position);
	}

	void MouseWheelRolled(int delta, ion::graphics::utilities::Vector2 position) noexcept override
	{
		if (controller && controller->IsVisible())
			controller->MouseWheelRolled(delta, position);
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
		engine.PixelsPerUnit(360.0_r);
		engine.UnitsPerMeter(0.04_r);

		auto &window = engine.RenderTo(
			ion::graphics::render::RenderWindow::Resizable("ION engine", {1280.0_r, 720.0_r}));
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
			auto cloud_diffuse = textures.CreateTexture("cloud", "cloud.png");
			auto star_diffuse = textures.CreateTexture("star", "star.png");
			auto ship_diffuse = textures.CreateTexture("ship", "ship.png");
			auto aura_diffuse = textures.CreateTexture("aura", "aura.png");
			auto raindrop_diffuse = textures.CreateTexture("raindrop", "raindrop.png");
			auto color_spectrum_diffuse = textures.CreateTexture("color_spectrum", "color_spectrum.png");

			//GUI
			//Mouse cursor
			auto mouse_cursor_diffuse = textures.CreateTexture("mouse_cursor_diffuse", "mouse_cursor.png");

			//Tooltip
			auto tooltip_center_diffuse = textures.CreateTexture("tooltip_center", "tooltip_center.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto tooltip_top_diffuse = textures.CreateTexture("tooltip_top", "tooltip_top.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto tooltip_left_diffuse = textures.CreateTexture("tooltip_left", "tooltip_left.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto tooltip_top_left_diffuse = textures.CreateTexture("tooltip_top_left", "tooltip_top_left.png");

			//Button
			auto button_center_enabled_diffuse = textures.CreateTexture("button_center_enabled", "button_center_enabled.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto button_center_disabled_diffuse = textures.CreateTexture("button_center_disabled", "button_center_disabled.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto button_center_pressed_diffuse = textures.CreateTexture("button_center_pressed", "button_center_pressed.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto button_center_hovered_diffuse = textures.CreateTexture("button_center_hovered", "button_center_hovered.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);

			auto button_top_enabled_diffuse = textures.CreateTexture("button_top_enabled", "button_top_enabled.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto button_top_focused_diffuse = textures.CreateTexture("button_top_focused", "button_top_focused.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto button_left_enabled_diffuse = textures.CreateTexture("button_left_enabled", "button_left_enabled.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto button_left_focused_diffuse = textures.CreateTexture("button_left_focused", "button_left_focused.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);

			auto button_top_left_enabled_diffuse = textures.CreateTexture("button_top_left_enabled", "button_top_left_enabled.png");
			auto button_top_left_focused_diffuse = textures.CreateTexture("button_top_left_focused", "button_top_left_focused.png");

			//Check box
			auto check_box_center_enabled_diffuse = textures.CreateTexture("check_box_center_enabled", "check_box_center_enabled.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto check_box_center_hovered_diffuse = textures.CreateTexture("check_box_center_hovered", "check_box_center_hovered.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);

			auto check_box_mark_enabled_diffuse = textures.CreateTexture("check_box_mark_enabled", "check_box_mark_enabled.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto check_box_mark_disabled_diffuse = textures.CreateTexture("check_box_mark_disabled", "check_box_mark_disabled.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);		
			auto check_box_mark_pressed_diffuse = textures.CreateTexture("check_box_mark_pressed", "check_box_mark_pressed.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto check_box_mark_hovered_diffuse = textures.CreateTexture("check_box_mark_hovered", "check_box_mark_hovered.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);

			//Progress bar
			auto progress_bar_bar_enabled_diffuse = textures.CreateTexture("progress_bar_bar_enabled", "progress_bar_bar_enabled.png");

			//Radio button
			auto radio_button_select_enabled_diffuse = textures.CreateTexture("radio_button_select_enabled", "radio_button_select_enabled.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto radio_button_select_disabled_diffuse = textures.CreateTexture("radio_button_select_disabled", "radio_button_select_disabled.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto radio_button_select_pressed_diffuse = textures.CreateTexture("radio_button_select_pressed", "radio_button_select_pressed.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);
			auto radio_button_select_hovered_diffuse = textures.CreateTexture("radio_button_select_hovered", "radio_button_select_hovered.png",
				ion::graphics::textures::texture::TextureFilter::Bilinear, ion::graphics::textures::texture::TextureWrapMode::Repeat);

			//Text box
			auto text_box_cursor_enabled_diffuse = textures.CreateTexture("text_box_cursor_enabled", "text_box_cursor_enabled.png");


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
			auto model_vert_shader = shaders.CreateShader("default_model_vert", "default_model.vert");
			auto model_frag_shader = shaders.CreateShader("default_model_frag", "default_model.frag");
			auto particle_vert_shader = shaders.CreateShader("default_particle_vert", "default_particle.vert");
			auto particle_frag_shader = shaders.CreateShader("default_particle_frag", "default_particle.frag");
			auto text_vert_shader = shaders.CreateShader("default_text_vert", "default_text.vert");
			auto text_frag_shader = shaders.CreateShader("default_text_frag", "default_text.frag");
			auto gui_text_vert_shader = shaders.CreateShader("default_gui_text_vert", "default_gui_text.vert");
			auto gui_text_frag_shader = shaders.CreateShader("default_gui_text_frag", "default_gui_text.frag");
			shaders.LoadAll(/*ion::resources::resource_manager::EvaluationStrategy::Lazy*/);

			//while (!shaders.Loaded());

			//Shader programs
			ion::graphics::shaders::ShaderProgramManager shader_programs;
			shader_programs.LogLevel(ion::graphics::shaders::shader_program_manager::InfoLogLevel::Error);
			auto model_program = shader_programs.CreateShaderProgram("default_model_prog", model_vert_shader, model_frag_shader);
			auto particle_program = shader_programs.CreateShaderProgram("default_particle_prog", particle_vert_shader, particle_frag_shader);
			auto text_program = shader_programs.CreateShaderProgram("default_text_prog", text_vert_shader, text_frag_shader);
			auto gui_text_program = shader_programs.CreateShaderProgram("default_gui_text_prog", gui_text_vert_shader, gui_text_frag_shader);
			shader_programs.LoadAll(/*ion::resources::resource_manager::EvaluationStrategy::Lazy*/);

			//while (!shader_programs.Loaded());

			using namespace ion::graphics::shaders::variables;

			//Model program
			{
				//Shader structs
				auto matrix_struct = model_program->CreateStruct("matrix");
				auto scene_struct = model_program->CreateStruct("scene");
				auto camera_struct = model_program->CreateStruct("camera");
				auto primitive_struct = model_program->CreateStruct("primitive");
				auto material_struct = model_program->CreateStruct("material");
				auto fog_struct = model_program->CreateStruct("fog");
				auto light_struct = model_program->CreateStruct("light", 8);


				//Shader variables
				//Vertex
				model_program->CreateAttribute<glsl::vec3>("vertex_position");
				model_program->CreateAttribute<glsl::vec3>("vertex_normal");
				model_program->CreateAttribute<glsl::vec4>("vertex_color");
				model_program->CreateAttribute<glsl::vec2>("vertex_tex_coord");

				//Matrices			
				matrix_struct->CreateUniform<glsl::mat4>("model_view");
				matrix_struct->CreateUniform<glsl::mat4>("projection");
				matrix_struct->CreateUniform<glsl::mat4>("model_view_projection");

				//Scene
				scene_struct->CreateUniform<glsl::vec4>("ambient");
				scene_struct->CreateUniform<float>("gamma");
				scene_struct->CreateUniform<bool>("has_fog");
				scene_struct->CreateUniform<int>("light_count");

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
				material_struct->CreateUniform<glsl::sampler2D>("specular_map");
				material_struct->CreateUniform<glsl::sampler2D>("normal_map");
				material_struct->CreateUniform<bool>("has_diffuse_map");
				material_struct->CreateUniform<bool>("has_specular_map");
				material_struct->CreateUniform<bool>("has_normal_map");
				material_struct->CreateUniform<bool>("lighting_enabled");

				//Fog
				fog_struct->CreateUniform<int>("mode");
				fog_struct->CreateUniform<float>("density");
				fog_struct->CreateUniform<float>("near");
				fog_struct->CreateUniform<float>("far");
				fog_struct->CreateUniform<glsl::vec4>("color");

				//Light
				light_struct->CreateUniform<int>("type");
				light_struct->CreateUniform<glsl::vec3>("position");
				light_struct->CreateUniform<glsl::vec3>("direction");
				light_struct->CreateUniform<glsl::vec4>("ambient");
				light_struct->CreateUniform<glsl::vec4>("diffuse");
				light_struct->CreateUniform<glsl::vec4>("specular");
				light_struct->CreateUniform<float>("constant");
				light_struct->CreateUniform<float>("linear");
				light_struct->CreateUniform<float>("quadratic");
				light_struct->CreateUniform<float>("cutoff");
				light_struct->CreateUniform<float>("outer_cutoff");

				shader_programs.LoadShaderVariableLocations(*model_program);
			}

			//Particle program
			{
				//Shader structs
				auto matrix_struct = particle_program->CreateStruct("matrix");
				auto scene_struct = particle_program->CreateStruct("scene");
				auto camera_struct = particle_program->CreateStruct("camera");
				auto node_struct = particle_program->CreateStruct("node");
				auto primitive_struct = particle_program->CreateStruct("primitive");
				auto material_struct = particle_program->CreateStruct("material");
				auto fog_struct = particle_program->CreateStruct("fog");
				auto light_struct = particle_program->CreateStruct("light", 8);


				//Shader variables
				//Vertex
				particle_program->CreateAttribute<glsl::vec3>("vertex_position");
				particle_program->CreateAttribute<float>("vertex_rotation");
				particle_program->CreateAttribute<float>("vertex_point_size");
				particle_program->CreateAttribute<glsl::vec4>("vertex_color");

				//Matrices			
				matrix_struct->CreateUniform<glsl::mat4>("model_view");
				matrix_struct->CreateUniform<glsl::mat4>("projection");
				matrix_struct->CreateUniform<glsl::mat4>("model_view_projection");

				//Scene
				scene_struct->CreateUniform<glsl::vec4>("ambient");
				scene_struct->CreateUniform<float>("gamma");
				scene_struct->CreateUniform<bool>("has_fog");
				scene_struct->CreateUniform<int>("light_count");

				//Camera
				camera_struct->CreateUniform<glsl::vec3>("position");
				camera_struct->CreateUniform<float>("rotation");

				//Node
				node_struct->CreateUniform<float>("rotation");
				node_struct->CreateUniform<glsl::vec2>("scaling");

				//Primitive
				primitive_struct->CreateUniform<bool>("has_material");

				//Material			
				material_struct->CreateUniform<glsl::vec4>("ambient");
				material_struct->CreateUniform<glsl::vec4>("diffuse");
				material_struct->CreateUniform<glsl::vec4>("specular");
				material_struct->CreateUniform<glsl::vec4>("emissive");
				material_struct->CreateUniform<float>("shininess");
				material_struct->CreateUniform<glsl::sampler2D>("diffuse_map");	
				material_struct->CreateUniform<glsl::sampler2D>("specular_map");
				material_struct->CreateUniform<glsl::sampler2D>("normal_map");
				material_struct->CreateUniform<bool>("has_diffuse_map");
				material_struct->CreateUniform<bool>("has_specular_map");
				material_struct->CreateUniform<bool>("has_normal_map");
				material_struct->CreateUniform<bool>("lighting_enabled");

				//Fog
				fog_struct->CreateUniform<int>("mode");
				fog_struct->CreateUniform<float>("density");
				fog_struct->CreateUniform<float>("near");
				fog_struct->CreateUniform<float>("far");
				fog_struct->CreateUniform<glsl::vec4>("color");

				//Light
				light_struct->CreateUniform<int>("type");
				light_struct->CreateUniform<glsl::vec3>("position");
				light_struct->CreateUniform<glsl::vec3>("direction");
				light_struct->CreateUniform<glsl::vec4>("ambient");
				light_struct->CreateUniform<glsl::vec4>("diffuse");
				light_struct->CreateUniform<glsl::vec4>("specular");
				light_struct->CreateUniform<float>("constant");
				light_struct->CreateUniform<float>("linear");
				light_struct->CreateUniform<float>("quadratic");
				light_struct->CreateUniform<float>("cutoff");
				light_struct->CreateUniform<float>("outer_cutoff");

				shader_programs.LoadShaderVariableLocations(*particle_program);
			}

			//Text program
			{
				//Shader structs
				auto matrix_struct = text_program->CreateStruct("matrix");
				auto scene_struct = text_program->CreateStruct("scene");
				auto camera_struct = text_program->CreateStruct("camera");
				auto primitive_struct = text_program->CreateStruct("primitive");
				auto fog_struct = text_program->CreateStruct("fog");
				auto light_struct = text_program->CreateStruct("light", 8);


				//Shader variables
				//Vertex
				text_program->CreateAttribute<glsl::vec3>("vertex_position");
				text_program->CreateAttribute<glsl::vec4>("vertex_color");
				text_program->CreateAttribute<glsl::vec2>("vertex_tex_coord");

				//Matrices			
				matrix_struct->CreateUniform<glsl::mat4>("model_view");
				matrix_struct->CreateUniform<glsl::mat4>("projection");
				matrix_struct->CreateUniform<glsl::mat4>("model_view_projection");

				//Scene
				scene_struct->CreateUniform<glsl::vec4>("ambient");
				scene_struct->CreateUniform<float>("gamma");
				scene_struct->CreateUniform<bool>("has_fog");
				scene_struct->CreateUniform<int>("light_count");

				//Camera
				camera_struct->CreateUniform<glsl::vec3>("position");	

				//Primitive
				primitive_struct->CreateUniform<glsl::sampler2D>("texture");
				primitive_struct->CreateUniform<bool>("has_texture");

				//Fog
				fog_struct->CreateUniform<int>("mode");
				fog_struct->CreateUniform<float>("density");
				fog_struct->CreateUniform<float>("near");
				fog_struct->CreateUniform<float>("far");
				fog_struct->CreateUniform<glsl::vec4>("color");

				//Light
				light_struct->CreateUniform<int>("type");
				light_struct->CreateUniform<glsl::vec3>("position");
				light_struct->CreateUniform<glsl::vec3>("direction");
				light_struct->CreateUniform<glsl::vec4>("ambient");
				light_struct->CreateUniform<glsl::vec4>("diffuse");
				light_struct->CreateUniform<glsl::vec4>("specular");
				light_struct->CreateUniform<float>("constant");
				light_struct->CreateUniform<float>("linear");
				light_struct->CreateUniform<float>("quadratic");
				light_struct->CreateUniform<float>("cutoff");
				light_struct->CreateUniform<float>("outer_cutoff");

				shader_programs.LoadShaderVariableLocations(*text_program);
			}

			//GUI text program
			{
				//Shader structs
				auto matrix_struct = gui_text_program->CreateStruct("matrix");
				auto scene_struct = gui_text_program->CreateStruct("scene");
				auto primitive_struct = gui_text_program->CreateStruct("primitive");


				//Shader variables
				//Vertex
				gui_text_program->CreateAttribute<glsl::vec3>("vertex_position");
				gui_text_program->CreateAttribute<glsl::vec4>("vertex_color");
				gui_text_program->CreateAttribute<glsl::vec2>("vertex_tex_coord");

				//Matrices			
				matrix_struct->CreateUniform<glsl::mat4>("model_view");
				matrix_struct->CreateUniform<glsl::mat4>("projection");
				matrix_struct->CreateUniform<glsl::mat4>("model_view_projection");

				//Scene
				scene_struct->CreateUniform<float>("gamma");

				//Primitive
				primitive_struct->CreateUniform<glsl::sampler2D>("texture");
				primitive_struct->CreateUniform<bool>("has_texture");

				shader_programs.LoadShaderVariableLocations(*gui_text_program);
			}


			//Font
			ion::graphics::fonts::FontManager fonts;
			fonts.CreateRepository(std::move(font_repository));
			auto verdana_regular_12 = fonts.CreateFont("verdana_regular_12", "verdana.ttf", 12);
			auto verdana_bold_12 = fonts.CreateFont("verdana_bold_12", "verdanab.ttf", 12);
			auto verdana_italic_12 = fonts.CreateFont("verdana_italic_12", "verdanai.ttf", 12);
			auto verdana_bold_italic_12 = fonts.CreateFont("verdana_bold_italic_12", "verdanaz.ttf", 12);
			auto verdana_regular_36 = fonts.CreateFont("verdana_regular_36", "verdana.ttf", 36);
			auto verdana_bold_36 = fonts.CreateFont("verdana_bold_36", "verdanab.ttf", 36);
			auto verdana_italic_36 = fonts.CreateFont("verdana_italic_36", "verdanai.ttf", 36);
			auto verdana_bold_italic_36 = fonts.CreateFont("verdana_bold_italic_36", "verdanaz.ttf", 36);
			fonts.LoadAll(/*ion::resources::resource_manager::EvaluationStrategy::Lazy*/);

			//while (!fonts.Loaded());

			//Type face
			ion::graphics::fonts::TypeFaceManager type_faces;
			auto verdana_12 = 
				type_faces.CreateTypeFace(
					"verdana_12",
					verdana_regular_12,
					verdana_bold_12,
					verdana_italic_12,
					verdana_bold_italic_12);

			auto verdana_36 = 
				type_faces.CreateTypeFace(
					"verdana_36",
					verdana_regular_36,
					verdana_bold_36,
					verdana_italic_36,
					verdana_bold_italic_36);

			//Sound
			ion::sounds::SoundManager sounds;
			sounds.CreateRepository(std::move(audio_repository));
			auto sound_listener = sounds.CreateSoundListener("listener");
			auto gui_sound_channel_group = sounds.CreateSoundChannelGroup("gui");
			gui_sound_channel_group->Volume(0.2_r);

			auto flicker = sounds.CreateSound(ion::sounds::Sound::Positional("flicker", "flicker.wav",
				ion::sounds::sound::SoundType::Sample, ion::sounds::sound::SoundLoopingMode::Forward));
			flicker->Distance(0.4_r); //Min distance of 10 meters
			auto night_runner = sounds.CreateSound("night_runner", "night_runner.mp3",
				ion::sounds::sound::SoundType::Stream, ion::sounds::sound::SoundLoopingMode::Forward);
			auto click = sounds.CreateSound("click", "click.wav", ion::sounds::sound::SoundType::Sample);	
			sounds.LoadAll(/*ion::resources::resource_manager::EvaluationStrategy::Lazy*/);

			//while (!sounds.Loaded());

			//night_runner->Play()->Volume(0.2_r);


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

			auto cloud =
				materials.CreateMaterial("cloud",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{0.6_r, 0.6_r, 0.6_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r, cloud_diffuse, nullptr, nullptr);

			auto star =
				materials.CreateMaterial("star",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, star_diffuse, nullptr, nullptr);
			star->LightingEnabled(false);

			auto ship =
				materials.CreateMaterial("ship",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{0.6_r, 0.6_r, 0.6_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r, ship_diffuse, nullptr, nullptr);

			auto aura =
				materials.CreateMaterial("aura",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{0.6_r, 0.6_r, 0.6_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r, aura_diffuse, nullptr, nullptr);

			auto raindrop =
				materials.CreateMaterial("raindrop",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{0.6_r, 0.6_r, 0.6_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r, raindrop_diffuse, nullptr, nullptr);
			
			auto color_spectrum =
				materials.CreateMaterial("color_spectrum",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, color_spectrum_diffuse, nullptr, nullptr);
			color_spectrum->LightingEnabled(false);

			auto cat =
				materials.CreateMaterial("cat",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{0.6_r, 0.6_r, 0.6_r},
					{0.0_r, 0.0_r, 0.0_r},
					32.0_r, cat_running, nullptr, nullptr);

			//GUI
			//Mouse cursor materials
			auto mouse_cursor_enabled =
				materials.CreateMaterial("mouse_cursor",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, mouse_cursor_diffuse, nullptr, nullptr);
			mouse_cursor_enabled->LightingEnabled(false);

			//Tooltip materials
			auto tooltip_center_enabled =
				materials.CreateMaterial("tooltip_center",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, tooltip_center_diffuse, nullptr, nullptr);
			tooltip_center_enabled->LightingEnabled(false);


			auto tooltip_top_enabled =
				materials.CreateMaterial("tooltip_top",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, tooltip_top_diffuse, nullptr, nullptr);
			tooltip_top_enabled->LightingEnabled(false);

			auto tooltip_left_enabled =
				materials.CreateMaterial("tooltip_left",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, tooltip_left_diffuse, nullptr, nullptr);
			tooltip_left_enabled->LightingEnabled(false);

			auto tooltip_bottom_enabled =
				materials.CreateMaterial("tooltip_bottom",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, tooltip_top_diffuse, nullptr, nullptr);
			tooltip_bottom_enabled->LightingEnabled(false);
			tooltip_bottom_enabled->FlipVertical();

			auto tooltip_right_enabled =
				materials.CreateMaterial("tooltip_right",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, tooltip_left_diffuse, nullptr, nullptr);
			tooltip_right_enabled->LightingEnabled(false);
			tooltip_right_enabled->FlipHorizontal();


			auto tooltip_top_left_enabled =
				materials.CreateMaterial("tooltip_top_left",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, tooltip_top_left_diffuse, nullptr, nullptr);
			tooltip_top_left_enabled->LightingEnabled(false);

			auto tooltip_bottom_left_enabled =
				materials.CreateMaterial("tooltip_bottom_left",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, tooltip_top_left_diffuse, nullptr, nullptr);
			tooltip_bottom_left_enabled->LightingEnabled(false);
			tooltip_bottom_left_enabled->FlipVertical();

			auto tooltip_top_right_enabled =
				materials.CreateMaterial("tooltip_top_right",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, tooltip_top_left_diffuse, nullptr, nullptr);
			tooltip_top_right_enabled->LightingEnabled(false);
			tooltip_top_right_enabled->FlipHorizontal();

			auto tooltip_bottom_right_enabled =
				materials.CreateMaterial("tooltip_bottom_right",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, tooltip_top_left_diffuse, nullptr, nullptr);
			tooltip_bottom_right_enabled->LightingEnabled(false);
			tooltip_bottom_right_enabled->FlipHorizontal();
			tooltip_bottom_right_enabled->FlipVertical();

			//Button materials
			auto button_center_enabled =
				materials.CreateMaterial("button_center_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_center_enabled_diffuse, nullptr, nullptr);
			button_center_enabled->LightingEnabled(false);

			auto button_center_disabled =
				materials.CreateMaterial("button_center_disabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_center_disabled_diffuse, nullptr, nullptr);
			button_center_disabled->LightingEnabled(false);

			auto button_center_pressed =
				materials.CreateMaterial("button_center_pressed",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_center_pressed_diffuse, nullptr, nullptr);
			button_center_pressed->LightingEnabled(false);

			auto button_center_hovered =
				materials.CreateMaterial("button_center_hovered",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_center_hovered_diffuse, nullptr, nullptr);
			button_center_hovered->LightingEnabled(false);
			

			auto button_top_enabled =
				materials.CreateMaterial("button_top_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_top_enabled_diffuse, nullptr, nullptr);
			button_top_enabled->LightingEnabled(false);

			auto button_top_focused =
				materials.CreateMaterial("button_top_focused",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_top_focused_diffuse, nullptr, nullptr);
			button_top_focused->LightingEnabled(false);

			auto button_left_enabled =
				materials.CreateMaterial("button_left_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_left_enabled_diffuse, nullptr, nullptr);
			button_left_enabled->LightingEnabled(false);

			auto button_left_focused =
				materials.CreateMaterial("button_left_focused",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_left_focused_diffuse, nullptr, nullptr);
			button_left_focused->LightingEnabled(false);

			auto button_bottom_enabled =
				materials.CreateMaterial("button_bottom_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_top_enabled_diffuse, nullptr, nullptr);
			button_bottom_enabled->LightingEnabled(false);
			button_bottom_enabled->FlipVertical();

			auto button_bottom_focused =
				materials.CreateMaterial("button_bottom_focused",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_top_focused_diffuse, nullptr, nullptr);
			button_bottom_focused->LightingEnabled(false);
			button_bottom_focused->FlipVertical();

			auto button_right_enabled =
				materials.CreateMaterial("button_right_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_left_enabled_diffuse, nullptr, nullptr);
			button_right_enabled->LightingEnabled(false);
			button_right_enabled->FlipHorizontal();

			auto button_right_focused =
				materials.CreateMaterial("button_right_focused",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_left_focused_diffuse, nullptr, nullptr);
			button_right_focused->LightingEnabled(false);
			button_right_focused->FlipHorizontal();


			auto button_top_left_enabled =
				materials.CreateMaterial("button_top_left_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_top_left_enabled_diffuse, nullptr, nullptr);
			button_top_left_enabled->LightingEnabled(false);

			auto button_top_left_focused =
				materials.CreateMaterial("button_top_left_focused",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_top_left_focused_diffuse, nullptr, nullptr);
			button_top_left_focused->LightingEnabled(false);

			auto button_bottom_left_enabled =
				materials.CreateMaterial("button_bottom_left_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_top_left_enabled_diffuse, nullptr, nullptr);
			button_bottom_left_enabled->LightingEnabled(false);
			button_bottom_left_enabled->FlipVertical();

			auto button_bottom_left_focused =
				materials.CreateMaterial("button_bottom_left_focused",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_top_left_focused_diffuse, nullptr, nullptr);
			button_bottom_left_focused->LightingEnabled(false);
			button_bottom_left_focused->FlipVertical();

			auto button_top_right_enabled =
				materials.CreateMaterial("button_top_right_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_top_left_enabled_diffuse, nullptr, nullptr);
			button_top_right_enabled->LightingEnabled(false);
			button_top_right_enabled->FlipHorizontal();

			auto button_top_right_focused =
				materials.CreateMaterial("button_top_right_focused",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_top_left_focused_diffuse, nullptr, nullptr);
			button_top_right_focused->LightingEnabled(false);
			button_top_right_focused->FlipHorizontal();

			auto button_bottom_right_enabled =
				materials.CreateMaterial("button_bottom_right_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_top_left_enabled_diffuse, nullptr, nullptr);
			button_bottom_right_enabled->LightingEnabled(false);
			button_bottom_right_enabled->FlipHorizontal();
			button_bottom_right_enabled->FlipVertical();

			auto button_bottom_right_focused =
				materials.CreateMaterial("button_bottom_right_focused",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, button_top_left_focused_diffuse, nullptr, nullptr);
			button_bottom_right_focused->LightingEnabled(false);
			button_bottom_right_focused->FlipHorizontal();
			button_bottom_right_focused->FlipVertical();

			//Check box materials
			auto check_box_center_enabled =
				materials.CreateMaterial("check_box_center_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, check_box_center_enabled_diffuse, nullptr, nullptr);
			check_box_center_enabled->LightingEnabled(false);

			auto check_box_center_hovered =
				materials.CreateMaterial("check_box_center_hovered",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, check_box_center_hovered_diffuse, nullptr, nullptr);
			check_box_center_hovered->LightingEnabled(false);


			auto check_box_mark_enabled =
				materials.CreateMaterial("check_box_mark_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, check_box_mark_enabled_diffuse, nullptr, nullptr);
			check_box_mark_enabled->LightingEnabled(false);

			auto check_box_mark_disabled =
				materials.CreateMaterial("check_box_mark_disabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, check_box_mark_disabled_diffuse, nullptr, nullptr);
			check_box_mark_disabled->LightingEnabled(false);

			auto check_box_mark_pressed =
				materials.CreateMaterial("check_box_mark_pressed",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, check_box_mark_pressed_diffuse, nullptr, nullptr);
			check_box_mark_pressed->LightingEnabled(false);

			auto check_box_mark_hovered =
				materials.CreateMaterial("check_box_mark_hovered",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, check_box_mark_hovered_diffuse, nullptr, nullptr);
			check_box_mark_hovered->LightingEnabled(false);

			//Progress bar
			auto progress_bar_bar_enabled =
				materials.CreateMaterial("progress_bar_bar_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, progress_bar_bar_enabled_diffuse, nullptr, nullptr);
			progress_bar_bar_enabled->LightingEnabled(false);

			//Radio buttons
			auto radio_button_select_enabled =
				materials.CreateMaterial("radio_button_select_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, radio_button_select_enabled_diffuse, nullptr, nullptr);
			radio_button_select_enabled->LightingEnabled(false);

			auto radio_button_select_disabled =
				materials.CreateMaterial("radio_button_select_disabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, radio_button_select_disabled_diffuse, nullptr, nullptr);
			radio_button_select_disabled->LightingEnabled(false);

			auto radio_button_select_pressed =
				materials.CreateMaterial("radio_button_select_pressed",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, radio_button_select_pressed_diffuse, nullptr, nullptr);
			radio_button_select_pressed->LightingEnabled(false);

			auto radio_button_select_hovered =
				materials.CreateMaterial("radio_button_select_hovered",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, radio_button_select_hovered_diffuse, nullptr, nullptr);
			radio_button_select_hovered->LightingEnabled(false);

			//Text box
			auto text_box_cursor_enabled =
				materials.CreateMaterial("text_box_cursor_enabled",
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					{1.0_r, 1.0_r, 1.0_r},
					0.0_r, text_box_cursor_enabled_diffuse, nullptr, nullptr);
			text_box_cursor_enabled->LightingEnabled(false);


			using namespace ion::graphics::utilities;

			//Particle system
			ion::graphics::particles::ParticleSystemManager particle_systems;
			auto rain = particle_systems.CreateParticleSystem("rain");

			auto emitter = rain->CreateEmitter(
				ion::graphics::particles::Emitter::Box(
					"spawner", {0.0_r, 0.0_r, 0.0_r}, vector2::NegativeUnitY, {3.56_r, 0.1_r}, {}, 50.0_r, 0.0_r, {}, 100
				));

			emitter->ParticleVelocity(1.5_r, 2.0_r);
			emitter->ParticleSize(8.0_r, 24.0_r);
			emitter->ParticleMass(1.0_r, 1.0_r);
			emitter->ParticleColor(Color{255, 255, 255, 0.75_r}, color::White);
			emitter->ParticleLifeTime(1.4_sec, 1.4_sec);
			emitter->ParticleMaterial(raindrop);

			//Text
			ion::graphics::fonts::TextManager texts;
			auto pangram =
				texts.CreateText(
					"pangram",
					"The <i>quick</i> <del><font color='saddlebrown'>brown</font></del> fox <b>jumps</b> <sup>over</sup> the <i>lazy</i> dog",
					verdana_12);

			pangram->AppendLine("How <del><font color='olivedrab'>vexingly</font></del> <u>quick</u> <sub>daft</sub> zebras <b>jump</b>!");
			//pangram->Overflow(ion::graphics::fonts::text::TextOverflow::WordWrap);
			//pangram->AreaSize(ion::graphics::utilities::Vector2{300.0_r, 100.0_r});
			pangram->Alignment(ion::graphics::fonts::text::TextAlignment::Left);
			pangram->VerticalAlignment(ion::graphics::fonts::text::TextVerticalAlignment::Top);	
			pangram->DefaultForegroundColor(color::White);

			auto fps =
				texts.CreateText(
					"fps",
					"",
					verdana_36);

			fps->Formatting(ion::graphics::fonts::text::TextFormatting::None);
			fps->DefaultForegroundColor(color::White);

			auto caption_text =
				texts.CreateText(
					"caption",
					"",
					verdana_12);

			caption_text->DefaultForegroundColor(color::White);

			//GUI styles

			ion::graphics::fonts::text::TextBlockStyle caption_style_enabled;
			caption_style_enabled.ForegroundColor = caption_text->DefaultForegroundColor();

			ion::graphics::fonts::text::TextBlockStyle caption_style_disabled;
			caption_style_disabled.ForegroundColor = color::DarkGray;

			ion::graphics::fonts::text::TextBlockStyle caption_style_hovered;
			caption_style_hovered.ForegroundColor = caption_text->DefaultForegroundColor();
			caption_style_hovered.Decoration = ion::graphics::fonts::text::TextDecoration::Underline;

			ion::graphics::fonts::text::TextBlockStyle placeholder_text_style_enabled;
			placeholder_text_style_enabled.ForegroundColor = color::Gray;
			placeholder_text_style_enabled.FontStyle = ion::graphics::fonts::text::TextFontStyle::Italic;

			ion::graphics::fonts::text::TextBlockStyle placeholder_text_style_disabled;
			placeholder_text_style_disabled.ForegroundColor = color::DarkGray;
			placeholder_text_style_disabled.FontStyle = ion::graphics::fonts::text::TextFontStyle::Italic;


			using namespace ion::utilities;

			//Scene manager
			auto scene_manager = engine.CreateSceneManager("main");

			//Viewport
			auto viewport = engine.Target()->GetViewport("main");

			//Frustum
			auto frustum = ion::graphics::render::Frustum::Orthographic(
				Aabb{-1.0_r, 1.0_r}, 1.0_r, 100.0_r, 16.0_r / 9.0_r);
			//auto frustum = ion::graphics::render::Frustum::Perspective(
			//	Aabb{-1.0_r, 1.0_r}, 1.0_r, 100.0_r, 90.0, 16.0_r / 9.0_r);

			//Camera
			auto camera = scene_manager->CreateCamera("main", frustum);
			viewport->ConnectedCamera(camera);

			auto player_camera = scene_manager->CreateCamera("player", frustum);

			//Lights
			auto head_light = scene_manager->CreateLight();
			head_light->Type(ion::graphics::scene::light::LightType::Spotlight);
			head_light->Direction(Vector3{0.0_r, 0.6_r, -0.4_r});
			head_light->AmbientColor(color::Transparent);
			head_light->DiffuseColor(color::White);
			head_light->SpecularColor(color::White);
			head_light->Attenuation(1.0_r, 0.09_r, 0.032_r);
			head_light->Cutoff(math::ToRadians(20.0_r), math::ToRadians(30.0_r));

			auto red_light = scene_manager->CreateLight();
			red_light->Type(ion::graphics::scene::light::LightType::Point);
			red_light->Direction({0.0_r, 0.0_r, -1.0_r});
			red_light->AmbientColor(color::Transparent);
			red_light->DiffuseColor(color::Red);
			red_light->SpecularColor(color::Red);
			red_light->Attenuation(1.0_r, 0.09_r, 0.032_r);
			red_light->Cutoff(math::ToRadians(45.0_r), math::ToRadians(55.0_r));

			auto green_light = scene_manager->CreateLight();
			green_light->Type(ion::graphics::scene::light::LightType::Point);
			green_light->Direction({0.0_r, 0.0_r, -1.0_r});
			green_light->AmbientColor(color::Transparent);
			green_light->DiffuseColor(color::Green);
			green_light->SpecularColor(color::Green);
			green_light->Attenuation(1.0_r, 0.09_r, 0.032_r);
			green_light->Cutoff(math::ToRadians(45.0_r), math::ToRadians(55.0_r));


			//Text
			auto text = scene_manager->CreateText(fps);
			text->AddPass(ion::graphics::render::Pass{/*gui_text_program*/});

			//Particle system
			auto particle_system = scene_manager->CreateParticleSystem(rain);
			particle_system->AddPass(ion::graphics::render::Pass{particle_program});
			particle_system->Get()->StartAll();

			//Sound
			auto player_sound_listener = scene_manager->CreateSoundListener(sound_listener);
			auto red_lamp_flicker = scene_manager->CreateSound(flicker);
			auto green_lamp_flicker = scene_manager->CreateSound(flicker);


			//Model
			auto model = scene_manager->CreateModel();
			model->CreateMesh(ion::graphics::scene::shapes::Sprite{
				{0.0_r, 0.0_r, 0.0_r}, {0.3671875_r, 0.5_r}, ship});
			model->AddPass(ion::graphics::render::Pass{model_program});
			model->BoundingVolumeExtent({{0.3_r, 0.2_r}, {0.7_r, 0.8_r}});
			model->QueryFlags(1);
			model->QueryMask(2 | 4);
			//model->ShowBoundingVolumes(true);

			auto model_star = scene_manager->CreateModel();
			model_star->CreateMesh(ion::graphics::scene::shapes::Sprite{
				{0.0_r, 0.0_r, 0.0_r}, {0.05_r, 0.05_r}, star});
			model_star->AddPass(ion::graphics::render::Pass{});

			auto model_aura = scene_manager->CreateModel();
			auto aura_sprite = model_aura->CreateMesh(ion::graphics::scene::shapes::Sprite{
				{0.0_r, 0.0_r, 0.0_r}, {0.432_r, 0.45_r}, aura});
			aura_sprite->FillColor(Color{255, 255, 255, 0.75_r});
			model_aura->AddPass(ion::graphics::render::Pass{model_program});

			auto background = scene_manager->CreateModel();
			background->CreateMesh(ion::graphics::scene::shapes::Sprite{
				{0.0_r, 0.0_r, 0.0_r}, {1.75_r, 1.75_r}, brick}); //Center
			background->CreateMesh(ion::graphics::scene::shapes::Sprite{
				{-1.75_r, 0.0_r, 0.0_r}, {1.75_r, 1.75_r}, brick}); //Left
			background->CreateMesh(ion::graphics::scene::shapes::Sprite{
				{1.75_r, 0.0_r, 0.0_r}, {1.75_r, 1.75_r}, brick}); //Right
			background->AddPass(ion::graphics::render::Pass{model_program});

			auto clouds = scene_manager->CreateModel();
			clouds->CreateMesh(ion::graphics::scene::shapes::Sprite{
				{-1.0_r, 0.4_r, 0.0_r}, {1.1627182_r, 1.25_r}, cloud}); //Left
			clouds->CreateMesh(ion::graphics::scene::shapes::Sprite{
				{1.0_r, -0.4_r, 0.0_r}, {1.1627182_r, 1.25_r}, cloud}); //Right
			clouds->AddPass(ion::graphics::render::Pass{model_program});

			auto model_spectrum = scene_manager->CreateModel();
			model_spectrum->CreateMesh(ion::graphics::scene::shapes::Sprite{
				{0.0_r, 0.0_r, 0.0_r}, {0.71_r, 0.71_r}, color_spectrum});
			model_spectrum->AddPass(ion::graphics::render::Pass{});

			/*auto box = scene_manager->CreateModel();
			box->CreateMesh(ion::graphics::scene::shapes::Rectangle{{0.25_r, 0.30_r}, color::DeepPink});
			box->AddPass(ion::graphics::render::Pass{model_program});
			box->QueryFlags(2);
			box->QueryMask(1 | 2 | 4);
			//box->ShowBoundingVolumes(true);

			auto box2 = scene_manager->CreateModel();
			box2->CreateMesh(ion::graphics::scene::shapes::Rectangle{{0.30_r, 0.25_r}, color::DarkViolet});
			box2->AddPass(ion::graphics::render::Pass{model_program});
			box2->QueryFlags(2);
			box2->QueryMask(1 | 2 | 4);
			//box2->ShowBoundingVolumes(true);

			auto circle = scene_manager->CreateModel();
			circle->CreateMesh(ion::graphics::scene::shapes::Ellipse{0.25_r, color::Khaki});
			circle->AddPass(ion::graphics::render::Pass{model_program});
			circle->PreferredBoundingVolume(ion::graphics::scene::movable_object::PreferredBoundingVolumeType::BoundingSphere);
			circle->QueryFlags(4);
			circle->QueryMask(1 | 2 | 4);
			//circle->ShowBoundingVolumes(true);

			auto circle2 = scene_manager->CreateModel();
			circle2->CreateMesh(ion::graphics::scene::shapes::Ellipse{0.30_r, color::Orchid});
			circle2->AddPass(ion::graphics::render::Pass{model_program});
			circle2->PreferredBoundingVolume(ion::graphics::scene::movable_object::PreferredBoundingVolumeType::BoundingSphere);
			circle2->QueryFlags(4);
			circle2->QueryMask(1 | 2 | 4);
			//circle2->ShowBoundingVolumes(true);*/


			//Scene graph
			auto scene_graph = engine.CreateSceneGraph("main");

			scene_graph->Gamma(1.0_r);
			scene_graph->AmbientColor(Color::RGB(50, 50, 50));
			scene_graph->FogEffect(ion::graphics::render::Fog::Linear(0.0_r, 2.25_r));
			scene_graph->FogEnabled(false);
			//scene_graph->LightingEnabled(false);


			//GUI
			ion::gui::GuiController controller{scene_graph->RootNode(), gui_sound_channel_group};
			controller.ZOrder(-2.0_r);

			//Theme
			auto theme = controller.CreateTheme("default", scene_manager);

			//Mouse cursor skin
			ion::gui::skins::gui_skin::SkinParts parts;
			parts.Center.Enabled = mouse_cursor_enabled;

			auto mouse_cursor_skin = theme->CreateSkin<ion::gui::controls::GuiMouseCursor>(parts);

			//Tooltip skin
			parts = {};
			parts.Center.Enabled = tooltip_center_enabled;
			parts.Center.FillColor.A(0.9_r);
			parts.Border.Sides.Top.Enabled = tooltip_top_enabled;
			parts.Border.Sides.Bottom.Enabled = tooltip_bottom_enabled;
			parts.Border.Sides.Left.Enabled = tooltip_left_enabled;
			parts.Border.Sides.Right.Enabled = tooltip_right_enabled;
			parts.Border.Corners.TopLeft.Enabled = tooltip_top_left_enabled;
			parts.Border.Corners.TopRight.Enabled = tooltip_top_right_enabled;
			parts.Border.Corners.BottomLeft.Enabled = tooltip_bottom_left_enabled;
			parts.Border.Corners.BottomRight.Enabled = tooltip_bottom_right_enabled;
			
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
			parts.Border.Sides.Bottom.Enabled = button_bottom_enabled;
			parts.Border.Sides.Bottom.Focused = button_bottom_focused;
			parts.Border.Sides.Left.Enabled = button_left_enabled;
			parts.Border.Sides.Left.Focused = button_left_focused;
			parts.Border.Sides.Right.Enabled = button_right_enabled;
			parts.Border.Sides.Right.Focused = button_right_focused;
			parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
			parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
			parts.Border.Corners.TopRight.Enabled = button_top_right_enabled;
			parts.Border.Corners.TopRight.Focused = button_top_right_focused;
			parts.Border.Corners.BottomLeft.Enabled = button_bottom_left_enabled;
			parts.Border.Corners.BottomLeft.Focused = button_bottom_left_focused;
			parts.Border.Corners.BottomRight.Enabled = button_bottom_right_enabled;
			parts.Border.Corners.BottomRight.Focused = button_bottom_right_focused;
			
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
			parts.Border.Sides.Bottom.Enabled = button_bottom_enabled;
			parts.Border.Sides.Bottom.Focused = button_bottom_focused;
			parts.Border.Sides.Left.Enabled = button_left_enabled;
			parts.Border.Sides.Left.Focused = button_left_focused;
			parts.Border.Sides.Right.Enabled = button_right_enabled;
			parts.Border.Sides.Right.Focused = button_right_focused;
			parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
			parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
			parts.Border.Corners.TopRight.Enabled = button_top_right_enabled;
			parts.Border.Corners.TopRight.Focused = button_top_right_focused;
			parts.Border.Corners.BottomLeft.Enabled = button_bottom_left_enabled;
			parts.Border.Corners.BottomLeft.Focused = button_bottom_left_focused;
			parts.Border.Corners.BottomRight.Enabled = button_bottom_right_enabled;
			parts.Border.Corners.BottomRight.Focused = button_bottom_right_focused;
			
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
			parts.Border.Sides.Bottom.Enabled = button_bottom_enabled;
			parts.Border.Sides.Left.Enabled = button_left_enabled;
			parts.Border.Sides.Right.Enabled = button_right_enabled;
			parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
			parts.Border.Corners.TopRight.Enabled = button_top_right_enabled;
			parts.Border.Corners.BottomLeft.Enabled = button_bottom_left_enabled;
			parts.Border.Corners.BottomRight.Enabled = button_bottom_right_enabled;
			
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
			parts.Border.Sides.Bottom.Enabled = button_bottom_enabled;
			parts.Border.Sides.Bottom.Focused = button_bottom_focused;
			parts.Border.Sides.Left.Enabled = button_left_enabled;
			parts.Border.Sides.Left.Focused = button_left_focused;
			parts.Border.Sides.Right.Enabled = button_right_enabled;
			parts.Border.Sides.Right.Focused = button_right_focused;
			parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
			parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
			parts.Border.Corners.TopRight.Enabled = button_top_right_enabled;
			parts.Border.Corners.TopRight.Focused = button_top_right_focused;
			parts.Border.Corners.BottomLeft.Enabled = button_bottom_left_enabled;
			parts.Border.Corners.BottomLeft.Focused = button_bottom_left_focused;
			parts.Border.Corners.BottomRight.Enabled = button_bottom_right_enabled;
			parts.Border.Corners.BottomRight.Focused = button_bottom_right_focused;
			
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
			parts.Border.Sides.Bottom.Enabled = button_bottom_enabled;
			parts.Border.Sides.Left.Enabled = button_left_enabled;
			parts.Border.Sides.Right.Enabled = button_right_enabled;
			parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
			parts.Border.Corners.TopRight.Enabled = button_top_right_enabled;
			parts.Border.Corners.BottomLeft.Enabled = button_bottom_left_enabled;
			parts.Border.Corners.BottomRight.Enabled = button_bottom_right_enabled;
			
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
			parts.Border.Sides.Bottom.Enabled = button_bottom_enabled;
			parts.Border.Sides.Bottom.Focused = button_bottom_focused;
			parts.Border.Sides.Left.Enabled = button_left_enabled;
			parts.Border.Sides.Left.Focused = button_left_focused;
			parts.Border.Sides.Right.Enabled = button_right_enabled;
			parts.Border.Sides.Right.Focused = button_right_focused;
			parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
			parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
			parts.Border.Corners.TopRight.Enabled = button_top_right_enabled;
			parts.Border.Corners.TopRight.Focused = button_top_right_focused;
			parts.Border.Corners.BottomLeft.Enabled = button_bottom_left_enabled;
			parts.Border.Corners.BottomLeft.Focused = button_bottom_left_focused;
			parts.Border.Corners.BottomRight.Enabled = button_bottom_right_enabled;
			parts.Border.Corners.BottomRight.Focused = button_bottom_right_focused;
			
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
			parts.Border.Sides.Bottom.Enabled = button_bottom_enabled;
			parts.Border.Sides.Bottom.Focused = button_bottom_focused;
			parts.Border.Sides.Left.Enabled = button_left_enabled;
			parts.Border.Sides.Left.Focused = button_left_focused;
			parts.Border.Sides.Right.Enabled = button_right_enabled;
			parts.Border.Sides.Right.Focused = button_right_focused;
			parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
			parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
			parts.Border.Corners.TopRight.Enabled = button_top_right_enabled;
			parts.Border.Corners.TopRight.Focused = button_top_right_focused;
			parts.Border.Corners.BottomLeft.Enabled = button_bottom_left_enabled;
			parts.Border.Corners.BottomLeft.Focused = button_bottom_left_focused;
			parts.Border.Corners.BottomRight.Enabled = button_bottom_right_enabled;
			parts.Border.Corners.BottomRight.Focused = button_bottom_right_focused;
			
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
			parts.Border.Sides.Bottom.Enabled = button_bottom_enabled;
			parts.Border.Sides.Bottom.Focused = button_bottom_focused;
			parts.Border.Sides.Left.Enabled = button_left_enabled;
			parts.Border.Sides.Left.Focused = button_left_focused;
			parts.Border.Sides.Right.Enabled = button_right_enabled;
			parts.Border.Sides.Right.Focused = button_right_focused;
			parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
			parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
			parts.Border.Corners.TopRight.Enabled = button_top_right_enabled;
			parts.Border.Corners.TopRight.Focused = button_top_right_focused;
			parts.Border.Corners.BottomLeft.Enabled = button_bottom_left_enabled;
			parts.Border.Corners.BottomLeft.Focused = button_bottom_left_focused;
			parts.Border.Corners.BottomRight.Enabled = button_bottom_right_enabled;
			parts.Border.Corners.BottomRight.Focused = button_bottom_right_focused;
			
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
			parts.Border.Sides.Bottom.Enabled = button_bottom_enabled;
			parts.Border.Sides.Bottom.Focused = button_bottom_focused;
			parts.Border.Sides.Left.Enabled = button_left_enabled;
			parts.Border.Sides.Left.Focused = button_left_focused;
			parts.Border.Sides.Right.Enabled = button_right_enabled;
			parts.Border.Sides.Right.Focused = button_right_focused;
			parts.Border.Corners.TopLeft.Enabled = button_top_left_enabled;
			parts.Border.Corners.TopLeft.Focused = button_top_left_focused;
			parts.Border.Corners.TopRight.Enabled = button_top_right_enabled;
			parts.Border.Corners.TopRight.Focused = button_top_right_focused;
			parts.Border.Corners.BottomLeft.Enabled = button_bottom_left_enabled;
			parts.Border.Corners.BottomLeft.Focused = button_bottom_left_focused;
			parts.Border.Corners.BottomRight.Enabled = button_bottom_right_enabled;
			parts.Border.Corners.BottomRight.Focused = button_bottom_right_focused;
			
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


			//Controls
			window.Cursor(ion::graphics::render::render_window::WindowCursor::None);
			auto mouse_cursor = controller.CreateMouseCursor("mouse_cursor", {});
			mouse_cursor->ZOrder(1.0_r);

			auto tooltip = controller.CreateTooltip("tooltip", {});
			tooltip->ZOrder(0.9_r);

			auto main_frame = controller.CreateFrame("main");
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
			list_box->ItemLayout(ion::gui::controls::gui_list_box::ListBoxItemLayout::Left);
			list_box->IconLayout(ion::gui::controls::gui_list_box::ListBoxIconLayout::Left);
			list_box->ShowIcons(true);
			list_box->AddItems({
				{"My <b>1st</b> <font color='purple'>item</font>"s, asteroid},
				{"My <b>2nd</b> <font color='purple'>item</font>"s, ship},
				{"My <b>3rd</b> <font color='purple'>item</font>"s, asteroid},
				{"My <b>4th</b> <font color='purple'>item</font>"s, star},
				{"My <b>5th</b> <font color='purple'>item</font>"s, asteroid},
				{"My <b>6th</b> <font color='purple'>item</font>"s, aura},
				{"My <b>7th</b> <font color='purple'>item</font>"s, asteroid},
				{"My <b>8th</b> <font color='purple'>item</font>"s, star},
				{"My <b>9th</b> <font color='purple'>item</font>"s, asteroid},
				{"My <b>10th</b> <font color='purple'>item</font>"s, ship}
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

			auto main_frame2 = controller.CreateFrame("main2");
			auto base_panel2 = main_frame2->CreatePanel("base");
			base_panel2->ZOrder(0.1_r);
			auto base_control2 = base_panel2->CreateControl<ion::gui::controls::GuiControl>("control");
			auto sub_panel2 = base_panel2->CreatePanel("sub");
			sub_panel2->ZOrder(0.1_r);
			auto sub_control2 = sub_panel2->CreateControl<ion::gui::controls::GuiControl>("control");

			main_frame->Activate();
			main_frame->Focus();


			//Camera
			auto cam_node = scene_graph->RootNode().CreateChildNode({0.0_r, 0.0_r, 0.0_r});
			cam_node->AttachObject(*camera);

			//Lights
			auto red_light_node = scene_graph->RootNode().CreateChildNode({-1.5_r, -0.75_r, -1.0_r});
			red_light_node->AttachObject(*red_light);

			auto red_lamp_node = red_light_node->CreateChildNode({0.0_r, 0.0_r, -0.8_r});
			red_lamp_node->AttachObject(*red_lamp_flicker);

			auto green_light_node = scene_graph->RootNode().CreateChildNode({1.5_r, 0.75_r, -1.0_r});
			green_light_node->AttachObject(*green_light);

			auto green_lamp_node = green_light_node->CreateChildNode({0.0_r, 0.0_r, -0.8_r});
			green_lamp_node->AttachObject(*green_lamp_flicker);

			//Text
			auto fps_node = scene_graph->RootNode().CreateChildNode({-1.75_r, 0.98_r, -1.5_r});
			fps_node->Scale({-0.5_r, -0.5_r});
			fps_node->AttachObject(*text);

			//Particles
			auto particle_node = scene_graph->RootNode().CreateChildNode({0.0_r, 1.0_r, -1.75_r}, vector2::NegativeUnitY);
			particle_node->AttachObject(*particle_system);

			//Models
			auto player_node = scene_graph->RootNode().CreateChildNode({0.0_r, -0.65_r, -1.8_r});

			auto ship_node = player_node->CreateChildNode({0.0_r, 0.0_r, 0.0_r});
			ship_node->AttachObject(*model);
			ship_node->AttachObject(*player_sound_listener);

			auto star_node = ship_node->CreateChildNode({0.15_r, 0.2_r, 0.1_r});
			star_node->AttachObject(*model_star);

			auto aura_node = ship_node->CreateChildNode({0.0_r, -0.05_r, -0.1_r});
			aura_node->InheritRotation(false);
			aura_node->AttachObject(*model_aura);

			auto background_node = scene_graph->RootNode().CreateChildNode({0.0_r, 0.0_r, -2.25_r});
			background_node->AttachObject(*background);

			auto cloud_node = scene_graph->RootNode().CreateChildNode({0.0_r, 0.0_r, -1.6_r});
			cloud_node->AttachObject(*clouds);

			auto spectrum_node = scene_graph->RootNode().CreateChildNode({1.4_r, -0.6_r, -1.1_r});
			spectrum_node->AttachObject(*model_spectrum);

			//auto button_node = scene_graph->RootNode().CreateChildNode({0.0_r, 0.5_r, -2.0_r});
			//button_node->AttachObject(*button_model);

			/*auto box_base_node = scene_graph->RootNode().CreateChildNode({-0.40_r, 0.35_r, -2.2_r});

			auto box_node = box_base_node->CreateChildNode({-0.05_r, -0.10_r, 0.0});
			box_node->AttachObject(*box);

			auto box2_node = box_base_node->CreateChildNode({0.05_r, 0.10_r, 0.0});
			box2_node->AttachObject(*box2);

			auto circle_node = scene_graph->RootNode().CreateChildNode({0.35_r, 0.25_r, -2.2_r});
			circle_node->AttachObject(*circle);

			auto circle2_node = scene_graph->RootNode().CreateChildNode({0.45_r, 0.45_r, -2.2_r});
			circle2_node->AttachObject(*circle2);*/

			//Head light
			auto light_node = ship_node->CreateChildNode({0.0_r, -0.15_r, -0.05_r}, vector2::UnitY, false);
			light_node->AttachObject(*head_light);

			//Player camera
			auto player_cam_node = player_node->CreateChildNode({0.0_r, 0.0_r, 1.8_r});
			player_cam_node->AttachObject(*player_camera);


			//Node animations
			using namespace ion::graphics::scene::graph::animations;

			auto scaler = cloud_node->CreateAnimation("scaler");
			scaler->AddScaling(0.25_r, 10.0_sec, 0.0_sec, node_animation::MotionTechniqueType::Sigmoid);
			scaler->AddScaling(-0.25_r, 10.0_sec, 10.0_sec, node_animation::MotionTechniqueType::Sigmoid);
			scaler->Start();

			auto mover = ship_node->CreateAnimation("mover");
			mover->AddTranslation({0.0_r, 0.02_r, 0.0_r}, 2.0_sec);
			mover->AddTranslation({0.02_r, -0.02_r, 0.0_r}, 2.0_sec, 2.0_sec);
			mover->AddTranslation({-0.02_r, -0.02_r, 0.0_r}, 2.0_sec, 4.0_sec);
			mover->AddTranslation({-0.02_r, 0.02_r, 0.0_r}, 2.0_sec, 6.0_sec);
			mover->AddTranslation({0.02_r, 0.02_r, 0.0_r}, 2.0_sec, 8.0_sec);
			mover->AddTranslation({0.0_r, -0.02_r, 0.0_r}, 2.0_sec, 10.0_sec);

			auto rotator = ship_node->CreateAnimation("rotator");
			rotator->AddRotation(math::ToRadians(-2.5_r), 2.0_sec, 2.0_sec);
			rotator->AddRotation(math::ToRadians(2.5_r), 2.0_sec, 4.0_sec);
			rotator->AddRotation(math::ToRadians(2.5_r), 2.0_sec, 6.0_sec);
			rotator->AddRotation(math::ToRadians(-2.5_r), 2.0_sec, 8.0_sec);

			auto idle = ship_node->CreateAnimationGroup("idle");
			idle->Add(mover);
			idle->Add(rotator);

			auto timeline = ship_node->CreateTimeline(1.0_r, false);
			timeline->Attach(idle);


			//Game
			game.scene_graph = scene_graph;	
			game.viewport = viewport;
			game.controller = &controller;
			game.sound_manager = &sounds;
			game.fps = text;
			game.player_node = player_node;
			game.light_node = light_node;
			game.aura_node = aura_node;
			game.camera = camera;
			game.player_camera = player_camera;
			game.idle = timeline;


			//Engine
			engine.FrameEvents().Subscribe(game);
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