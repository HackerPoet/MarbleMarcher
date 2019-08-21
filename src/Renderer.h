#pragma once

#include<Shaders.h>
#include<ExprParser.h>

//TODO
class Wobject
{
	vec3 position;
	vec3 radius; //bounding volume radius

};

class WorldModel
{

};

class Renderer
{
public:
	Renderer(int w, int h, std::string config);
	Renderer();

	void Initialize(int w, int h, std::string config);
	void ReInitialize(int w, int h);

	void SetOutputTexture(sf::Texture& tex);
	void LoadShader(std::string shader_file);

	void Render();
	std::vector<ComputeShader> shader_pipeline;
	Camera camera;

private:
	std::string config_file;
	GLuint GenerateTexture(float w, float h);

	int width, height;
	std::vector<vec2> global_size;
	std::vector<GLuint> main_textures;
	std::vector<std::vector<GLuint>> shader_textures;
};