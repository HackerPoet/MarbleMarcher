#pragma once

#include<Shaders.h>
#include<ExprParser.h>

class Renderer
{
public:
	Renderer(int w, int h, std::string compute_folder);
	Renderer();

	void Initialize(int w, int h, std::string compute_folder);

	void SetOutputTexture(sf::Texture& tex);
	void LoadShader(std::string shader_file);

	void Render();
	std::vector<ComputeShader> shader_pipeline;
	Camera camera;

private:
	std::string shader_folder;
	GLuint GenerateTexture(float w, float h);

	int width, height;
	std::vector<vec2> global_size;
	std::vector<std::vector<GLuint>> shader_textures;
};