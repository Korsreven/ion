# Ion Engine
***
A fast, lightweight and fully functional 2D game engine, with an easy to use API for developers. One of the main targets is to be able to easily develop indy games for the Steam platform.

## Source Code
The game engine is written entirely in C++17/20 on top of OpenGL. While the development is mainly done using Visual Studio on a Microsoft Windows PC, the source code itself should be portable and platform independent.

## Project History
The project started in November 2017, where my main motivational factor was to modernize a former game engine called Lunar Engine (not published). It was in development over a period of 10 years (2007-2017), with a codebase of over 60 KLOC. Instead of just porting the source code from C++98 to C++17/20, I decided it would be fun to write a new engine from scratch applying modern best practices.

## Third party libraries
One of the things I enjoy the most when writing code, is to actually write most of the code myself. But there is no rule without exceptions... Ion Engine is using four different libraries.
* FMOD v2.1.10 (https://www.fmod.com/)
* FreeImage v3.18.0 (https://freeimage.sourceforge.io/)
* FreeType v2.10.3 (https://www.freetype.org/)
* GLEW v2.1.0 (http://glew.sourceforge.net/)