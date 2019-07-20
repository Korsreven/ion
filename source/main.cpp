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

#include "IonSingleton.h"

#include "adaptors/IonFlatMap.h"
#include "adaptors/IonFlatSet.h"
#include "adaptors/ranges/IonBasicIterable.h"
#include "adaptors/ranges/IonDereferenceIterable.h"
#include "adaptors/iterators/IonDereferenceIterator.h"
#include "adaptors/iterators/IonFlatMapIterator.h"
#include "adaptors/iterators/IonFlatSetIterator.h"

#include "events/IonCallback.h"
#include "events/IonRecurringCallback.h"
#include "events/IonInputController.h"
#include "events/listeners/IonFrameListener.h"
#include "events/listeners/IonKeyListener.h"
#include "events/listeners/IonListener.h"
#include "events/listeners/IonListenerInterface.h"
#include "events/listeners/IonMouseListener.h"
#include "events/listeners/IonResourceListener.h"
#include "events/listeners/IonWindowListener.h"

#include "graphics/utilities/IonAabb.h"
#include "graphics/utilities/IonColor.h"
#include "graphics/utilities/IonMatrix3.h"
#include "graphics/utilities/IonObb.h"
#include "graphics/utilities/IonSphere.h"
#include "graphics/utilities/IonVector2.h"

#include "resources/IonResource.h"
#include "resources/IonResourceManager.h"
#include "resources/files/IonFileResource.h"
#include "resources/files/IonFileResourceLoader.h"
#include "resources/files/repositories/IonAudioRepository.h"
#include "resources/files/repositories/IonFontRepository.h"
#include "resources/files/repositories/IonImageRepository.h"
#include "resources/files/repositories/IonShaderRepository.h"
#include "resources/files/repositories/IonVideoRepository.h"

#include "script/IonScriptCompiler.h"
#include "script/IonScriptError.h"
#include "script/IonScriptTree.h"
#include "script/IonScriptValidator.h"
#include "script/utilities/IonParseUtility.h"

#include "system/IonSystemUtility.h"
#include "system/IonSystemWindow.h"
#include "system/events/IonSystemInput.h"
#include "system/events/listeners/IonSystemInputListener.h"
#include "system/events/listeners/IonSystemMessageListener.h"

#include "timers/IonAsyncTimerManager.h"
#include "timers/IonTimer.h"
#include "timers/IonTimerManager.h"
#include "timers/IonStopwatch.h"

#include "types/IonTypes.h"
#include "types/IonTypeCasts.h"
#include "types/IonTypeTraits.h"

#include "utilities/IonCodec.h"
#include "utilities/IonConvert.h"
#include "utilities/IonConvertV2.h"
#include "utilities/IonCrypto.h"
#include "utilities/IonFileUtility.h"
#include "utilities/IonMath.h"
#include "utilities/IonRandom.h"
#include "utilities/IonStringUtility.h"

#ifdef _WIN32

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ion::types::type_literals;
using namespace ion::graphics::utilities::color::literals;
using namespace ion::graphics::utilities::vector2::literals;
using namespace ion::utilities::file::literals;
using namespace ion::utilities::math::literals;


struct Owner
{
};

struct Ownee
{
	Owner &owner;
};

struct Texture
{
	int id = 0;
};

struct Sprite : ion::events::listeners::ResourceListener<Texture>
{
	void ResourceCreated(Texture &resource) noexcept override
	{
		resource;
	}

	void ResourceRemoved(Texture &resource) noexcept override
	{
		resource;
	}

	void Subscribed(ion::events::listeners::ListenerInterface<ion::events::listeners::ResourceListener<Texture>> &listener_interface) noexcept override
	{
		listener_interface;
	}

	void Unsubscribed(ion::events::listeners::ListenerInterface<ion::events::listeners::ResourceListener<Texture>> &listener_interface) noexcept override
	{
		listener_interface;
	}
};

struct Test : ion::system::events::listeners::MessageListener
{
	bool MessageReceived(HWND, UINT, WPARAM, LPARAM) noexcept override
	{
		return false;
	}
};

void OnTick(ion::timers::Timer &ticked_timer)
{
	ticked_timer;
}

//Entry point for windows 32/64 bit
int WINAPI WinMain([[maybe_unused]] _In_ HINSTANCE instance,
				   [[maybe_unused]] _In_opt_ HINSTANCE prev_instance,
				   [[maybe_unused]] _In_ LPSTR cmd_line,
				   [[maybe_unused]] _In_ int show_cmd)
{
	/*
		Initializing Ion Engine
		(should eventually be located in IonCore)
	*/

	auto application_path = ion::system::utilities::ApplicationPath();

	if (!application_path)
		return 1;
	
	application_path->remove_filename();
	
	if (!ion::utilities::file::CurrentPath(*application_path))
		return 1;
	

	/*
		Test code
	*/

	auto args = ion::system::utilities::CommandLineArguments();
	auto repetitions = 1;

	//Compile script
	{
		ion::script::ScriptCompiler script;
		ion::script::CompileError compile_error;

		ion::timers::Stopwatch stopwatch;
		auto compile_elapsed = std::chrono::microseconds{0};
		std::optional<ion::script::ScriptTree> compiled_tree;

		for (auto i = 0; i < repetitions; ++i)
		{
			stopwatch.Start();
			compiled_tree = script.Compile("bin/script.ion", compile_error);
			stopwatch.Stop();

			if (compile_elapsed.count() == 0 ||
				stopwatch.ElapsedMicroseconds() < compile_elapsed)
					compile_elapsed = stopwatch.ElapsedMicroseconds();

			if (compile_error)
				break;
		}

		auto deserialize_elapsed = std::chrono::microseconds{0};
		std::optional<ion::script::ScriptTree> deserialized_tree;

		//Serialize tree
		if (compiled_tree)
		{
			auto component = ion::script::script_validator::ClassDefinition::Create("component")
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
			auto okay = validator.Validate(*compiled_tree, validate_error);

			if (validate_error)
			{
				//What?
			}


			auto tree_bytes = compiled_tree->Serialize();
			ion::utilities::file::Save("bin/script.obj",
				{reinterpret_cast<char*>(std::data(tree_bytes)), std::size(tree_bytes)},
				ion::utilities::file::FileSaveMode::Binary);

			//Load object file

			std::string bytes;
			ion::utilities::file::Load("bin/script.obj", bytes,
				ion::utilities::file::FileLoadMode::Binary);

			//Deserialize

			for (auto i = 0; i < repetitions; ++i)
			{
				stopwatch.Start();
				deserialized_tree = ion::script::ScriptTree::Deserialize(bytes);
				stopwatch.Stop();

				if (deserialize_elapsed.count() == 0 ||
					stopwatch.ElapsedMicroseconds() < deserialize_elapsed)
						deserialize_elapsed = stopwatch.ElapsedMicroseconds();

				if (!deserialized_tree)
					break;
			}
		}

		//Output

		auto message = compile_error ?
			ion::utilities::string::Concat(compile_error.Condition.message(), " (line number ", compile_error.LineNumber, ")") :
			"Script has been compiled successfully!";

		auto output = ion::utilities::string::Concat(
			"- ION script compiler output -\n\n",
			"Compile time: \t\t", compile_elapsed.count(), "mu\n"
			"Deserialize time: \t", deserialize_elapsed.count() > 0 ?
				ion::utilities::convert::ToString(deserialize_elapsed.count()) + "mu" : "-", "\n"
			"Compile output: \t", message);

		if (compiled_tree)
		{
			output += "\n\n\n- Tree printed using DFS pre-traversal -\n";

			for (auto [object, parent, depth] : compiled_tree->DepthFirst())
			{
				output += "\n" + std::string(depth, '\t') + "[object : " + object.Name() + "]";

				for (auto &property : object.Properties())
					output += "\n" + std::string(depth + 1, '\t') + "property : " + property.Name();
			}
		}

		ion::utilities::file::Save("bin/output.txt", output);
	}

	/*{
		std::string result_str;
		std::string result_str_v2;
		std::optional<real> result;
		std::optional<real> result_v2;

		auto stopwatch = ion::timers::Stopwatch::StartNew();
		for (auto i = 0; i < 1'000'000; ++i)
			result_str = ion::utilities::convert::ToString(3.14);
		stopwatch.Stop();
		auto elapsed_str = stopwatch.Elapsed();

		stopwatch.Start();
		for (auto i = 0; i < 1'000'000; ++i)
			result_str_v2 = ion::utilities::experimental::convert::ToString(3.14);
		stopwatch.Stop();
		auto elapsed_str_v2 = stopwatch.Elapsed();

		

 		stopwatch.Start();
		for (auto i = 0; i < 1'000'000; ++i)
			result = ion::utilities::convert::To<real>("3.14");
		stopwatch.Stop();
		auto elapsed = stopwatch.Elapsed();

		stopwatch.Start();
		for (auto i = 0; i < 1'000'000; ++i)
			result_v2 = ion::utilities::experimental::convert::To<real>("3.14");
		stopwatch.Stop();
		auto elapsed_v2 = stopwatch.Elapsed();
	}*/

	/*ion::timers::TimerManager timer_manager;
	auto &timer = timer_manager.CreateTimer(std::chrono::seconds{1});

	auto timer_handle = std::make_unique<ion::timers::Timer>(std::chrono::seconds{2});
	auto &timer2 = timer_manager.Adopt(std::move(timer_handle));

	ion::timers::AsyncTimerManager async_timer_manager;
	auto &async_timer = async_timer_manager.CreateTimer(std::chrono::seconds{4}, OnTick);
	async_timer.Start();*/

	/*{
		std::vector<real> empty;
		std::vector samples{1.0_r, 4.0_r, 6.0_r, 7.0_r, 4.0_r, 2.0_r};

		auto empty_sum = ion::utilities::math::Sum(empty);
		auto sum = ion::utilities::math::Sum(samples);

		auto empty_mean = ion::utilities::math::Mean(empty);
		auto mean = ion::utilities::math::Mean(samples);

		auto empty_median = ion::utilities::math::Median(empty);
		auto median = ion::utilities::math::Median(samples);

		auto empty_mode = ion::utilities::math::Mode(empty);
		auto mode = ion::utilities::math::Mode(samples);

		auto empty_range = ion::utilities::math::Range(empty);
		auto range = ion::utilities::math::Range(samples);
	}*/

	/*ion::adaptors::FlatMap<std::string, int> flat_map;
	flat_map.insert({{"d", 3}, {"a", 1}});
	flat_map.insert({"b", 2});
	flat_map.erase("d");
	flat_map["d"] = 10;
	flat_map.emplace_hint(std::end(flat_map), "e", 3);

	for (auto &value : flat_map)
	{
		value.second = 0;
	}

	ion::adaptors::FlatSet<int> flat_set;
	flat_set.insert({1, 2, 4, 5});
	flat_set.insert(std::begin(flat_set) + 2, 5);

	ion::resources::files::repositories::AudioRepository audio_repository{ion::resources::files::repositories::file_repository::NamingConvention::FileName};
	ion::resources::files::repositories::FontRepository font_repository{ion::resources::files::repositories::file_repository::NamingConvention::FileName};
	ion::resources::files::repositories::ImageRepository image_repository{ion::resources::files::repositories::file_repository::NamingConvention::FileName};
	ion::resources::files::repositories::ShaderRepository shader_repository{ion::resources::files::repositories::file_repository::NamingConvention::FileName};
	ion::resources::files::repositories::VideoRepository video_repository{ion::resources::files::repositories::file_repository::NamingConvention::FileName};

	ion::resources::files::FileResourceLoader file_resource_loader;
	file_resource_loader.Attach(audio_repository);
	file_resource_loader.Attach(font_repository);
	file_resource_loader.Attach(image_repository);
	file_resource_loader.Attach(shader_repository);
	file_resource_loader.Attach(video_repository);
	
	file_resource_loader.LoadDirectory("bin", ion::utilities::file::DirectoryIteration::Recursive);
	//file_resource_loader.CompileDataFile("bin/resources.dat");*/

	{
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
	}
	
	
	{
		auto concat_result = ion::utilities::string::Concat(10, ","sv, 'A', ","s, 3.14, ",");
		auto join_result = ion::utilities::string::Join(","sv, 10, 'A', 3.14, "string"s, "char[]");
		auto format_result = ion::utilities::string::Format("{0}, {1} and {2 : 00.0000} + {3}, {4} and {5}", 10, 'A', 3.14, "string_view"sv, "string"s, "char[]");
	}

	return 0;
}

#else

//Entry point for non windows systems
int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
	return 0;
}

#endif