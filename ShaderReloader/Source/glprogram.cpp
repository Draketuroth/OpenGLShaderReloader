//============================================================================
// # ShaderReloader #
// Name: glprogram.cpp
// Author: Fredrik Linde
// Created on: Apr 26, 2019
// Description: OpenGL shader program container.
//============================================================================

#include "glprogram.h"
#include "glstatus.h"
#include "glshader.h"

GLProgramHandle GLProgram::initialize()
{
  GLProgramHandle handle;
  handle._internalID = glCreateProgram();
  return handle;
}

void GLProgram::remove(GLuint id)
{
	glUseProgram(0);
	glDeleteProgram(id);
}

bool GLProgram::link(GLProgramHandle &handle)
{
  glLinkProgram(handle._internalID);

  if (!status(handle._internalID))
  {
	handle._publicID = 0;
    return false;
  }
  handle._publicID = handle._internalID;

  return true;
}

void GLProgram::use(const GLProgramHandle handle)
{
  glUseProgram(handle._publicID);
}

bool GLProgram::status(GLuint id)
{
	return checkStatus(id, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}