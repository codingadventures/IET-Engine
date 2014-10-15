#version 330 core 

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

const int MAX_BONES = 100;


const vec3 colorMap[16] = vec3[16](
	vec3(1.0f,0.0f,0.0f),
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,0.0f,1.0f),
	vec3(1.0f,0.0f,1.0f),
	vec3(1.0f,1.0f,0.0f),
	vec3(1.0f,0.0f,0.0f),
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,0.0f,1.0f),
	vec3(1.0f,0.0f,1.0f),
	vec3(1.0f,1.0f,0.0f),
	vec3(1.0f,0.0f,0.0f),
	vec3(0.0f,1.0f,0.0f),
	vec3(0.0f,0.0f,1.0f),
	vec3(1.0f,0.0f,1.0f),
	vec3(1.0f,1.0f,0.0f),
	vec3(0.5f,0.2f,1.0f)
);

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 bones[MAX_BONES];

out vec2 TexCoords;
out vec3 colour;

void main() 
{ 

//	mat4 BoneTransform = bones[int(BoneIDs[0])] * Weights[0];
  //  BoneTransform     += bones[BoneIDs[1]] * Weights[1];
    //BoneTransform     += bones[BoneIDs[2]] * Weights[2];
    //BoneTransform     += bones[BoneIDs[3]] * Weights[3];
	 
	//vec4 PosL    = BoneTransform * vec4(position, 1.0);
	colour = colorMap[BoneIDs[0]];
	//mat4 v = bones[0];
	gl_Position =  projection * view * model * vec4(position,1.0f) ;//* PosL; 
	 
	TexCoords = texCoord;
}