#pragma once

#include <glm/glm.hpp>

#define NUM_BONES_PER_VERTEX 4


struct VertexBoneData
{
	GLuint IDs[NUM_BONES_PER_VERTEX];
	float Weights[NUM_BONES_PER_VERTEX];
};


struct Vertex {
	// Position
	glm::vec3 Position;
	// Normal
	glm::vec3 Normal;
	// TexCoords
	glm::vec2 TexCoords;
	//BoneData
	VertexBoneData BoneData;
};

