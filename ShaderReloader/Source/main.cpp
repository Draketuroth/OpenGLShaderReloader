//============================================================================
// # ShaderReloader #
// Name: main.cpp
// Author: Fredrik Linde
// Created on: Apr 25, 2019
// Description: Application entry point
//============================================================================

#include <gl/glew.h>
#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <thread>
#include <functional>
#include <atomic>
#include <chrono>

#include "glassert.h"
#include "glshaderset.h"

#include "filenotifier.h"

#include "glfw/applicationglfw.h"

#include "filereadstream.h"
#include "document.h"
#include "istreamwrapper.h"
#include "pointer.h"

static void infoGL()
{
  glCheckError();
  const GLubyte *str;
  std::cout << "OpenGL Version Info" << std::endl;
  str = glGetString(GL_RENDERER);
  std::cout << "Renderer : " << str << std::endl;
  str = glGetString(GL_VENDOR);
  std::cout << "Vendor : " << str << std::endl;
  str = glGetString(GL_VERSION);
  std::cout << "OpenGL Version : " << str << std::endl;
  str = glGetString(GL_SHADING_LANGUAGE_VERSION);
  std::cout << "GLSL Version : " << str << std::endl;
  glCheckError();
}

void sendDataToOpenGL()
{
  GLfloat verts[] =
  {
    +0.0f, +1.0f,
    -1.0f, -1.0f,
    +1.0f, -1.0f,
  };
  GLuint vertex_buffer_ID;
  glGenBuffers(1, &vertex_buffer_ID);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_ID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

int main(int argc, char** argv) 
{
  if (!glfw::init())
  {
    return false;
  }

  int w = 800;
  int h = 600;
  int vw = w;
  int vh = h;
  std::string version = "";

  std::ifstream config_input("Config\\ShaderReloader.json");
  if (!config_input.good()) 
  {
	  std::cout << "Could not find config file" << std::endl;
	  return 0;
  }
  rapidjson::IStreamWrapper config_stream(config_input);

  rapidjson::Document document;
  document.ParseStream(config_stream);
  if (document.IsObject()) 
  {
	  if (document.HasMember("version")) 
	  {
		  if (document["version"].IsString()) 
		  {
			  version = document["version"].GetString();
		  }
	  }
	  if (document.HasMember("display")) 
	  {
		  if (document["display"].IsArray()) 
		  {
			  rapidjson::Value& parameter = document["display"];
			  for (rapidjson::SizeType i = 0; i < parameter.Size(); i++) {
				  vw = w = parameter[i]["width"].GetInt();
				  vh = h = parameter[i]["height"].GetInt();
			  }
		  }
	  }
  }

  std::string title = "ShaderReloader v.";
  title.append(version);

  glfw::Window* window = new glfw::Window(w, h, title.c_str(), NULL);
  if (!window)
  {
    glfwTerminate();
    return false;
  }

  window->makeContextCurrent();

  bool needsRefresh = true;

  std::unique_ptr<glfw::EventHandler<class glfw::WindowEvent>>* resizeHandler = new std::unique_ptr<glfw::EventHandler<class glfw::WindowEvent>>(new glfw::EventHandler<class glfw::WindowEvent>(std::function<void(glfw::WindowEvent*)>([&](glfw::WindowEvent * e) {
	  vw = e->getWidth();
	  vh = e->getHeight();
	  std::cout << "Width: " << vw << ", Height: " << vh << std::endl;
	  needsRefresh = true;
	  })));

  std::unique_ptr<glfw::EventHandler<class glfw::WindowEvent>>* closeHandler = new std::unique_ptr<glfw::EventHandler<class glfw::WindowEvent>>(new glfw::EventHandler<class glfw::WindowEvent>(std::function<void(glfw::WindowEvent*)>([](glfw::WindowEvent * e) {
	  std::cout << "Accepting window close request!" << std::endl;
	  if (e->getEventSource()->isDestroyed())e->getEventSource()->destroy();

	  })));

  window->setOnWindowResized(resizeHandler->get());
  window->setOnWindowCloseRequest(closeHandler->get());

  GLenum error_code = glewInit();
  assert(error_code == 0);

  infoGL();

  sendDataToOpenGL();

  GLShaderHandle vertex_shader;
  GLShader::initialize(vertex_shader, "Shaders\\default.vert");

  GLShaderHandle fragment_shader;
  GLShader::initialize(fragment_shader, "Shaders\\default.frag");

  GLShaderSet set;
  GLuint* program = set.addProgramFromShaders({ vertex_shader, fragment_shader });

  FileNotifier fn("Shaders");
  fn.subscribe(&set);

  while (!window->isDestroyed())
  {
	set.updatePrograms(needsRefresh);
	glUseProgram(*program);

	if (needsRefresh) 
	{
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, vw, vh);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		window->swapBuffers();

		needsRefresh = false;
	}
	glfw::Window::pollEvents();
  }

  glfw::terminate();

  glDeleteProgram(*program);
  program = nullptr;
  std::cout << "deleted program object!" << std::endl;

  delete window;
  delete resizeHandler->release();
  delete closeHandler->release();
  delete resizeHandler;
  delete closeHandler;
  std::cout << "deleted window object!" << std::endl;

  system("pause");
  return 0;
}