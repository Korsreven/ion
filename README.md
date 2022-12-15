![logo](docs/logo.png?raw=true)

# ION Engine 1.0

## Introduction
[ION engine](https://github.com/Korsreven/ion) is a fast, lightweight and fully functional 2D game engine using OpenGL.  

With the combination of an easy to use high-level scene graph, a blazingly fast low-level batch renderer, fully functional GUI, integrated scripting and powerful utilities, ION Engine has ***everything you need*** to create games right out of the box. To learn some more about the engine, please check out the list of [main features](FEATURES.md) and the latest online version of the [API reference](https://korsreven.github.io/). No prior knowledge of OpenGL is required to get started.

## Source Code
ION engine is written in modern C++ (with some GLSL) and consists of around 115K lines of code. The development has been done exclusively using Visual Studio on a Microsoft Windows PC. The source code itself is platform independent and should be portable (with some coding effort).

## Project Status
ION engine has been actively under development for a period of 5 years (2017 to 2022). Now that the engine is stable and feature complete, development will begin to slow down a bit while I start working on some games. The source code will be maintained from time to time, when new bugs are found or new features are needed. Tutorials and code examples will become available at a later date.

## Dependencies
One of the things I enjoy the most when writing code, is actually writing most of the code myself. But there is no rule without exception... ION engine is using four different third party libraries.
* FMOD v2.01.20 (https://www.fmod.com/)
* FreeImage v3.18.0 (https://freeimage.sourceforge.io/)
* FreeType v2.12.1 (https://www.freetype.org/)
* GLEW v2.1.0 (http://glew.sourceforge.net/)

## How to get started
* Download and install [Microsoft Visual Studio 2022](https://visualstudio.microsoft.com/vs/community/)
    * Remember to include ***Desktop development with C++*** before installing
* Clone the [Korsreven/ion](https://github.com/Korsreven/ion) repository
* Create directories `ion/binaries/Debug` and `ion/binaries/Release`
* Copy the following files to the `Debug` and `Release` directories
    * `fmod.dll` from [ion/libraries/Fmod/bin](libraries/Fmod/bin)
    * `FreeImage.dll` from [ion/libraries/FreeImage/bin](libraries/FreeImage/bin)
    * `freetype.dll` from [ion/libraries/FreeType/bin](libraries/FreeType/bin)
    * `glew32.dll` from [ion/libraries/GLEW/bin](libraries/GLEW/bin)
* Copy `glew.h` and `wglew.h` from [ion/libraries/GLEW](libraries/GLEW) to `C:/Program Files (x86)/Windows Kits/10/Include/<latest>/um/gl`
* Copy `glew32.lib` from [ion/libraries/GLEW/lib](libraries/GLEW/lib) to `C:/Program Files (x86)/Windows Kits/10/Lib/<latest>/um/x86`
* Open the project file `ion.sln` and rebuild solution

## How to run the demo
* Do all of the steps in ***How to get started***
* Copy the [ion/demo/data](demo/data) directory to the `Debug` and `Release` directories
* Copy all files in [ion/source/engine/graphics/shaders/glsl](source/engine/graphics/shaders/glsl) to the `Debug\data\shaders` and `Release\data\shaders` directories
* Copy `main.cpp` from [ion/demo](demo) to [ion/source](source) and overwrite the existing file

### Demo screenshots
<p float="left">
    <img src="demo/screenshots/splash_screen.jpg?raw=true" width="384px" />
    <img src="demo/screenshots/demo_screen.jpg?raw=true" width="384px" />
    <img src="demo/screenshots/demo_screen_bounds.jpg?raw=true" width="384px" />
    <img src="demo/screenshots/gui_screen.jpg?raw=true" width="384px" />
</p>

## How to setup shaders
ION engine comes with some [default shaders](source/engine/graphics/shaders/glsl).
* `IonModelShader` is needed for rendering models (meshes, shapes, sprites etc.) as designed
* `IonParticleShader` is needed for rendering particle systems as designed
* `IonTextShader` is needed for rendering text as designed
* `IonSimple*` are mostly for rendering GUI/HUD objects without any lighting or fog

The *fixed function pipeline* is used when rendering without shaders (***not recommended***). To render text properly without shaders, use `FontManager::FontGlyphTextureType(GlyphTextureType::Texture2D)`.
* Copy the shaders you need from [ion/source/engine/graphics/shaders/glsl](source/engine/graphics/shaders/glsl) to your project directory
* Check out `main.cpp` from [ion/demo](demo) to see how to setup and use shaders correctly in your own project

## Author
The ION game engine is written by Jan Ivar Goli.

## License
The ION game engine is licensed under the [MIT license](https://opensource.org/licenses/MIT/).