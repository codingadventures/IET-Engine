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

	Texture(string file_name, TextureType type);
	bool Load(string directory);
	string Get_Uniform_Name(string index);
};

Texture::Texture(string file_name, TextureType type) : m_texture_type(type),m_file_name(file_name)
{
	assert(file_name!= "");
}

std::string Texture::Get_Uniform_Name(string index)
{
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
		image = new Magick::Image(fullPath.c_str());
		image->write(&blob, "RGBA");
	}
	catch (Magick::Error& Error) {
		std::cout << "Error loading texture '" << fullPath << "': " << Error.what() << std::endl;
		return false;
	}

 


	//SOIL_load_OGL_texture(filename.c_str(),SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS|SOIL_FLAG_INVERT_Y|SOIL_FLAG_NTSC_SAFE_RGB|SOIL_FLAG_COMPRESS_TO_DXT);;
	glGenTextures(1, &id);
	//int width,height,channels;
	//unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, nullptr, 0);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA, image->columns(), image->rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());

	// Parameters
	/*glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glGenerateMipmap(GL_TEXTURE_2D);	*/
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 

	glBindTexture(GL_TEXTURE_2D, 0);

	
	delete image; 
	//	SOIL_free_image_data(image); 
	return true;
}



#endif // Texture_h__