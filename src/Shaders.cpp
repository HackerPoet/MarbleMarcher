#include "Shaders.h"

ComputeShader::ComputeShader()
{
	int work_grp_cnt[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
}

ComputeShader::ComputeShader(const std::string file_path)
{
	LoadShader(file_path);
}

std::string ComputeShader::LoadFileText(fs::path path)
{
	std::string text;
	std::ifstream TextStream(path, std::ios::in);
	if (TextStream.is_open())
	{
		std::string Line = "";
		while (getline(TextStream, Line))
			text += Line + "\n";
		TextStream.close();
	}
	else
	{
		ERROR_MSG("Impossible to open text file");
	}
	return text;
}

void ComputeShader::LoadShader(const std::string file_path)
{
		// Create the shaders
		GLuint ComputeShaderID = glCreateShader(GL_COMPUTE_SHADER);

		// Read the Vertex Shader code from the file
		std::string ComputeShaderCode = PreprocessIncludes(fs::path(file_path));

		GLint Result = GL_FALSE;
		int InfoLogLength;

		// Compile Compute Shader
		char const * ComputeSourcePointer = ComputeShaderCode.c_str();
		glShaderSource(ComputeShaderID, 1, &ComputeSourcePointer, NULL);
		glCompileShader(ComputeShaderID);

		// Check Compute Shader
		glGetShaderiv(ComputeShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(ComputeShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ComputeShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(ComputeShaderID, InfoLogLength, NULL, &ComputeShaderErrorMessage[0]);
			ERROR_MSG(("Compute shader compilation error. \n" + std::string(&ComputeShaderErrorMessage[0])).c_str());
		}

		// Link the program
		ProgramID = glCreateProgram();
		glAttachShader(ProgramID, ComputeShaderID);
		glLinkProgram(ProgramID); 

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			ERROR_MSG(("Compute program error. \n" + std::string(&ProgramErrorMessage[0])).c_str());
		}
}

void ComputeShader::Run(vec2 global)
{
	glUseProgram(ProgramID);
	glDispatchCompute(global.x, global.y, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ComputeShader::setUniform(std::string name, float X, float Y)
{
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform2f(A, X, Y);
}

void ComputeShader::setUniform(std::string name, float X, float Y, float Z)
{
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform3f(A, X, Y, Z);
}

void ComputeShader::setUniform(std::string name, float X)
{
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform1f(A, X);
}

void ComputeShader::setUniform(std::string name, int X)
{
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform1i(A, X);
}

void ComputeShader::setUniform(std::string name, glm::mat3 X, bool transpose)
{
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniformMatrix3fv(A, 1, transpose, glm::value_ptr(X));
}

void ComputeShader::setUniform(std::string name, glm::vec3 X)
{
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform3fv(A, 1, glm::value_ptr(X));
}

void ComputeShader::setUniform(std::string name, glm::vec2 X)
{
	GLuint A = glGetUniformLocation(ProgramID, name.c_str());
	glUniform2fv(A, 1, glm::value_ptr(X));
}

void ComputeShader::setCamera(gl_camera cam)
{
	setUniform("Camera.position", cam.position);
	setUniform("Camera.bokeh", cam.bokeh);
	setUniform("Camera.dirx", cam.dirx);
	setUniform("Camera.diry", cam.diry);
	setUniform("Camera.dirz", cam.dirz);
	setUniform("Camera.exposure", cam.exposure);
	setUniform("Camera.focus", cam.focus);
	setUniform("Camera.FOV", cam.FOV);
	setUniform("Camera.mblur", cam.mblur);
	setUniform("Camera.position", cam.position);
	setUniform("Camera.resolution", cam.resolution);
	setUniform("Camera.size", cam.size);
	setUniform("Camera.speckle", cam.speckle);
	setUniform("Camera.stepN", cam.stepN);
}

GLuint ComputeShader::getNativeHandle()
{
	return ProgramID;
}

bool INIT()
{
	if (glewInit() != GLEW_OK) {
		ERROR_MSG("Failed to initialize GLEW\n");
		return false;
	}
	return true;
}


std::string ComputeShader::PreprocessIncludes(const fs::path& filename, int level /*= 0 */)
{
	if (level > 32)
		ERROR_MSG("Header inclusion depth limit reached, might be caused by cyclic header inclusion");
	using namespace std;

	static const regex re("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
	stringstream input;
	stringstream output;
	input << LoadFileText(filename);

	smatch matches;
	string line;
	while (std::getline(input, line))
	{
		if (regex_search(line, matches, re))
		{
			std::string include_file = matches[1];
			output << PreprocessIncludes(include_file, level + 1) << endl;
		}
		else
		{
			output << line << "\n";
		}
	}
	return output.str();
}
