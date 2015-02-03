#version 330

layout (location = 0) in vec3 position; 
 
//It applies the Model View Projection transformation matrix 
vec4 transform(vec4 position); 

void main()
{  
	gl_Position = transform(vec4(position, 1.0));
}