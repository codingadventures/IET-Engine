#pragma once

#include <glm/glm.hpp>
#include "VertexWeight.h"



struct Vertex {
	// Position
	glm::vec3 Position;
	// Normal
	glm::vec3 Normal;
	// TexCoords
	glm::vec2 TexCoords;
	//BoneData
	VertexWeight WeightData;
	//Color
	glm::vec3 Color;
};

