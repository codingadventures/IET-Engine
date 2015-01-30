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

private:
	vector<string>	d_vertex_source_path;
	vector<string>	d_fragment_source_path;
	
	GLchar**		d_vertex_code;
	
	GLchar**		d_fragment_code;

	GLint*			d_vertex_string_count;
	GLint*			d_fragment_string_count;

	const size_t	d_n_vertex;
	const size_t	d_n_fragment;

	GLint			d_mvp_uniform; 
	GLint			d_vertex_shader;
	GLint			d_fragment_shader;

	string			d_model_uniform_name;




public:
	// Constructor reads and builds our shader
	Shader(string vertexSourcePath, string fragmentSourcePath, string modelUniformName = "mvp") 
		: d_n_vertex(1),d_n_fragment(1),d_model_uniform_name(modelUniformName)
	{
		
		d_vertex_source_path.push_back(vertexSourcePath);
		d_fragment_source_path.push_back(fragmentSourcePath);
		init();
	}

	Shader(vector<string> vertex_source_paths,vector<string> fragment_source_paths, string modelUniformName = "mvp")
		: 
		d_n_vertex(vertex_source_paths.size()),
		d_n_fragment(fragment_source_paths.size()),
		d_vertex_source_path(vertex_source_paths),
		d_fragment_source_path(fragment_source_paths),
		d_model_uniform_name(modelUniformName)
	{

		init();

	} 
	~Shader()
	{
	 
		delete[] d_vertex_code;
		delete[] d_fragment_code;

		delete d_vertex_string_count;
		delete d_fragment_string_count;
	}
	// Use our program
	void Use()
	{
		glUseProgram(this->Program);
	}

	void SetModel(glm::mat4 model)
	{
		glUniformMatrix4fv(d_mvp_uniform, 1, GL_FALSE, glm::value_ptr(model));

	}

	void SetModelViewProjection(glm::mat4 model,  glm::mat4 view, glm::mat4 projection)
	{
		glm::mat4 mvp = projection * view * model;
		glUniformMatrix4fv(d_mvp_uniform, 1, GL_FALSE, glm::value_ptr(mvp)); 
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

	void init()
	{
		init_pointers();
		load_vertex();
		load_fragment();
		compile();
		link(); 
		d_mvp_uniform = glGetUniformLocation(Program, d_model_uniform_name.c_str()); 

	}

	void load_vertex()
	{
		for (size_t i = 0; i < d_n_vertex; i++)
		{
			Load(d_vertex_source_path[i],   d_vertex_code[i], d_vertex_string_count[i]);
		}  
	}

	void load_fragment()
	{
		for (size_t i = 0; i < d_n_fragment; i++)
		{
			Load(d_fragment_source_path[i], d_fragment_code[i],d_fragment_string_count[i]);
		} 

	}

	static void Load(string source_path,  GLchar*& output,GLint& count )
	{
		string return_code;
		try 
		{
			// Open files
			ifstream vShaderFile(source_path);
			//ifstream fShaderFile(d_fragment_source_path);
			stringstream vShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			//fShaderStream << fShaderFile.rdbuf();		
			// close file handlers
			vShaderFile.close();
			//fShaderFile.close();
			// Convert stream into GLchar array
			return_code = vShaderStream.str();
			//	d_fragment_code = fShaderStream.str();		
		}
		catch(exception e)
		{
			cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << source_path << endl;
		}
		 
		count = return_code.length(); 
		output = new GLchar[count];
		std::size_t length = return_code.copy(output,count,0);
		output[length]= '\0';
	}

	void compile()
	{
		const GLchar* vertex_code_char;
		const GLchar* fragment_code_char;
		GLint success;
		GLchar infoLog[512];


		d_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		d_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);


#pragma region [ Vertex Shader ]
		glShaderSource(d_vertex_shader, d_n_vertex, d_vertex_code , d_vertex_string_count);
		glCompileShader(d_vertex_shader);

		glGetShaderiv(d_vertex_shader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(d_vertex_shader, 512, nullptr, infoLog);
			cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog <<  endl;
		}
#pragma endregion

#pragma region [ Fragment Shader ]
		glShaderSource(d_fragment_shader, d_n_fragment, d_fragment_code, d_fragment_string_count);
		glCompileShader(d_fragment_shader);
		glGetShaderiv(d_fragment_shader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(d_fragment_shader, 512, nullptr, infoLog);
			cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog <<  endl;
		} 
#pragma endregion

		this->Program = glCreateProgram();
		glAttachShader(this->Program, d_vertex_shader);
		glAttachShader(this->Program, d_fragment_shader); 
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

		glDeleteShader(d_vertex_shader);
		glDeleteShader(d_fragment_shader);  

	}

	void init_pointers()
	{
		d_vertex_code = new   GLchar*[d_n_vertex];
		d_vertex_string_count = new GLint[d_n_vertex];

		d_fragment_code = new    GLchar*[d_n_fragment];
		d_fragment_string_count = new GLint[d_n_fragment];
	}


};