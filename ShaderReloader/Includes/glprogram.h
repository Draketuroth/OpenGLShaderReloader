//============================================================================
// # ShaderReloader #
// Name: glprogram.h
// Author: Fredrik Linde
// Created on: Apr 26, 2019
// Description: OpenGL shader program container.
//============================================================================

#ifndef GLPROGRAM_H
#define GLPROGRAM_H

#include <memory>
#include <gl/glew.h>

#include <glshader.h>

struct GLProgramHandle 
{
	GLProgramHandle() :
	  _publicID(0),
	  _internalID(0)
	  {
	  }

	GLuint _publicID;
	GLuint _internalID;
};

class GLProgram
{
public:

  static GLProgramHandle initialize();
  static void remove(GLuint id);

  static bool link(GLProgramHandle &handle);
  static void use(const GLProgramHandle handle);

  static bool status(GLuint id);

};

#endif