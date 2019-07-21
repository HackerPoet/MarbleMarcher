#pragma once

class ComputeShader
{
public:
	GLuint ProgramID;

	ShaderHandle()
	{}

	void LoadShader(const std::string file_path);

	void setUniform(string name, float X, float Y);
	void setUniform(string name, float X, float Y, float Z);
	void setUniform(string name, float X);
	void setUniform(string name, int X);
	void setUniform(string name, Eigen::Matrix3f X, bool transpose);
	void setUniform(string name, Eigen::Vector3f X);


	GLuint getNativeHandle()
	{
		return ProgramID;
	}
};
