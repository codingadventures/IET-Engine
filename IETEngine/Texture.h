#ifndef Texture_h__
#define Texture_h__


#include <Magick++.h>
 
#include <GL/glew.h>

enum TextureType{ 
	TextureType_DIFFUSE,
	TextureType_SPECULAR,
	TextureType_NORMAL,
	TextureType_REFLECTION
};

struct Texture {
	GLuint id;
	TextureType m_texture_type;
	string m_file_name;
	string m_directory;
	string m_uniform_name;
	Texture(string file_name, TextureType type, string uniform_name = "");
	Texture(string directory, string uniform_name = "");
	bool Load(string directory);
	bool Load3D(vector<string> textures);
	string Get_Uniform_Name(string index);
};

Texture::Texture(string file_name, TextureType type, string uniform_name ) : m_texture_type(type),m_file_name(file_name), m_uniform_name(uniform_name)
{
	assert(file_name!= "");
}

Texture::Texture(string directory, string uniform_name /*= ""*/) 
	: m_directory(directory), m_uniform_name(uniform_name)
{
	
}

std::string Texture::Get_Uniform_Name(string index)
{
	if (!m_uniform_name.empty()) return m_uniform_name;

	switch (m_texture_type)
	{
	case TextureType_REFLECTION:
		return "material.texture_reflection" + index;
		break;

	case TextureType_DIFFUSE:
		return "material.texture_diffuse" + index;
		break;
	case TextureType_SPECULAR:
		return "material.texture_specular" + index;
		break;
	case TextureType_NORMAL:
		return "material.texture_normal" + index;
		break;
	 
	default:
		break;
	}
}

bool Texture::Load(string directory)
{
	Magick::Blob blob;
	Magick::Image* image; 
	string stringFileName(m_file_name);
	string fullPath = directory + "\\" + stringFileName;
	try {
		image = new Magick::Image(fullPath);
		image->write(&blob, "RGBA");
	}
	catch (Magick::Error& Error) {
		std::cout << "Error loading texture '" << fullPath << "': " << Error.what() << std::endl;
		return false;
	}

 
	int col = image->columns();

	//SOIL_load_OGL_texture(filename.c_str(),SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS|SOIL_FLAG_INVERT_Y|SOIL_FLAG_NTSC_SAFE_RGB|SOIL_FLAG_COMPRESS_TO_DXT);;
	glGenTextures(1, &id);
	//int width,height,channels;
	//unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, nullptr, 0);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA, image->columns(), image->rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());

	// Parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	//glGenerateMipmap(GL_TEXTURE_2D);	/**/
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);/* */

	glBindTexture(GL_TEXTURE_2D, 0);

	
//	delete image; 
	//	SOIL_free_image_data(image); 
	return true;
}



bool Texture::Load3D(  vector<string> textures)
{
	GLuint texture;
	Magick::Blob blob;
	Magick::Image* image; 

	if(textures.size() == 0)
	{
		return false;
	}

	GLsizei width, height, depth = (GLsizei)textures.size();

	string stringFileName(m_file_name);
	string fullPath = m_directory + "\\" + textures[0];
	try {
		image = new Magick::Image(fullPath.c_str());
		image->write(&blob, "RGBA");

		width = image->columns();
		height = image->rows();
	}
	catch (Magick::Error& Error) {
		std::cout << "Error loading texture '" << fullPath << "': " << Error.what() << std::endl;
		return false;
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_3D, id);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, 
		height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glTexSubImage3D(
		GL_TEXTURE_3D, 0, 
		0, 0, (GLint)1,
		width, height, 1,
		GL_RGBA, GL_UNSIGNED_BYTE,
		blob.data()
		);

//	delete image;

	for (int i = 1; i < textures.size(); i++)
	{
		fullPath = m_directory + "\\" + textures[i];
		image = new Magick::Image(fullPath.c_str());
		image->write(&blob, "RGBA");
		
		glTexSubImage3D(
			GL_TEXTURE_3D, 0, 
			0, 0, (GLint)i,
			width, height, 1,
			GL_RGBA, GL_UNSIGNED_BYTE,
			blob.data()
			);

	//	delete image;
	} 

	glGenerateMipmap(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, 0);
}

#endif // Texture_h__