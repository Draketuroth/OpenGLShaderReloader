//============================================================================
// # ShaderReloader #
// Name: glstatus.h
// Author: Fredrik Linde
// Created on: Apr 26, 2019
// Description: OpenGL error checking functions.
//============================================================================

#ifndef GLSTATUS_H
#define GLSTATUS_H

#include <gl/glew.h>
#include <iostream>

bool checkStatus(GLuint object_id, PFNGLGETSHADERIVPROC object_property_getter, PFNGLGETSHADERINFOLOGPROC info_log_func, GLenum status_type);

#endif
