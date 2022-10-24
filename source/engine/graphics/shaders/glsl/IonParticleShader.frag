/*
-------------------------------------------
This source file is part of Ion Engine
	- A fast and lightweight 2D game engine
	- Written in C++ using OpenGL

Author:	Jan Ivar Goli
Area:	graphics/shaders/glsl
File:	IonParticleShader.frag
-------------------------------------------
*/

#version 330 core

struct Scene
{
	vec4 ambient;
	float gamma;
	bool has_fog;
	sampler1DArray lights;
	sampler1DArray emissive_lights;
	int light_count;
	int emissive_light_count;
};

struct Camera
{
	vec3 position;
	float rotation;
};

struct Primitive
{
	bool has_material;
};

struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 emissive;
	float shininess;

	sampler2D diffuse_map;
	sampler2D normal_map;
	sampler2D specular_map;
	sampler2D emissive_map;

	bool has_diffuse_map;
	bool has_normal_map;
	bool has_specular_map;
	bool has_emissive_map;
	bool lighting_enabled;
};

struct Fog
{
	int mode;
	float density;
	float near;
	float far;
	vec4 color;
};

struct Light
{
	int type;
	vec3 position;
	vec3 direction;
	float radius;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float constant;
	float linear;
	float quadratic;

	float cutoff;
	float outer_cutoff;
};

struct EmissiveLight
{
	vec3 position;
	float radius;
	vec4 color;
};


in vec3 vert_position;
in vec4 vert_color;
in mat3 normal_matrix;
in mat2 rotation_matrix;

out vec4 frag_color;

uniform Scene scene;
uniform Camera camera;
uniform Primitive primitive;
uniform Material material;
uniform Fog fog;


const vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
const vec4 dark_gray = vec4(0.66, 0.66, 0.66, 1.0);
const float log2e = 1.442695;

vec2 tex_coord = (rotation_matrix * (gl_PointCoord - 0.5)).xy + 0.5;
float fog_frag_coord = abs(vert_position.z);
float fog_scale = 1.0 / (fog.far - fog.near);
bool render_lights = scene.light_count + scene.emissive_light_count > 0 &&
	(!primitive.has_material || material.lighting_enabled);


float linear_fog()
{
	return (fog.far - fog_frag_coord) * fog_scale;
}

float exp_fog()
{
	return exp2(-fog.density * fog_frag_coord * log2e);
}

float exp2_fog()
{
	return exp2(-fog.density * fog.density * fog_frag_coord * fog_frag_coord * log2e);
}


Light fetch_light(int i)
{
	vec4 texel1 = texelFetch(scene.lights, ivec2(0, i), 0);
	vec4 texel2 = texelFetch(scene.lights, ivec2(1, i), 0);
	vec4 texel3 = texelFetch(scene.lights, ivec2(2, i), 0);
	vec4 texel4 = texelFetch(scene.lights, ivec2(3, i), 0);
	vec4 texel5 = texelFetch(scene.lights, ivec2(4, i), 0);
	vec4 texel6 = texelFetch(scene.lights, ivec2(5, i), 0);
	vec4 texel7 = texelFetch(scene.lights, ivec2(6, i), 0);

	//Create light from texels
	Light light;
	light.type = int(floor(texel1.x + 0.5));
	light.position = texel1.yzw;
	light.direction = texel2.xyz;
	light.radius = texel2.w;

	light.ambient = texel3.rgba;
	light.diffuse = texel4.rgba;
	light.specular = texel5.rgba;

	light.constant = texel6.x;
	light.linear = texel6.y;
	light.quadratic = texel6.z;

	light.cutoff = texel7.x;
	light.outer_cutoff = texel7.y;
	return light;
}

EmissiveLight fetch_emissive_light(int i)
{
	vec4 texel1 = texelFetch(scene.emissive_lights, ivec2(0, i), 0);
	vec4 texel2 = texelFetch(scene.emissive_lights, ivec2(1, i), 0);

	//Create emissive light from texels
	EmissiveLight emissive_light;
	emissive_light.position = texel1.xyz;
	emissive_light.radius = texel1.w;
	emissive_light.color = texel2.rgba;
	return emissive_light;
}


vec3 calc_point_light(Light light, vec3 normal, vec3 view_dir, vec4 ambient_color, vec4 diffuse_color, vec4 specular_color, float shininess)
{
	vec3 light_dir = normalize(light.position - vert_position);

	//Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    //Specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);

    //Attenuation
	light.position.z = light.radius > 0.0 ?
		vert_position.z : //Discard z
		light.position.z; //Keep z
	
	float dist = length(light.position - vert_position);
	float attenuation = light.radius > 0.0 ?
		max(1.0 - (dist / light.radius), 0.0) :
		1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	//Combine ambient, diffuse and specular color
	ambient_color *= light.ambient;
	diffuse_color *= light.diffuse;
	specular_color *= light.specular;

	ambient_color.rgb *= ambient_color.a * attenuation;
	diffuse_color.rgb *= diffuse_color.a * diff * attenuation;
	specular_color.rgb *= specular_color.a * spec * attenuation;

	return ambient_color.rgb + diffuse_color.rgb + specular_color.rgb;
}

vec3 calc_directional_light(Light light, vec3 normal, vec3 view_dir, vec4 ambient_color, vec4 diffuse_color, vec4 specular_color, float shininess)
{
	vec3 light_dir = normalize(-light.direction);

	//Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    //Specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
	
	//Combine ambient, diffuse and specular color
	ambient_color *= light.ambient;
	diffuse_color *= light.diffuse;
	specular_color *= light.specular;

	ambient_color.rgb *= ambient_color.a;
	diffuse_color.rgb *= diffuse_color.a * diff;
	specular_color.rgb *= specular_color.a * spec;

	return ambient_color.rgb + diffuse_color.rgb + specular_color.rgb;
}

vec3 calc_spot_light(Light light, vec3 normal, vec3 view_dir, vec4 ambient_color, vec4 diffuse_color, vec4 specular_color, float shininess)
{
	vec3 light_dir = normalize(light.position - vert_position);

	//Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    //Specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);

    //Attenuation
    float dist = length(light.position - vert_position);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
    //Intensity
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);

    //Combine ambient, diffuse and specular color
	ambient_color *= light.ambient;
	diffuse_color *= light.diffuse;
	specular_color *= light.specular;

	ambient_color.rgb *= ambient_color.a * attenuation * intensity;
	diffuse_color.rgb *= diffuse_color.a * diff * attenuation * intensity;
	specular_color.rgb *= specular_color.a * spec * attenuation * intensity;

	return ambient_color.rgb + diffuse_color.rgb + specular_color.rgb;
}

vec3 calc_emissive_light(EmissiveLight emissive_light, vec3 normal, vec3 view_dir, vec4 ambient_color, vec4 diffuse_color, vec4 specular_color, float shininess)
{
	vec3 light_dir = normalize(emissive_light.position - vert_position);

	//Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0);
    //Specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);

    //Attenuation
	emissive_light.position.z = vert_position.z; //Discard z
    float dist = length(emissive_light.position - vert_position);
    float attenuation = max(1.0 - (dist / emissive_light.radius), 0.0);

	//Combine ambient, diffuse and specular color
	ambient_color *= emissive_light.color;
	diffuse_color *= emissive_light.color;
	specular_color *= dark_gray;

	ambient_color.rgb *= ambient_color.a * attenuation;
	diffuse_color.rgb *= diffuse_color.a * diff * attenuation;
	specular_color.rgb *= specular_color.a * spec * attenuation;

	return ambient_color.rgb + diffuse_color.rgb + specular_color.rgb;
}


void main()
{
	//Ambient, diffuse, specular and emissive color
	vec4 ambient_color = scene.ambient * vert_color;
	vec4 diffuse_color = vert_color;
	vec4 specular_color = dark_gray;
	vec4 emissive_color = black;
	float shininess = 32.0;

	//Normal/bump mapping
	vec3 normal = vec3(0.0, 0.0, 1.0);


	if (primitive.has_material)
	{
		ambient_color *= material.ambient;
		diffuse_color *= material.diffuse;
		specular_color = material.specular;
		emissive_color = material.emissive;
		shininess = material.shininess;

		if (material.has_diffuse_map)
		{
			vec4 diffuse_map_color = texture(material.diffuse_map, tex_coord);
			ambient_color *= diffuse_map_color;
			diffuse_color *= diffuse_map_color;
			emissive_color *= diffuse_map_color;
		}

		if (material.has_specular_map)
		{
			vec4 specular_map_color = texture(material.specular_map, tex_coord);
			specular_color *= specular_map_color;
		}

		if (material.has_emissive_map)
		{
			vec4 emissive_map_color = texture(material.emissive_map, tex_coord);
			emissive_color *= emissive_map_color;
		}

		if (material.has_normal_map)
		{
			normal = texture(material.normal_map, tex_coord).rgb;
			normal = normal_matrix * normalize(normal * 2.0 - 1.0);
		}
	}


	vec4 color = ambient_color +
		vec4(emissive_color.rgb * emissive_color.a, 0.0);

	//Fog effect
	if (scene.has_fog)
	{
		vec4 fog_color = scene.ambient * fog.color;
		fog_color.rgb *= fog.color.a;
		fog_color.a = color.a; //Set equal before mixing

		//Exponential fog
		if (fog.mode == 0)
			color = mix(fog_color, color, clamp(exp_fog(), 0.0, 1.0));
		//Exponential2 (squared) fog
		else if (fog.mode == 1)
			color = mix(fog_color, color, clamp(exp2_fog(), 0.0, 1.0));
		//Linear fog
		else if (fog.mode == 2)
			color = mix(fog_color, color, clamp(linear_fog(), 0.0, 1.0));
	}

	
	//Lighting
	if (render_lights)
	{
		vec3 light_color = black.rgb;
		vec3 view_dir = normalize(camera.position - vert_position);

		//Accumulate each light
		for (int i = 0; i < scene.light_count; ++i)
		{
			Light light = fetch_light(i);

			//Point light
			if (light.type == 0)
				light_color += calc_point_light(light, normal, view_dir, ambient_color, diffuse_color, specular_color, shininess);
			
			//Directional light
			else if (light.type == 1)
				light_color += calc_directional_light(light, normal, view_dir, ambient_color, diffuse_color, specular_color, shininess);
			
			//Spot light
			else if (light.type == 2)
				light_color += calc_spot_light(light, normal, view_dir, ambient_color, diffuse_color, specular_color, shininess);
		}

		//Accumulate each emissive light
		for (int i = 0; i < scene.emissive_light_count; ++i)
		{
			EmissiveLight emissive_light = fetch_emissive_light(i);
			light_color += calc_emissive_light(emissive_light, normal, view_dir, ambient_color, diffuse_color, specular_color, shininess);
		}

		color.rgb += light_color;
	}


	//Gamma correction
    color = pow(color, vec4(1.0 / scene.gamma));

	//Fragment color
	frag_color = color;
}