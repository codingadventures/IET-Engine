#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoord; 
layout (location = 3) in vec4 color; 
layout (location = 4) in vec3 tangent; 

 
vec4 mvpTransform(vec4 position);
vec4 mvTransform(vec4 position);
vec3 normal_transform(mat4 model_transpose_inverse, vec3 normal);
vec3 calculate_light_direction(vec3 vertex_world_space);
 

uniform mat4 model_transpose_inverse;
uniform mat4 model_matrix;
uniform vec3 eye_position; 


out vec3 normalized_normal;  

out vec3 light_direction;
out vec3 eye_direction;

out vec3 tex_coord_skybox;  
out vec3 vertex_world_space;
out vec3 vertex_view_space;   
out vec3 vHatchWeights0;
out vec3 vHatchWeights1;
out vec2 tex_coord_geom; 

void main()
{  
 
    tex_coord_geom    =  texCoord;

   vec4 position_vec4 =  vec4(position, 1.0f);
   vertex_world_space   
                      =  vec3(model_matrix * position_vec4); 

   vertex_view_space  =  vec3(mvTransform(position_vec4));
 
   vec3 normal        =  normal_transform(model_transpose_inverse, normals); 
   eye_direction      =  normalize(eye_position - vertex_world_space);
 
   normalized_normal  =  normalize(normal);

    light_direction   =  calculate_light_direction(vertex_world_space);
 
   gl_Position        =  mvpTransform(position_vec4);
     
   
   float  diffuse     = min(1.0,max(0.0,dot(light_direction.xyz,normal)));
   diffuse = diffuse * diffuse;
   diffuse = diffuse * diffuse;

   float  hatchFactor = diffuse * 6.0;
   vec3 weight0 = vec3(0.0);
   vec3 weight1 = vec3(0.0);

   if (hatchFactor>5.0)
   {
      weight0.x = 1.0;
   } // End if

   else if (hatchFactor>4.0)
   {
      weight0.x = 1.0 - (5.0 - hatchFactor);
      weight0.y = 1.0 - weight0.x;
   } // End else if

   else if (hatchFactor>3.0)
   {
      weight0.y = 1.0 - (4.0 - hatchFactor);
      weight0.z = 1.0 - weight0.y;
   } // End else if

   else if (hatchFactor>2.0)
   {
      weight0.z = 1.0 - (3.0 - hatchFactor);
      weight1.x = 1.0 - weight0.z;
   } // End else if

   else if (hatchFactor>1.0)
   {
      weight1.x = 1.0 - (2.0 - hatchFactor);
      weight1.y = 1.0 - weight1.x;
   } // End else if

   else if (hatchFactor>0.0)
   {
      weight1.y = 1.0 - (1.0 - hatchFactor);
      weight1.z = 1.0 - weight1.y;
   } // End else if

   vHatchWeights0 = weight0;
   vHatchWeights1 = weight1;
}