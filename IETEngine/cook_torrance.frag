#version 330
 

vec3 calculate_light_direction(vec3 vertex_world_space);
vec3 calculate_diffuse_component_material(vec3 normal, vec3 light_direction); 
vec3 get_light_ambient_material(); 
vec3 specular_component_material(float specular);

in  vec3 N; 
in  vec3 vertex_world_space;

uniform vec3 eye_position;
 

out vec4 color;

void main()
{
	// set important material values
    float roughnessValue		= 0.3; // 0 : smooth, 1: rough
    float F0 					= 0.8; // fresnel reflectance at normal incidence
    
	//Properties
	vec3 light_direction 		= 	calculate_light_direction(vertex_world_space);
	vec3 eye_direction 			=   normalize(eye_position - vertex_world_space);
	vec3 norm 					= 	normalize(N); 
 	vec3 H						= 	normalize(eye_direction + light_direction);

	float NdotL 				= 	dot(norm, light_direction);
	
	float NdotH 				= 	dot(norm, H); 
    float NdotV 				= 	dot(norm, eye_direction); // note: this could also be NdotL, which is the same value
    float VdotH 				= 	dot(eye_direction, H);
 	float mSquared 				= 	roughnessValue * roughnessValue;
 

 	float Gc 					=	2 * ( NdotH * NdotV ) / VdotH;
	float Gb 					=	2 * ( NdotH * NdotL ) / VdotH;
	float geo_attenuation 		= 	min(1.0,min(Gb,Gc));

	// roughness (or: microfacet distribution function)
    // beckmann distribution function
    float r1 					= 	1.0 / ( 4.0 * mSquared * pow(NdotH, 4.0));
    float r2 					= 	(NdotH * NdotH - 1.0) / (mSquared * NdotH * NdotH);
    float roughness 			= 	r1 * exp(r2);

	// fresnel
    // Schlick approximation
    float fresnel 				= 	F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
  
    float specular 				=   max(0.0,(fresnel * geo_attenuation * roughness) / (NdotV * NdotL * 4.0));

    vec3 ambient_color 			= 	get_light_ambient_material();
    vec3 diffuse_color 			= 	calculate_diffuse_component_material(norm,light_direction);
    vec3 specular_color			=   specular_component_material(specular);

	color 						=   vec4(ambient_color + diffuse_color + specular_color, 1.0f);
}