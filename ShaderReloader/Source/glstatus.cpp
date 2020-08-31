
#include "glstatus.h"

bool checkStatus(GLuint object_id, PFNGLGETSHADERIVPROC object_property_getter, PFNGLGETSHADERINFOLOGPROC info_log_func, GLenum status_type)
{
  // iv = integer vector. We are just passing one.
  GLint status;
  object_property_getter(object_id, status_type, &status);
  if (status != GL_TRUE)
  {
    GLint info_log_length;
    object_property_getter(object_id, GL_INFO_LOG_LENGTH, &info_log_length);
    GLchar* buffer = new GLchar[info_log_length];

    GLsizei buffer_size;
    info_log_func(object_id, info_log_length, &buffer_size, buffer);
    std::cout << buffer << std::endl;

    delete[] buffer;
    return false;
  }
  return true;
}