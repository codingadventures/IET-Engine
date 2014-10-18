#pragma once

#define NUM_BONES_PER_VERTEX 4


struct VertexWeight
{
	GLuint IDs[NUM_BONES_PER_VERTEX];
	float Weights[NUM_BONES_PER_VERTEX];
};
