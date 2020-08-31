//============================================================================
// # ShaderReloader #
// Name: glshaderset.h
// Author: Fredrik Linde
// Created on: May 5, 2019
// Description: OpenGL shader set for gl programs.
//============================================================================

#ifndef GLSHADERSET_H
#define GLSHADERSET_H

#include <gl/glew.h>
#include <map>
#include <vector>

#include "glshader.h"
#include "glprogram.h"
#include "enums.h"

#include "topic.h"

class GLShaderSet : public Subscriber
{
public:
	GLShaderSet() = default;
	~GLShaderSet();

	GLuint* addProgram(const std::vector<GLShader>& shaders);
	GLuint* addProgramFromShaders(const std::vector<GLShaderHandle>& shaders);

	void update(Publisher* pub, void* msg = 0);
	void updatePrograms(bool& refreshFlag);

private:

	// Map shaders to handles in order to reuse shared shaders.
	std::map<TypePair, GLShaderHandle>  _shaders;

	// Lookup table for program with matching set of shaders.
	std::map<std::vector<const TypePair*>, GLProgramHandle> _programs;

	std::vector<TypePair> _updatedShaders;
};


#endif
