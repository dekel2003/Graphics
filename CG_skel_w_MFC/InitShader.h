#pragma once
static char* readShaderSource(const char* shaderFile);
GLuint InitShader(const char* vShaderFile, const char* fShaderFile);

struct Shader {
	const char*  filename;
	GLenum       type;
	GLchar*      source;
};