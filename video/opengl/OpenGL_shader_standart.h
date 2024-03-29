﻿#ifndef YY_OPENGL_SHADER_STD_H__
#define YY_OPENGL_SHADER_STD_H__

class OpenGLShaderStd
{
public:
	OpenGLShaderStd();
	~OpenGLShaderStd();

	GLuint m_program;
	GLuint m_VAO;
	GLint m_uniform_WVP;
	GLint m_uniform_W;
	GLint m_uniform_LightView;
	GLint m_uniform_LightProjection;
	GLint m_uniform_sunDir;
	GLint m_uniform_ambientColor;
	GLint m_uniform_selfLight;

//	GLint m_uniform_diffuseTexture = 0;

	bool init();
};

#endif