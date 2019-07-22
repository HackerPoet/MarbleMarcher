#pragma once
#include <string>
#include <GL/glew.h>
#include <glm\glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <fstream>
#include <Camera.h>

class ComputeShader
{
public:
	GLuint ProgramID;

	ComputeShader();

	void LoadShader(const std::string file_path);

	void setUniform(std::string name, float X, float Y);
	void setUniform(std::string name, float X, float Y, float Z);
	void setUniform(std::string name, float X);
	void setUniform(std::string name, int X);
	void setUniform(std::string name, glm::mat3 X, bool transpose);
	void setUniform(std::string name, glm::vec3 X);
	void setCamera(gl_camera cam);

	GLuint getNativeHandle();
};
