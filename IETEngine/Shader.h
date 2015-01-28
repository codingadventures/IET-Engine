#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

#include <GL/glew.h>;  

class Shader
{
public:
	// Our program ID
	GLuint Program;
	// Constructor reads and builds our shader
	Shader(const GLchar* vertexSourcePath, const GLchar* fragmentSourcePath
		, string modelUniformName = "model"
		, string viewUniformName = "view"
		, string projectionUniformName = "projection") 
		: vertexSourcePath(vertexSourcePath),fragmentSourcePath(fragmentSourcePath)
	{
		load();
		compile();
		link(); 
		modelUniform = glGetUniformLocation(Program, modelUniformName.c_str());
		viewUniform = glGetUniformLocation(Program, viewUniformName.c_str());
		projectionUniform = glGetUniformLocation(Program, projectionUniformName.c_str());
	}

	~Shader()
	{
		delete vertexSourcePath;
		delete fragmentSourcePath;
	}
	// Use our program
	void Use()
	{
		glUseProgram(this->Program);
	}

	void SetModel(glm::mat4 model)
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model));

	}

	void SetModelViewProjection(glm::mat4 model,  glm::mat4 view, glm::mat4 projection)
	{
		glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewUniform, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));
	}

	void SetUniform(string name, glm::vec3 value){
		GLint uniform =  glGetUniformLocation(Program, name.c_str());
		glUniform3fv(uniform, 1, glm::value_ptr(value));
	}

	void SetUniform(string name, float value){
		GLint uniform =  glGetUniformLocation(Program, name.c_str());
		glUniform1f(uniform, value);
	}
private:
	 
	GLint modelUniform;
	GLint viewUniform ;
	GLint projectionUniform;
	GLint vertexShader;
	GLint fragmentShader;

	const GLchar* vertexSourcePath;
	const GLchar* fragmentSourcePath;

	string vCode,fCode;


	void load()
	{
		// 1. Retrieve the vertex/fragment source code from filePath
		try 
		{
			// Open files
			ifstream vShaderFile(vertexSourcePath);
			ifstream fShaderFile(fragmentSourcePath);
			stringstream vShaderStream, fShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();		
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// Convert stream into GLchar array
			vCode = vShaderStream.str();
			fCode = fShaderStream.str();		


		}
		catch(exception e)
		{
			cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
		}
	}
	void compile()
	{
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		const GLchar* vertexCode =  vCode.c_str();;
		const GLchar* fragmentCode = fCode.c_str();
#pragma region [ Vertex Shader ]
		glShaderSource(vertexShader, 1, &vertexCode , nullptr);
		glCompileShader(vertexShader);

		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
			cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog <<  endl;
		}
#pragma endregion
#pragma region [ Fragment Shader ]
		glShaderSource(fragmentShader, 1, &fragmentCode, nullptr);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
			cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog <<  endl;
		} 
#pragma endregion

		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertexShader);
		glAttachShader(this->Program, fragmentShader); 
	}
	void link()
	{
		GLint success;
		GLchar infoLog[512];

		glLinkProgram(this->Program);

		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);

		if(!success)
		{
			GLint maxLength = 0;

			glGetShaderInfoLog(this->Program, 512, nullptr, infoLog);
			cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog <<  endl;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);  

	}

};