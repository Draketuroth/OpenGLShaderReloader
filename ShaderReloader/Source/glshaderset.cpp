
#include "glshaderset.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <set>

GLShaderSet::~GLShaderSet()
{

}

GLuint* GLShaderSet::addProgram(const std::vector<GLShader>& shaders)
{
	return nullptr;
}

GLuint* GLShaderSet::addProgramFromShaders(const std::vector<GLShaderHandle>& shaders)
{
	std::vector<const TypePair*> shaderHandles;

	// Find references to existing shaders and create ones that did not previously exist.
	for (const auto& shader : shaders) 
	{
		TypePair tmp(shader._name, shader._type);

		// Test that the file can be opened in order to catch typos.
		std::string path = "Shaders\\";
		path.append(tmp._name);
		std::ifstream file(path);
		if (!file) 
		{
			std::cout << "Failed to open shader " << tmp._name << std::endl;
		}
		
		// TODO: Do not make a copy, move shader!
	    auto foundShader = _shaders.emplace(std::move(tmp), std::move(shader)).first;
		if (!foundShader->second._id) 
		{
			// Shader was not initialized. Create it.
		}

		shaderHandles.push_back(&foundShader->first);
	}

	// Make sure the programs comes in a canonical order.
	std::sort(std::begin(shaderHandles), std::end(shaderHandles));
	shaderHandles.erase(std::unique(begin(shaderHandles), end(shaderHandles)), end(shaderHandles));

	// Find the program associated with these shaders, or create.
	auto foundProgram = _programs.emplace(std::vector<const TypePair*>(shaderHandles), GLProgramHandle{}).first;
	if (!foundProgram->second._internalID) 
	{
		foundProgram->second = GLProgram::initialize();
		for (const TypePair* key : shaderHandles) 
		{
			glAttachShader(foundProgram->second._internalID, _shaders[*key]._id);
		}
		if (!GLProgram::link(foundProgram->second)) 
		{
			std::cout << "Program could not be linked" << std::endl;
		}
	}

	return &foundProgram->second._publicID;
}

void GLShaderSet::update(Publisher* pub, void* msg)
{
	std::string* fileStr = static_cast<std::string*>(msg);

	// Construct key.
	TypePair key;
	if (GLShader::getNameTypePair(key, *fileStr)) 
	{
		// If shader is present in the shaderset, set it as dirty.
		const auto foundShader = _shaders.find(key);
		if (foundShader != _shaders.end())
		{
			_updatedShaders.emplace_back(key);
		}
	}
	else 
	{
		std::cout << "GLShaderSet::update could not reconstruct type pair" << std::endl;
	}
}

void GLShaderSet::updatePrograms(bool& refreshFlag)
{
	if (_updatedShaders.size() > 0) 
	{
		// Recompile all updated shaders.
		for (TypePair key : _updatedShaders)
		{
			auto shader = _shaders.at(key);
			GLShader::compile(shader);
		}

		// Relink all programs that had their shaders updated.
		for (std::pair<const std::vector<const TypePair*>, GLProgramHandle>& program : _programs)
		{
			bool programNeedsRelink = false;
			for (const TypePair* programShader : program.first)
			{
				for (const TypePair key : _updatedShaders)
				{
					if (key._name == programShader->_name && key._type == programShader->_type)
					{
						programNeedsRelink = true;
						break;
					}
				}
				if (programNeedsRelink)
				{
					break;
				}
			}

			// Clear.
			_updatedShaders.clear();

			// Do not attempt to link shaders that did not compile successfully.
			bool canRelink = true;
			if (programNeedsRelink)
			{
				for (const TypePair* programShader : program.first)
				{
					if (!GLShader::status(_shaders[*programShader]._id))
					{
						canRelink = false;
						break;
					}
				}
			}

			if (programNeedsRelink && canRelink)
			{
				GLProgram::link(program.second);
				refreshFlag = true;
			}
		}
	}
}
