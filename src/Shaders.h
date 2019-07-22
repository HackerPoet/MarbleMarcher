#pragma once
#include <string>
#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

class ComputeShader
{
public:
	GLuint ProgramID;

	ShaderHandle()
	{}

	void LoadShader(const std::string file_path);

	void setUniform(std::string name, float X, float Y);
	void setUniform(std::string name, float X, float Y, float Z);
	void setUniform(std::string name, float X);
	void setUniform(std::string name, int X);
	void setUniform(std::string name, Eigen::Matrix3f X, bool transpose);
	void setUniform(std::string name, Eigen::Vector3f X);


	GLuint getNativeHandle()
	{
		return ProgramID;
	}
};
