#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <fstream>
#include <string>
#include <glad/glad.h>

class ShaderLoader {
public:
	ShaderLoader() = default;
	~ShaderLoader() = default;

	static GLuint LoadShader(const std::string_view vertexPath, const std::string_view fragmentPath);
};


#endif