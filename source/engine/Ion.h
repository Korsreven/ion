/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	engine
File:	Ion.h
-------------------------------------------
*/

#ifndef ION_H
#define ION_H

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
#include "graphics/render/IonRenderer.h"
#include "graphics/render/IonRenderPass.h"
#include "graphics/render/IonRenderPrimitive.h"
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
#include "graphics/scene/shapes/IonAnimatedSprite.h"
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
#include "gui/controls/IonGuiImage.h"
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
#include "managed/IonObjectRegister.h"
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
#include "script/interfaces/IonAnimationScriptInterface.h"
#include "script/interfaces/IonFontScriptInterface.h"
#include "script/interfaces/IonFrameSequenceScriptInterface.h"
#include "script/interfaces/IonGuiScriptInterface.h"
#include "script/interfaces/IonGuiThemeScriptInterface.h"
#include "script/interfaces/IonMaterialScriptInterface.h"
#include "script/interfaces/IonParticleSystemScriptInterface.h"
#include "script/interfaces/IonScriptInterface.h"
#include "script/interfaces/IonSceneScriptInterface.h"
#include "script/interfaces/IonShaderScriptInterface.h"
#include "script/interfaces/IonShaderLayoutScriptInterface.h"
#include "script/interfaces/IonShaderProgramScriptInterface.h"
#include "script/interfaces/IonSoundScriptInterface.h"
#include "script/interfaces/IonTextScriptInterface.h"
#include "script/interfaces/IonTextureScriptInterface.h"
#include "script/interfaces/IonTypeFaceScriptInterface.h"
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
#include "types/IonSuppressCopy.h"
#include "types/IonSuppressMove.h"
#include "types/IonTrace.h"
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


/*
	Main page (for doxygen)
*/

/**
	@mainpage API Reference

	@section intro Introduction
	<a href="https://github.com/Korsreven/ion">ION engine</a> is a fast, lightweight and fully functional 2D game engine using OpenGL.<br><br>
	With the combination of an easy to use high-level scene graph, a blazingly fast low-level batch renderer, fully functional GUI, integrated scripting and powerful utilities, ION Engine has <b><i>everything you need</i></b> to create games right out of the box.
	To learn some more about the engine, please check out the list of <a href="https://github.com/Korsreven/ion/blob/dev/FEATURES.md">main features</a> and the latest online version of the <a href="https://korsreven.github.io/">API reference</a>.
	No prior knowledge of OpenGL is required to get started.

	@section source Source Code
	ION engine is written in modern C++ (with some GLSL) and consists of around 115K lines of code.
	The development has been done exclusively using Visual Studio on a Microsoft Windows PC.
	The source code itself is platform independent and should be portable (with some coding effort).

	@section project Project Status
	ION engine has been actively under development for a period of 5 years (2017 to 2022).
	Now that the engine is stable and feature complete, development will begin to slow down a bit while I start working on some games.
	The source code will be maintained from time to time, when new bugs are found or new features are needed.
	Tutorials and code examples will become available at a later date.

	@section author Author
	The ION game engine is written by Jan Ivar Goli.

	@section license License
	The ION game engine is licensed under the <a href="https://opensource.org/licenses/MIT/">MIT license</a>.
*/


/*
	Namespace hierarchy (for doxygen)
*/

///@brief Root namespace containing everything in the engine
namespace ion
{
	///@brief Namespace containing adaptor functionality
	namespace adaptors
	{
		///@brief Namespace containing different adaptor iterators
		namespace iterators
		{
		} //iterators

		///@brief Namespace containing different container ranges
		namespace ranges
		{
		} //ranges
	} //adaptors

	///@brief Namespace containing asset functionality
	namespace assets
	{
		///@brief Namespace containing asset repository functionality
		namespace repositories
		{
		} //repositories
	} //assets

	///@brief Namespace containing callback and event functionality
	namespace events
	{
		///@brief Namespace containing event listener functionality
		namespace listeners
		{
		} //listeners
	} //events

	///@brief Namespace containing graphics functionality
	namespace graphics
	{
		///@brief Namespace containing font and text functionality
		namespace fonts
		{
		} //fonts

		///@brief Namespace containing material functionality
		namespace materials
		{
		} //materials

		///@brief Namespace containing particle system functionality
		namespace particles
		{
			///@brief Namespace containing different affectors
			namespace affectors
			{
			} //affectors
		} //particles

		///@brief Namespace containing rendering functionality
		namespace render
		{
			///@brief Namespace containing vertex object functionality
			namespace vertex
			{
			} //vertex
		} //render

		///@brief Namespace containing scene functionality
		namespace scene
		{
			///@brief Namespace containing scene graph and node functionality
			namespace graph
			{
				///@brief Namespace containing node animation functionality
				namespace animations
				{
				} //animations
			} //graph

			///@brief Namespace containing scene query functionality for collision detection
			namespace query
			{
			} //query

			///@brief Namespace containing different shapes
			namespace shapes
			{
			} //shapes
		} //scene

		///@brief Namespace containing shader functionality
		namespace shaders
		{
			///@brief Namespace containing attribute and uniform functionality
			namespace variables
			{
			} //variables
		} //shaders

		///@brief Namespace containing animation and texture functionality
		namespace textures
		{
		} //textures

		///@brief Namespace containing commonly used graphics utilities
		namespace utilities
		{
		} //utilities
	} //graphics

	///@brief Namespace containing GUI functionality
	namespace gui
	{
		///@brief Namespace containing different GUI controls
		namespace controls
		{
		} //controls

		///@brief Namespace containing GUI skin and theme functionality
		namespace skins
		{
		} //skins
	} //gui

	///@brief Namespace containing managed object functionality
	namespace managed
	{
	} //managed

	///@brief Namespace containing memory and smart pointer functionality
	namespace memory
	{
	} //memory

	///@brief Namespace containing parallel and worker functionality
	namespace parallel
	{
	} //parallel

	///@brief Namespace containing resource functionality
	namespace resources
	{
	} //resources

	///@brief Namespace containing scripting functionality
	namespace script
	{
		///@brief Namespace containing predefined script interfaces
		namespace interfaces
		{
		} //interfaces
	} //script

	///@brief Namespace containing sound functionality
	namespace sounds
	{
	} //sounds

	///@brief Namespace containing system specific functionality
	namespace system
	{
		///@brief Namespace containing system specific event listeners
		namespace events::listeners
		{
		} //events::listeners
	} //system

	///@brief Namespace containing stopwatch and timer functionality
	namespace timers
	{
	} //timers

	///@brief Namespace containing commonly used types and type traits
	namespace types
	{
	} //types

	///@brief Namespace containing unmanaged object functionality
	namespace unmanaged
	{
	} //unmanaged

	///@brief Namespace containing commonly used general utilities
	namespace utilities
	{
	} //utilities
} //ion

#endif