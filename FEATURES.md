![logo](docs/logo.png?raw=true)

# ION Engine 1.0

## Main features
Here is an updated list with most of the main features of ION engine.

### Animations
* Frame animations
	* Looping and non-looping
	* Different directions
		* Normal
		* Reverse
		* Alternate
		* Alternate reverse
	* Adjustable playback speeds
	* Frame-by-frame controllable
* Node animations
	* Animating translation, rotation, scaling and fading
	* Different motion techniques
		* Cubic
		* Exponential
		* Linear
		* Logarithmic
		* Sigmoid
		* Sinh
		* Tanh
	* User definable motion techniques
	* Animation groups and timelines
	* Triggerable actions

### Asset management
* Loading assets from individual files (perfect for modding)
* Loading assets from single large data files
* Categorizing assets into repositories
* Saving repositories to single large data files

### Cameras
* Interchangeable frustums
* Orthographic and perspective projections
* Adjustable FOV
* Different aspect ratio formats
	* Pan and scan
	* Letterbox
	* Windowbox

### Collision detection
* Intersection scene queries
* Ray scene queries
* Different built-in primitives
	* Axis-aligned bounding boxes
	* Oriented bounding boxes
	* Bounding spheres
	* Rays

### Colors
* 32-bit colors with alpha channel
* Hex and shorthand hex notations
* X11 colors provided as constants
* Different color spaces
	* RGB
	* CMYK
	* HSL
	* HSV
	* HWB
	* YCbCr

### GUI (fully functional)
* Mouse and keyboard compatable
* Frames and panels
	* Grid layouts
	* Horizontally and vertically aligned grid cells
	* Multiple controls per grid cell
* Mouse cursors and tooltips
* Customizable skins and themes
* Different built-in controls
	* Buttons
	* Check boxes
	* Group boxes
	* Images
	* Labels
	* List boxes
	* Progress bars
	* Radio buttons
	* Scroll bars
	* Sliders
	* Text boxes

### Lighting
* Different light types
	* Directional
	* Point
	* Spot
* Ambient, diffuse and specular colors
* Emissive lighting using point lights
* Ambient scene lighting
* Phong shading

### Materials
* Ambient, diffuse, specular and emissive colors
* Tex-coord cropping and flipping
* Different texture maps
	* Diffuse maps
	* Normal maps
	* Specular maps
	* Emissive maps
* Animatable texture maps

### Models
* Sprites and animated sprites
	* Auto sizing and repeating
	* Keep proportions when resizing
	* Tex-coord cropping and flipping
* Different built-in shapes
	* Borders
	* Curves
	* Ellipses
	* Lines
	* Rectangles
	* Triangles
* User derivable shapes
* Custom meshes

### OpenGL and GLSL
* Hardware adaptation
* OpenGL 4.6 and GLSL 4.60 (460) compatable
* OpenGL 2.0 and GLSL 1.10 (110) backward compatable
* Different integrated GLSL types
	* bool, int, uint, float and double
	* vec2, vec3 and vec4
	* mat2, mat3 and mat4
	* sampler1D and sampler2D
	* sampler1DArray and sampler2DArray

### Particle systems
* Box, point and ring emitters
* Different built-in affectors
	* Color faders
	* Direction randomizers
	* Gravitations
	* Linear forces
	* Scalers
	* Sine forces
	* Velocity randomizers
* User derivable affectors

### Render window (Win32)
* Movable and resizable
* Windowed and fullscreen (Alt+Enter)
* Different fullscreen resolutions
* Mouse, key and window events
* Multiple viewports (alignable and anchorable)
* Replaceable system window base class (OS portable)

### Rendering
* Multi-passes
* Pass blending
* Unit scaling
	* Pixels per unit (PPU)
	* Units per meter/foot
* Different refresh rates
	* Unlimited (no VSync)
	* VSync
	* Adaptive VSync
	* Adaptive VSync half-rate
	* Frame limited
* Programmable pipeline
* Fixed function pipeline compatable

### Resource management
* Preparing resources (blocking/non-blocking)
* Loading, unloading, reloading and repairing resources
* Loading progress reporting

### Scene
* Multiple scene graphs
* Animatable and transformable scene nodes
* Multiple objects per scene node

### Scripting (fully integrated)
* CSS like syntax
* Classes and selectors
* SCSS like variables
* Import script files/resources
* Tree serialization/deserialization
* Class, properties and rules validator
* Error reporting

### Shading
* Vertex and fragment shaders
* Fully customizable shader layouts
* 6 default shaders included
* 6 integrated attributes (plug and play)
* 50 integrated uniforms (plug and play)
* Adjustable gamma
* Different fog modes
	* Exponential
	* Exponential squared
	* Linear

### Sounds
* Non-positional sounds
* Positional (3D) sounds
* Sampling and streaming
* Sound channels and sound channel groups

### Text
* HTML and inline CSS formatting
* Regular, bold, italic and bold-italic fonts
* Horizontal and vertical alignment
* Area size, padding and line height
* Word wrapping and truncating
* ASCII and extended ASCII character sets

### Textures
* Clampable and repeatable
* Mipmapping (LOD)
* 24-bit and 32-bit textures
	* NPOT support
	* POT canvas resizing
	* POT image resampling
	* Different resample filters
		* Box
		* Bicubic
		* Bilinear
		* B-spline
		* Catmull-rom
		* Lanczos3
* Texture atlases
	* Row-major
	* Column-major

### Utilities
* Conversions
* Cryptography
* Encoding/decoding
* File system
* Math functions
* Matrix primitives
* Random numbers
* String functions
* System functions
* Vector primitives