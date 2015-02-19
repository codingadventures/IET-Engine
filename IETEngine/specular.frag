#version 330
  
vec3 calculate_ambient_component_material();
vec3 calculate_diffuse_component_material(vec3 normal, vec3 light_direction);
float calculate_specular_component(vec3 normalized_normal, vec3 eye_direction, vec3 reflection_direction); 
vec3 calculate_specular_component_material(float specular);

vec3 calculate_ambient_component_material_texture(vec2 tex_coord);
vec3 calculate_diffuse_component_material_texture(vec3 normal, vec3 light_direction,vec2 tex_coord);
vec3 calculate_specular_component_material_texture(float specular,vec2 tex_coord);
 

mat3 calculate_bumped_matrix(vec3 normal,vec3 tangent);

vec3 calculate_bumped_normal(mat3 TBN, vec2 tex_coord);

in vec3 normalized_normal;  
in vec2 tex_coord;
in vec3 light_direction;
in vec3 eye_direction;
  
in vec3 tangent_dir;

uniform bool hasTexture;
 
uniform bool    use_bump_mapping;
out vec4 color;

void main()
{
	vec3 light_dir = light_direction;
    vec3 normal = normalized_normal;
    vec3 reflection_direction;
    vec3 eye_dir = eye_direction;
	if (use_bump_mapping)
    {    
        mat3 tbn = calculate_bumped_matrix(normalized_normal,tangent_dir);  
                                                         
        normal = normalize(calculate_bumped_normal(tbn,tex_coord));
		reflection_direction 	= 	reflect(-light_direction, normal);
		//reflection_direction = tbn * reflection_direction;
		//light_dir = tbn * light_direction;
		//eye_dir = tbn * eye_direction;

    }
     else
		reflection_direction 	= 	reflect(-light_direction, normalized_normal);

	//Properties
	vec3 Ia,Id,Is;
 	float specular = calculate_specular_component(normal,eye_dir,  reflection_direction);
 	
 	if (!hasTexture)
 	{
	   Ia = calculate_ambient_component_material();
	   Id = calculate_diffuse_component_material(normal, light_dir);
	   Is = calculate_specular_component_material(specular);
	}
	else
	{
		Ia =  calculate_ambient_component_material_texture(tex_coord);
		Id =  calculate_diffuse_component_material_texture(normal,light_dir,tex_coord);
		Is =  calculate_specular_component_material_texture(specular,tex_coord);
	}
	 // Combine results
   		 		
	color = vec4(Ia + Id + Is, 1.0f);
}