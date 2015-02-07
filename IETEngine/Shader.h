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
	GLuint			m_program;

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





public:
	// Constructor reads and builds our shader
	Shader(string vertexSourcePath, string fragmentSourcePath) 
		: d_n_vertex(1),d_n_fragment(1) 
	{

		d_vertex_source_path.push_back(vertexSourcePath);
		d_fragment_source_path.push_back(fragmentSourcePath);
		init();
	}

	Shader(vector<string> vertex_source_paths,string fragmentSourcePath)
		: 
		d_n_vertex(vertex_source_paths.size()),
		d_vertex_source_path(vertex_source_paths), 
		d_n_fragment(1)
	{
		d_fragment_source_path.push_back(fragmentSourcePath);
		init();

	} 

	Shader(vector<string> vertex_source_paths,vector<string> fragment_source_paths )
		: 
		d_n_vertex(vertex_source_paths.size()),
		d_n_fragment(fragment_source_paths.size()),
		d_vertex_source_path(vertex_source_paths),
		d_fragment_source_path(fragment_source_paths) 
	{

		init();

	} 
	~Shader()
	{
		for (int i = 0; i < d_n_vertex; i++)
		{
			delete[] d_vertex_code[i];
		}

		for (int i = 0; i < d_n_fragment; i++)
		{
			delete[] d_fragment_code[i];
		}

		delete[] d_vertex_code;
		delete[] d_fragment_code;

		delete d_vertex_string_count;
		delete d_fragment_string_count;
	}
	// Use our program
	void Use()
	{
		glUseProgram(this->m_program);
	}


	void SetUniform(string name, glm::vec3 value){
		GLint uniform =  glGetUniformLocation(m_program, name.c_str());
		glUniform3fv(uniform, 1, glm::value_ptr(value));
	}

	void SetUniform(string name, glm::vec4 value){
		GLint uniform =  glGetUniformLocation(m_program, name.c_str());
		glUniform3fv(uniform, 1, glm::value_ptr(value));
	}

	void SetUniform(string name, glm::mat4 value){
		GLint uniform =  glGetUniformLocation(m_program, name.c_str());
		glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(value)); 
	}

	void SetUniform(string name, float value){
		GLint uniform =  glGetUniformLocation(m_program, name.c_str());
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

		count = return_code.length() ; 
		output = new GLchar[count + 1];
		std::size_t length = return_code.copy(output,count,0);
		output[length]= '\0';
	}

	void compile()
	{
		const GLchar* vertex_code_char;
		const GLchar* fragment_code_char;
		GLint success;
		GLint logSize = 0;
		GLchar* infoLog;


		d_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		d_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);


#pragma region [ Vertex Shader ]
		glShaderSource(d_vertex_shader, d_n_vertex, d_vertex_code , d_vertex_string_count);
		glCompileShader(d_vertex_shader);

		glGetShaderiv(d_vertex_shader, GL_COMPILE_STATUS, &success);
		if(!success)
		{ 
			glGetShaderiv(d_vertex_shader, GL_INFO_LOG_LENGTH, &logSize);
			infoLog = new GLchar[logSize];

			glGetShaderInfoLog(d_vertex_shader, logSize, nullptr, infoLog);
			cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog <<  endl;
			delete infoLog;
		}
#pragma endregion

#pragma region [ Fragment Shader ]
		glShaderSource(d_fragment_shader, d_n_fragment, d_fragment_code, d_fragment_string_count);
		glCompileShader(d_fragment_shader);
		glGetShaderiv(d_fragment_shader, GL_INFO_LOG_LENGTH, &success);
		if(!success)
		{
			glGetShaderiv(d_fragment_shader, GL_INFO_LOG_LENGTH, &logSize);

			infoLog = new GLchar[logSize];

			glGetShaderInfoLog(d_fragment_shader, logSize, nullptr, infoLog);
			cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog <<  endl;
			delete infoLog;
		} 
#pragma endregion

		this->m_program = glCreateProgram();
		glAttachShader(this->m_program, d_vertex_shader);
		glAttachShader(this->m_program, d_fragment_shader); 
	}

	void link()
	{
		GLint success;
		GLchar* infoLog;

		glLinkProgram(this->m_program);

		glGetProgramiv(this->m_program, GL_LINK_STATUS, &success);

		if(!success)
		{
			GLint logSize = 0;
			glGetProgramiv(this->m_program, GL_INFO_LOG_LENGTH, &logSize);
			infoLog = new GLchar[logSize];
			glGetProgramInfoLog(this->m_program, logSize, nullptr, infoLog);
			cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n";
			cout << "File names: \n";
			for (auto vertex : d_vertex_source_path)
			{
				cout << vertex + "\n";
			}

			for (auto fragment : d_fragment_source_path)
			{
				cout << fragment + "\n";
			}

			cout  << infoLog <<  endl;
			delete infoLog;
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