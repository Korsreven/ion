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
#include "adaptors/ranges/IonBasicIterable.h"
#include "adaptors/ranges/IonDereferenceIterable.h"
#include "adaptors/iterators/IonDereferenceIterator.h"
#include "adaptors/iterators/IonFlatMapIterator.h"
#include "adaptors/iterators/IonFlatSetIterator.h"

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
#include "graphics/particles/IonEmitter.h"
#include "graphics/particles/IonEmitterFactory.h"
#include "graphics/particles/IonParticle.h"
#include "graphics/particles/IonParticleSystem.h"
#include "graphics/particles/IonParticleSystemManager.h"
#include "graphics/particles/affectors/IonAffector.h"
#include "graphics/particles/affectors/IonAffectorFactory.h"
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
#include "graphics/scene/IonCamera.h"
#include "graphics/scene/IonSceneManager.h"
#include "graphics/shaders/IonShader.h"
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
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "graphics/utilities/IonVector2.h"

#include "managed/IonManagedObject.h"
#include "managed/IonObjectManager.h"
#include "managed/IonObjectObserver.h"
#include "managed/IonObservedObject.h"

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

struct SpriteContainer : ion::events::Listenable<ion::events::listeners::ResourceListener<ion::graphics::textures::Texture, ion::graphics::textures::TextureManager>>
{
};

struct Sprite : ion::events::listeners::ResourceListener<ion::graphics::textures::Texture, ion::graphics::textures::TextureManager>
{
	void ResourcePrepared(ion::graphics::textures::Texture &resource) noexcept override
	{
		resource;
	}

	void ResourceLoaded(ion::graphics::textures::Texture &resource) noexcept override
	{
		resource;
	}

	void ResourceUnloaded(ion::graphics::textures::Texture &resource) noexcept override
	{
		resource;
	}

	void ResourceFailed(ion::graphics::textures::Texture &resource) noexcept override
	{
		resource;
	}

	void ResourceLoadingStateChanged(ion::graphics::textures::Texture &resource) noexcept override
	{
		resource;
	}


	void ObjectCreated(ion::graphics::textures::Texture &resource) noexcept override
	{
		resource;
	}

	void ObjectRemoved(ion::graphics::textures::Texture &resource) noexcept override
	{
		resource;
	}


	void Subscribed(ion::events::Listenable<ion::events::listeners::ResourceListener<ion::graphics::textures::Texture, ion::graphics::textures::TextureManager>> &listenable) noexcept override
	{
		listenable;
	}

	void Unsubscribed(ion::events::Listenable<ion::events::listeners::ResourceListener<ion::graphics::textures::Texture, ion::graphics::textures::TextureManager>> &listenable) noexcept override
	{
		listenable;
	}
};

struct FrameTest :
	ion::events::listeners::FrameListener
{
	bool FrameStarted(duration time) noexcept override
	{
		time;
		return true;
	}

	bool FrameEnded(duration time) noexcept override
	{
		time;
		return true;
	}
};

struct InputTest :
	ion::events::listeners::KeyListener,
	ion::events::listeners::MouseListener
{
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


	void MousePressed(ion::events::listeners::MouseButton button, ion::graphics::utilities::Vector2 position) noexcept override
	{
		button;
		position;
	}

	void MouseReleased(ion::events::listeners::MouseButton button, ion::graphics::utilities::Vector2 position) noexcept override
	{
		button;
		position;
	}

	void MouseMoved(ion::graphics::utilities::Vector2 position) noexcept override
	{
		position;
	}

	void MouseWheelRolled(int delta, ion::graphics::utilities::Vector2 position) noexcept override
	{
		delta;
		position;
	}
};

void OnTick(ion::timers::Timer &ticked_timer)
{
	ticked_timer;
}

auto Concat(std::string x, std::string y)
{
	return x + y;
}

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

	FrameTest frame_test;
	InputTest input_test;

	auto exit_code = 0;
	{	
		ion::Engine engine;

		auto &window = engine.RenderTo(
			ion::graphics::render::RenderWindow::Resizable("ION engine", {1280.0_r, 720.0_r}),
			ion::graphics::utilities::Aabb{-1.0_r, 1.0_r}, -1.0_r, 1.0_r, 16.0_r / 9.0_r);
		window.MinSize(ion::graphics::utilities::Vector2{640.0_r, 360.0_r});

		if (engine.Initialize())
		{
			//Check API support
			{
				auto newest_gl = ion::graphics::gl::HasGL(ion::graphics::gl::Version::v4_6);
				auto blend_func_separate = ion::graphics::gl::BlendFuncSeparate_Support();
				auto frame_buffer_object = ion::graphics::gl::FrameBufferObject_Support();
				auto multi_texture = ion::graphics::gl::MultiTexture_Support();
				auto point_sprite = ion::graphics::gl::PointSprite_Support();
				auto shader = ion::graphics::gl::Shader_Support();
				auto npot = ion::graphics::gl::TextureNonPowerOfTwo_Support();
				auto vertex_buffer_object = ion::graphics::gl::VertexBufferObject_Support();
				auto max_texture_size = ion::graphics::gl::MaxTextureSize();
				auto max_texture_units = ion::graphics::gl::MaxTextureUnits();
				auto break_point = false;
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
			auto &rikku_texture = textures.CreateTexture("rikku", "rikku.png");
			auto &rikku_np2_texture = textures.CreateTexture("rikku_np2", "rikku_np2.png");
			auto &cloud_texture = textures.CreateTexture("cloud", "cloud.png");
			auto &cloud_np2_texture = textures.CreateTexture("cloud_np2", "cloud_np2.png");
			auto &background_texture = textures.CreateTexture("background", "background.jpg");
			auto &background_np2_texture = textures.CreateTexture("background_np2", "background_np2.jpg");
			textures.LoadAll(/*ion::resources::resource_manager::EvaluationStrategy::Lazy*/);

			//while (!textures.Loaded());

			//Frame sequences
			ion::graphics::textures::FrameSequenceManager frame_sequences;
			auto &frame_sequence = frame_sequences.CreateFrameSequence(
				"misc", rikku_texture, cloud_texture, background_texture);

			//Animation
			ion::graphics::textures::AnimationManager animations;
			auto animation = animations.CreateAnimation(ion::graphics::textures::Animation::Looping(
				"alternate", frame_sequence, 6.0_sec, ion::graphics::textures::animation::PlaybackDirection::Alternate));
			animation.RepeatCount(1);
			animation.JumpForward(7.5_sec);
			animation.RepeatCount({});
			animation.RepeatCount(1);

			//Shaders
			ion::graphics::shaders::ShaderManager shaders;
			shaders.CreateRepository(std::move(shader_repository));
			shaders.LogLevel(ion::graphics::shaders::shader_manager::InfoLogLevel::Error);
			auto &vert_shader = shaders.CreateShader("default_particle_vert", "default_particle.vert");
			auto &frag_shader = shaders.CreateShader("default_particle_frag", "default_particle.frag");
			shaders.LoadAll(/*ion::resources::resource_manager::EvaluationStrategy::Lazy*/);

			//while (!shaders.Loaded());

			//Font
			ion::graphics::fonts::FontManager fonts;
			fonts.CreateRepository(std::move(font_repository));
			auto &verdana_regular_12 = fonts.CreateFont("verdana_regular_12", "verdana.ttf", 12);
			auto &verdana_bold_12 = fonts.CreateFont("verdana_bold_12", "verdanab.ttf", 12);
			auto &verdana_italic_12 = fonts.CreateFont("verdana_italic_12", "verdanai.ttf", 12);
			auto &verdana_bold_italic_12 = fonts.CreateFont("verdana_bold_italic_12", "verdanaz.ttf", 12);
			fonts.LoadAll(/*ion::resources::resource_manager::EvaluationStrategy::Lazy*/);

			//while (!fonts.Loaded());

			//Font utility
			auto fmt_elements = ion::graphics::fonts::utilities::FormattedElements(
				"<p style=\"font-color:black;\">Hello<b><font color=\"red\">Roses<br><i><u style=\"font-decoration-color:black;\">are</u> red</i></font></b>World!</p>"
			);

			auto char_size_pixels = ion::graphics::fonts::utilities::MeasureCharacter('A', verdana_regular_12);
			auto str_size_pixels = ion::graphics::fonts::utilities::MeasureString("Hello world!", verdana_regular_12);
			auto truncated_str = ion::graphics::fonts::utilities::TruncateString("Hello world!", 90, verdana_regular_12);
			auto word_wrapped_str = ion::graphics::fonts::utilities::WordWrap("The quick brown fox jumps over the lazy dog", 100, verdana_regular_12);

			//Type face
			ion::graphics::fonts::TypeFaceManager type_faces;
			auto &verdana_12 = 
				type_faces.CreateTypeFace(
					"verdana_12",
					verdana_regular_12,
					verdana_bold_12,
					verdana_italic_12,
					verdana_bold_italic_12);

			//EXAMPLE end

			engine.Subscribe(frame_test);

			if (auto input = engine.Input(); input)
			{
				input->KeyEvents().Subscribe(input_test);
				input->MouseEvents().Subscribe(input_test);
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


	using namespace ion::graphics::shaders::variables;

	Uniform<float> u{"my_uniform"};
	glsl::uniform<float> &glsl_u = u.Get();

	UniformVariable &u_var = u;
	u_var.Visit(
		[](glsl::uniform<float> &x) noexcept
		{
			x = 6.28f;
			auto break_point = true;
		},
		[](auto&&) noexcept
		{
			//Everything else
		});
	

	struct vertex
	{
		float x, y;
	};

	vertex vertices[]{{1.0f, 2.0f}, {2.0f, 3.0f}, {3.0f, 4.0f}, {4.0f, 5.0f}};

	Attribute<glsl::vec2> a{"my_attribute"};
	glsl::attribute<glsl::vec2> &glsl_a = a.Get();
	
	AttributeVariable &a_var = a;
	a_var.Visit(
		[&](glsl::attribute<glsl::vec2> &x) noexcept
		{	
			x.VertexData(vertices, sizeof(vertex));
			auto a = x[0].XY();
			auto b = x[1].XY();
			auto c = x[2].XY();
			auto d = x[3].XY();
			auto break_point = true;
		},
		[](auto&&) noexcept
		{
			//Everything else
		});

	auto break_point = true;


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