#ifndef Texture_h__
#define Texture_h__


//#include <Magick++.h>

#include <GL/glew.h>
//#include "png.h"
#include "SOIL.h"

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
	bool  m_has3dTexture;
	Texture(string file_name, TextureType type, string uniform_name = "");
	Texture(string directory, string uniform_name = "");
	bool Load(string directory);
	bool Load3D(vector<string> textures);
	string Get_Uniform_Name(string index);
};

Texture::Texture(string file_name, TextureType type, string uniform_name ) 
	: m_texture_type(type),m_file_name(file_name), m_uniform_name(uniform_name), m_has3dTexture(false)
{
	assert(file_name!= "");
}

Texture::Texture(string directory, string uniform_name /*= ""*/) 
	: m_directory(directory), m_uniform_name(uniform_name), m_has3dTexture(false)
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

//GLuint png_texture_load(const char * file_name, int * width, int * height)
//{
//	png_byte header[8];
//
//	FILE *fp = fopen(file_name, "rb");
//	if (fp == 0)
//	{
//		perror(file_name);
//		return 0;
//	}
//
//	// read the header
//	fread(header, 1, 8, fp);
//
//	if (png_sig_cmp(header, 0, 8))
//	{
//		fprintf(stderr, "error: %s is not a PNG.\n", file_name);
//		fclose(fp);
//		return 0;
//	}
//
//	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//	if (!png_ptr)
//	{
//		fprintf(stderr, "error: png_create_read_struct returned 0.\n");
//		fclose(fp);
//		return 0;
//	}
//
//	// create png info struct
//	png_infop info_ptr = png_create_info_struct(png_ptr);
//	if (!info_ptr)
//	{
//		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
//		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
//		fclose(fp);
//		return 0;
//	}
//
//	// create png info struct
//	png_infop end_info = png_create_info_struct(png_ptr);
//	if (!end_info)
//	{
//		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
//		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
//		fclose(fp);
//		return 0;
//	}
//
//	// the code in this if statement gets called if libpng encounters an error
//	if (setjmp(png_jmpbuf(png_ptr))) {
//		fprintf(stderr, "error from libpng\n");
//		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//		fclose(fp);
//		return 0;
//	}
//
//	// init png reading
//	png_init_io(png_ptr, fp);
//
//	// let libpng know you already read the first 8 bytes
//	png_set_sig_bytes(png_ptr, 8);
//
//	// read all the info up to the image data
//	png_read_info(png_ptr, info_ptr);
//
//	// variables to pass to get info
//	int bit_depth, color_type;
//	png_uint_32 temp_width, temp_height;
//
//	// get info about png
//	png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
//		NULL, NULL, NULL);
//
//	if (width){ *width = temp_width; }
//	if (height){ *height = temp_height; }
//
//	// Update the png info struct.
//	png_read_update_info(png_ptr, info_ptr);
//
//	// Row size in bytes.
//	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);
//
//	// glTexImage2d requires rows to be 4-byte aligned
//	rowbytes += 3 - ((rowbytes-1) % 4);
//
//	// Allocate the image_data as a big block, to be given to opengl
//	png_byte * image_data;
//	image_data = malloc(rowbytes * temp_height * sizeof(png_byte)+15);
//	if (image_data == NULL)
//	{
//		fprintf(stderr, "error: could not allocate memory for PNG image data\n");
//		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//		fclose(fp);
//		return 0;
//	}
//
//	// row_pointers is for pointing to image_data for reading the png with libpng
//	png_bytep * row_pointers = malloc(temp_height * sizeof(png_bytep));
//	if (row_pointers == NULL)
//	{
//		fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
//		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//		free(image_data);
//		fclose(fp);
//		return 0;
//	}
//
//	// set the individual row_pointers to point at the correct offsets of image_data
//	int i;
//	for (i = 0; i < temp_height; i++)
//	{
//		row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
//	}
//
//	// read the png into image_data through row_pointers
//	png_read_image(png_ptr, row_pointers);
//
//	// Generate the OpenGL texture object
//	GLuint texture;
//	glGenTextures(1, &texture);
//	glBindTexture(GL_TEXTURE_2D, texture);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp_width, temp_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	// clean up
//	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//	free(image_data);
//	free(row_pointers);
//	fclose(fp);
//	return texture;
//}

bool Texture::Load(string directory)
{ 


	/*Magick::Blob blob;
	Magick::Image* image; */
	string stringFileName(m_file_name);
	string fullPath = directory + "\\" + stringFileName;
	/*try {
	image = new Magick::Image(fullPath);
	image->write(&blob, "RGBA");
	}
	catch (Magick::Error& Error) {
	std::cout << "Error loading texture '" << fullPath << "': " << Error.what() << std::endl;
	return false;
	}
	*/

	//int col = image->columns();
	int width, height, channels;
	unsigned char *ht_map = SOIL_load_image
		(fullPath.c_str()
		,
		&width, &height, &channels,
		SOIL_LOAD_RGBA
		);


	glGenTextures(1, &id);
	//int width,height,channels;
	//unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, nullptr, 0);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, id);
	// Parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glGenerateMipmap(GL_TEXTURE_2D); 

	glTexImage2D(GL_TEXTURE_2D,  0, GL_RGBA , width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ht_map );
	// 

	/*id  = SOIL_load_OGL_texture
	(
	fullPath.c_str(),
	SOIL_LOAD_AUTO,
	SOIL_CREATE_NEW_ID,
	SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);*/

	// clean up
	/*png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	free(image_data);
	free(row_pointers);
	fclose(fp);*/
	//glBindTexture(GL_TEXTURE_2D, 0);


	//	delete image; 
	//	SOIL_free_image_data(image); 
	return true;
}



bool Texture::Load3D(  vector<string> textures)
{
	return true;
	//	GLuint texture;
	//	Magick::Blob blob;
	//	Magick::Image* image; 
	//
	//	if(textures.size() == 0)
	//	{
	//		return false;
	//	}
	//
	//	GLsizei width, height, depth = (GLsizei)textures.size();
	//
	//	string stringFileName(m_file_name);
	//	string fullPath = m_directory + "\\" + textures[0];
	//	try {
	//		image = new Magick::Image(fullPath.c_str());
	//		image->write(&blob, "RGBA");
	//
	//		width = image->columns();
	//		height = image->rows();
	//	}
	//	catch (Magick::Error& Error) {
	//		std::cout << "Error loading texture '" << fullPath << "': " << Error.what() << std::endl;
	//		return false;
	//	}
	//
	//	glGenTextures(1, &id);
	//	glBindTexture(GL_TEXTURE_3D, id);
	//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//	
	//	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, 
	//		height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	//
	//	glTexSubImage3D(
	//		GL_TEXTURE_3D, 0, 
	//		0, 0, (GLint)1,
	//		width, height, 1,
	//		GL_RGBA, GL_UNSIGNED_BYTE,
	//		blob.data()
	//		);
	//
	////	delete image;
	//
	//	for (int i = 1; i < textures.size(); i++)
	//	{
	//		fullPath = m_directory + "\\" + textures[i];
	//		image = new Magick::Image(fullPath.c_str());
	//		image->write(&blob, "RGBA");
	//		
	//		glTexSubImage3D(
	//			GL_TEXTURE_3D, 0, 
	//			0, 0, (GLint)i,
	//			width, height, 1,
	//			GL_RGBA, GL_UNSIGNED_BYTE,
	//			blob.data()
	//			);
	//
	//	//	delete image;
	//	} 
	//	m_has3dTexture = true;
	//	glGenerateMipmap(GL_TEXTURE_3D);
	//	glBindTexture(GL_TEXTURE_3D, 0);
}

#endif // Texture_h__