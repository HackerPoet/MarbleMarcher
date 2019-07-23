#pragma once

#include<Shaders.h>

class Renderer
{
public:
	Renderer(std::string compute_folder);

	void SetResolution(int w, int h);
	void SetOutputTexture(sf::Texture& tex);
	void LoadPipelineFromFolder(std::string compute_folder);
	void LoadConfig(std::string pipeline_cfg);
	void LoadShader(std::string shader_file);

	void Render();

private:
	
	int width, height;
	std::vector<vec2> global_size;
	std::vector<vec2> work_group_size;
	std::vector<std::vector<GLint>> shader_textures;
	std::vector<ComputeShader> shader_pipeline;
};