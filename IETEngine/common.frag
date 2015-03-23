struct Material {
    vec3 ambient;
    vec3 diffuse;
	sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Hatching{
  //For hatching
  sampler2D hatch1;
  sampler2D hatch2;
  sampler2D hatch3;
  sampler2D hatch4;
  sampler2D hatch5;
  sampler2D hatch6;  
};

uniform float rimWeight;  
uniform int invertRim;  

uniform Light light;
uniform Material material;
uniform Hatching hatching;



uniform float   roughnessValue; // 0 : smooth, 1: rough
uniform float   fresnelReflectance;// fresnel reflectance at normal incidence

float calculate_specular_cook_torrance_component(vec3 eye_direction,vec3 light_direction, vec3 normalized_normal, vec3 H)
{ 
	float NdotL 				= 	dot(normalized_normal, light_direction);
	
	float NdotH 				= 	dot(normalized_normal, H); 
    float NdotV 				= 	dot(normalized_normal, eye_direction); // note: this could also be NdotL, which is the same value
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
    float fresnel 				= 	fresnelReflectance + (1.0 - fresnelReflectance) * pow(1.0 - VdotH, 5.0);
  
    float specular 				=   max(0.0,(fresnel * geo_attenuation * roughness) / (NdotV * NdotL * 4.0));

    return specular;
}

vec3 calculate_diffuse_component(vec3 normal, vec3 light_direction)
{
	vec3 normalized_normal = normalize(normal);
	float diffuse_factor =  max(dot(normalized_normal, light_direction), 0.0);
	return diffuse_factor * light.diffuse;
}

vec3 calculate_diffuse_component_material(vec3 normal, vec3 light_direction)
{
	vec3 normalized_normal = normalize(normal);
	float diffuse_factor =  max(dot(normalized_normal, light_direction), 0.0);
	return  light.diffuse * diffuse_factor * material.diffuse;
}

vec3 calculate_diffuse_component_material_texture(vec3 normal, vec3 light_direction, vec2 tex_coord)
{
	vec3 normalized_normal = normalize(normal);
	float diffuse_factor =  max(dot(normalized_normal, light_direction), 0.0);
	return  light.diffuse * diffuse_factor * vec3(texture(material.texture_diffuse1,tex_coord));
}


vec3 calculate_ambient_component_material()
{

	return light.ambient * material.ambient;
}

vec3 calculate_ambient_component_material_texture(vec2 tex_coord)
{ 

	return  light.ambient *  vec3(texture(material.texture_diffuse1,tex_coord));
}


float calculate_specular_component(vec3 normalized_normal, vec3 eye_direction, vec3 reflection_direction)
{
	//Specular Shading
	float specular 	= pow(max(dot(eye_direction, reflection_direction), 0.0), material.shininess);
	return 	specular;
}

vec3 calculate_specular_component_material(float specular)
{
		return	light.specular * specular * material.specular;
}

vec3  calculate_specular_component_material_texture(float specular,vec2 tex_coord)
{
		return	light.specular * specular * vec3(texture(material.texture_specular1,tex_coord));
} 

vec3 get_light_ambient()
{
	return light.ambient;
}

vec3 calculate_bumped_normal(mat3 TBN,vec2 tex_coord)
{

    vec3 BumpMapNormal = texture(material.texture_normal1, tex_coord).xyz;                                
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);                              
    return   BumpMapNormal ;    
}
  
vec4 calculate_hatching(
    float nDotVP, 
    vec3 normalized_normal, 
    vec3 vertex_view_space,
    vec2 tex_coord,
    vec4 ink_color,
    vec3 light_dir
    ) 
{
    vec4 c;
    vec3 Ia,Id,Is;
    float diffuse = nDotVP;
    float specular = 0.;
    float ambient = 1.;

    vec3 n = normalize( normalized_normal );

    vec3 r = -reflect(light.position, n);
    r = normalize(r);
    vec3 v = -vertex_view_space.xyz;
    v = normalize(v);
    float nDotHV = max( 0., dot( r, v ) );

    if( nDotVP != 0. ) specular = pow ( nDotHV, material.shininess );
    //float rim = max( 0., abs( dot( n, normalize( -vertex_view_space.xyz ) ) ) );
    //if( invertRim == 1 ) rim = 1. - rim;


    Ia =  calculate_ambient_component_material();
    Id =  calculate_diffuse_component_material(normalized_normal,light_dir);
    Is =  calculate_specular_component_material(specular);
 
    float shading = Ia + Id /*+ rimWeight * rim */+ Is;

    float step = 1. / 6.;
    if( shading <= step ){   
        c = mix( texture2D( hatching.hatch6, tex_coord ), texture2D( hatching.hatch5, tex_coord ), 6. * shading );
    }
    if( shading > step && shading <= 2. * step ){
        c = mix( texture2D( hatching.hatch5, tex_coord ), texture2D( hatching.hatch4, tex_coord) , 6. * ( shading - step ) );
    }
    if( shading > 2. * step && shading <= 3. * step ){
        c = mix( texture2D( hatching.hatch4, tex_coord ), texture2D( hatching.hatch3, tex_coord ), 6. * ( shading - 2. * step ) );
    }
    if( shading > 3. * step && shading <= 4. * step ){
        c = mix( texture2D( hatching.hatch3, tex_coord ), texture2D( hatching.hatch2, tex_coord ), 6. * ( shading - 3. * step ) );
    }
    if( shading > 4. * step && shading <= 5. * step ){
        c = mix( texture2D( hatching.hatch2, tex_coord ), texture2D( hatching.hatch1, tex_coord ), 6. * ( shading - 4. * step ) );
    }
    if( shading > 5. * step ){
        c = mix( texture2D( hatching.hatch1, tex_coord ), vec4( 1. ), 6. * ( shading - 5. * step ) );
    }

    vec4 src = mix( mix( ink_color, vec4( 1. ), c.r ), c, .5 );
    
    return src;
}