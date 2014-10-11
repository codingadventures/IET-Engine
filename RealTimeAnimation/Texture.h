#pragma once

#include <assimp/types.h>
#include <GL/glew.h>

struct Texture {
	GLuint id;
	std::string type;
	aiString path;
};