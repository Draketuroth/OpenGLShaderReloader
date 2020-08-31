//============================================================================
// # ShaderReloader #
// Name: glshader.cpp
// Author: Fredrik Linde
// Created on: Apr 26, 2019
// Description: OpenGL shader wrapper.
//============================================================================

#include <fstream>
#include <iostream>

#include "glstatus.h"
#include "glshader.h"

bool GLShader::initialize(GLShaderHandle& handle, const char* path)
{
  handle._path = path;

  TypePair tp;
  if (getNameTypePair(tp, handle._path)) 
  {
	handle._name = tp._name;
	handle._type = tp._type;
	handle._id = create(handle._type);
    handle._hash = (int32_t)std::hash<std::string>()(handle._name) & 0x7FFF;

	if (!compile(handle))
	{
	  std::cout << "Failed to compile shader " << tp._name << std::endl;
	  return false;
	}

  }
  else 
  {
	  std::cout << "GLShader::initialize could not construct typepair" << std::endl;
  }

  return true;
}

bool GLShader::getNameTypePair(TypePair& tp, std::string path)
{
	std::string fileStr = path;

	// Get name.
	size_t delimiter = fileStr.find_last_of('\\');
	if (delimiter == std::string::npos)
	{
		std::cout << "Failed to acquire shader name from path" << std::endl;
		return false;
	}
	tp._name = fileStr.substr(delimiter + 1);

	// Get type.
	delimiter = tp._name.find_last_of('.');
	if (delimiter == std::string::npos)
	{
		std::cout << "Failed to acquire shader type from path" << std::endl;
		return false;
	}
	std::string extension = tp._name.substr(delimiter + 1);
	if (extension == "vert")
	{
		tp._type = GL_VERTEX_SHADER;
	}
	else if (extension == "frag")
	{
		tp._type = GL_FRAGMENT_SHADER;
	}
	else
	{
		std::cout << "Failed to set shader type from extension" << std::endl;
		return false;
	}

	return true;
}

GLuint GLShader::create(GLenum type)
{
	return glCreateShader(type);
}

void GLShader::remove(GLuint id)
{
	glDeleteShader(id);
}

bool GLShader::compile(GLShaderHandle handle)
{
	const char* adapter[1];
	std::string temp = readShaderCode(handle._path);
	adapter[0] = temp.c_str();
	
	glShaderSource(handle._id, 1, adapter, 0);
	glCompileShader(handle._id);
	
	if (!status(handle._id))
	{
		return false;
	}
	return true;
}

bool GLShader::status(GLuint id)
{
	return checkStatus(id, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

std::string GLShader::readShaderCode(std::string file_name)
{
  std::ifstream shader_input(file_name);
  if (!shader_input.good())
  {
    std::cout << "File failed to load..." << file_name << std::endl;
    exit(1);
  }
  return std::string(
    std::istreambuf_iterator<char>(shader_input),
    std::istreambuf_iterator<char>());
}
