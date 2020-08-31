//============================================================================
// # ShaderReloader #
// Name: glshader.h
// Author: Fredrik Linde
// Created on: Apr 26, 2019
// Description: OpenGL shader wrapper.
//============================================================================

#ifndef GLSHADER_H
#define GLSHADER_H

#include <gl/glew.h>
#include <string>

struct TypePair
{
	TypePair() : _name(""), _type(0) {}
	TypePair(const std::string name, const GLenum type) : _name(name), _type(type) {}
	bool operator<(const TypePair& rhs) const
	{
		return std::tie(_name, _type) < std::tie(rhs._name, rhs._type);
	}
	std::string _name;
	GLenum _type;
};

struct GLShaderHandle 
{
	GLShaderHandle() :
		_name(""),
		_path(""),
		_type(0),
		_id(0),
		_hash(0){}

	std::string _name;
	std::string _path;
	GLenum _type;

	int32_t _hash;
	GLuint _id;
};

class GLShader
{
public:

  static bool initialize(GLShaderHandle& handle, const char* path);
  static bool getNameTypePair(TypePair& tp, std::string path);

  static GLuint create(GLenum type);
  static void remove(GLuint id);

  static bool compile(GLShaderHandle handle);
  static bool status(GLuint id);

  static std::string readShaderCode(std::string file_name);
};

#endif
