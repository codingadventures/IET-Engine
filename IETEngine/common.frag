

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

uniform sampler3D hatch3d;

uniform float rimWeight;  
uniform int invertRim;  

uniform Light light;
uniform Material material;
uniform Hatching hatching;



uniform float   roughnessValue; // 0 : smooth, 1: rough
uniform float   fresnelReflectance;// fresnel reflectance at normal incidence

float calculate_specular_cook_torrance_component(vec3 eye_direction,vec3 light_direction, vec3 normalized_normal, vec3 H)
{ 
    float NdotL                 =   dot(normalized_normal, light_direction);
    
    float NdotH                 =   dot(normalized_normal, H); 
    float NdotV                 =   dot(normalized_normal, eye_direction); // note: this could also be NdotL, which is the same value
    float VdotH                 =   dot(eye_direction, H);
    float mSquared              =   roughnessValue * roughnessValue;
 

    float Gc                    =   2 * ( NdotH * NdotV ) / VdotH;
    float Gb                    =   2 * ( NdotH * NdotL ) / VdotH;
    float geo_attenuation       =   min(1.0,min(Gb,Gc));

    // roughness (or: microfacet distribution function)
    // beckmann distribution function
    float r1                    =   1.0 / ( 4.0 * mSquared * pow(NdotH, 4.0));
    float r2                    =   (NdotH * NdotH - 1.0) / (mSquared * NdotH * NdotH);
    float roughness             =   r1 * exp(r2);

    // fresnel
    // Schlick approximation
    float fresnel               =   fresnelReflectance + (1.0 - fresnelReflectance) * pow(1.0 - VdotH, 5.0);
  
    float specular              =   max(0.0,(fresnel * geo_attenuation * roughness) / (NdotV * NdotL * 4.0));

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

vec4 get_texture_diffuse(vec2 tex_coord)
{
    return  texture(material.texture_diffuse1,tex_coord);
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
    float specular  = pow(max(dot(eye_direction, reflection_direction), 0.0), 32);
    return  specular;
}

vec3 calculate_specular_component_material(float specular)
{
        return  light.specular * specular * material.specular;
}

vec3  calculate_specular_component_material_texture(float specular,vec2 tex_coord)
{
        return  light.specular * specular * vec3(texture(material.texture_specular1,tex_coord));
} 

vec3 get_light_ambient()
{
    return light.ambient;
}

mat3 calculate_bumped_matrix(vec3 normal,vec3 tangent)                                                                 
{                                                                                                                                              
    tangent = normalize(tangent - dot(tangent, normal) * normal);                           
    vec3 Bitangent = cross(tangent, normal);                                                                                                                    
    mat3 TBN = mat3(tangent, Bitangent, normal);                                            
    return TBN;                                                    
}     

vec3 calculate_bumped_normal(mat3 TBN,vec2 tex_coord)
{

    vec3 BumpMapNormal = texture(material.texture_normal1, tex_coord).xyz;                                
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);                              
    return   BumpMapNormal ;    
}


// vec4 calculate_hatch_color(vec2 tex_coord, float shading)
// {
//     vec4 c;
//      float step = 1. / 6.;
//     if( shading <= step ){   
//         c = mix( texture2D( hatching.hatch6, tex_coord ), texture2D( hatching.hatch5, tex_coord ), 6. * shading );
//     }
//     if( shading > step && shading <= 2. * step ){
//         c = mix( texture2D( hatching.hatch5, tex_coord ), texture2D( hatching.hatch4, tex_coord) , 6. * ( shading - step ) );
//     }
//     if( shading > 2. * step && shading <= 3. * step ){
//         c = mix( texture2D( hatching.hatch4, tex_coord ), texture2D( hatching.hatch3, tex_coord ), 6. * ( shading - 2. * step ) );
//     }
//     if( shading > 3. * step && shading <= 4. * step ){
//         c = mix( texture2D( hatching.hatch3, tex_coord ), texture2D( hatching.hatch2, tex_coord ), 6. * ( shading - 3. * step ) );
//     }
//     if( shading > 4. * step && shading <= 5. * step ){
//         c = mix( texture2D( hatching.hatch2, tex_coord ), texture2D( hatching.hatch1, tex_coord ), 6. * ( shading - 4. * step ) );
//     }
//     if( shading > 5. * step ){
//         c = mix( texture2D( hatching.hatch1, tex_coord ), vec4( 1. ), 6. * ( shading - 5. * step ) );
//     }

//     return c;
// }
  
// vec4 calculate_hatching( 
//     vec3 normalized_normal, 
//     vec3 vertex_world_space,
//     vec2 tex_coord,
//     vec4 ink_color,
//     vec3 light_dir
//     ) 
// {
//     vec4 c;
//     vec3 Ia,Id,Is;
//     float specular = 0.;
//     float ambient = 1.;
//     float nDotVP = max( 0., dot( normalized_normal, normalize( vec3( light.position ) ) ) );
//     vec3 n =   normalized_normal ;
//     float diffuse = nDotVP;

//     vec3 r = -reflect(light.position, n);
//     r = normalize(r);
//     vec3 v = -vertex_world_space.xyz;
//     v = normalize(v);
//     float nDotHV = max( 0., dot( r, v ) );

//     if( nDotVP != 0. ) specular = pow ( nDotHV, material.shininess );
//     float rim = max( 0., abs( dot( n, normalize( -vertex_world_space.xyz ) ) ) );
//     //if( invertRim == 1 ) rim = 1. - rim;

     
//     Ia =  calculate_ambient_component_material();
//     Id =  calculate_diffuse_component_material(normalized_normal, light_dir);
//     Is =  calculate_specular_component_material(specular);
 
//      float shading = clamp(Ia + Id  + 0.65 * rim + Is, 0, 1); 
 
//     c = calculate_hatch_color(tex_coord, shading);   

//     vec4 src = mix( mix( ink_color, vec4( 1. ), c.r ), c, .5 );
    
//     return src;
// }

// vec4 calculate_hatching_adj(vec3 normalized_normal, vec3 light_dir, vec3 eye_direction, vec3 reflection_direction,vec3 tex_coord[4], vec3 vertex_world_space)
// {
//     vec3 finalColor; 
//     ivec3 sizeOfTex = textureSize(hatch3d, 0);
//     vec3 Ia,Id,Is; 
//     float sumF = 0.0;
//     vec3 colort[4];
//     // sample depth of the texture by light intensity
     
//     float specular = calculate_specular_component(normalized_normal,  eye_direction,  reflection_direction);
//     float rim = max( 0., abs( dot( normalized_normal, normalize( -vertex_world_space.xyz ) ) ) );

//     Ia =  calculate_ambient_component_material();
//     Id =  calculate_diffuse_component_material(normalized_normal, light_dir);
//     Is =  calculate_specular_component_material(specular);

//     float shading = clamp(Ia + Id + 0.65 * rim + Is, 0 , 1);

//     for(int i = 0; i< 4; i++)
//     {
//        colort[i] = calculate_hatch_color(tex_coord[i].st * 3, shading).rgb;
//        sumF += tex_coord[i].p;  
//     }

//     for(int i =0; i < 4; i++)
//     {
//         finalColor += colort[i] * tex_coord[i].p;
//     }
//     finalColor = finalColor /  sumF ;

//     return vec4(finalColor,1.0); 
// }

// vec4 calculate_hatching_3d(vec3 normalized_normal, vec3 light_dir, vec3 eye_direction, vec3 reflection_direction,vec2 tex_coord, vec3 vertex_world_space)
// {
//     vec3 finalColor; 
//     ivec3 sizeOfTex = textureSize(hatch3d, 0);
//     vec3 Ia,Id,Is; 
//     float sumF = 0.0;
//     // sample depth of the texture by light intensity
     
//     float specular = calculate_specular_component(normalized_normal,  eye_direction,  reflection_direction);

//     Ia =  calculate_ambient_component_material();
//     Id =  calculate_diffuse_component_material(normalized_normal, light_dir);
//     Is =  calculate_specular_component_material(specular);

//     float rim = max( 0., abs( dot( normalized_normal, normalize( -vertex_world_space.xyz ) ) ) );

//     float shading = clamp(Ia + Id + 0.65 * rim + Is, 0 , 1);

//     finalColor = texture(hatch3d, vec3(tex_coord / sizeOfTex.x, shading)).rgb; 
//     return   vec4(finalColor,1.0); 
// }

// vec4 weighted_hatching(vec2 tex_coord, vec3 vHatchWeights0, vec3 vHatchWeights1)
// {
 
//     vec4 hatchTex0 = texture2D(hatching.hatch1,tex_coord) * vHatchWeights0.x;
//     vec4 hatchTex1 = texture2D(hatching.hatch2,tex_coord) * vHatchWeights0.y;
//     vec4 hatchTex2 = texture2D(hatching.hatch3,tex_coord) * vHatchWeights0.z;
//     vec4 hatchTex3 = texture2D(hatching.hatch4,tex_coord) * vHatchWeights1.x;
//     vec4 hatchTex4 = texture2D(hatching.hatch5,tex_coord) * vHatchWeights1.y;
//     vec4 hatchTex5 = texture2D(hatching.hatch6,tex_coord) * vHatchWeights1.z;
    
    
//     vec4 hatchColor =   hatchTex0 +
//                         hatchTex1 +
//                         hatchTex2 +
//                         hatchTex3 +
//                         hatchTex4 +
//                         hatchTex5;

//     return hatchColor;
// }