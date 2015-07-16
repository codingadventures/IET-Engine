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

uniform Light light;
uniform Material material;

vec3 calculate_diffuse_component_material_texture(vec3 normal, vec3 light_direction, vec2 tex_coord)
{
    vec3 normalized_normal = normalize(normal);
    float diffuse_factor =  max(dot(normalized_normal, light_direction), 0.0);
    return  light.diffuse * diffuse_factor * vec3(texture(material.texture_diffuse1,tex_coord));
}


vec3 get_light_ambient()
{
    return light.ambient;
}


vec3 calculate_ambient_component_material_texture(vec2 tex_coord)
{ 

    return  light.ambient *  vec3(texture(material.texture_diffuse1,tex_coord));
}