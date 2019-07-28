#include "Renderer.h"

Renderer::Renderer(int w, int h,std::string compute_folder)
{
	Initialize(w, h, compute_folder);
}

void Renderer::Initialize(int w, int h, std::string compute_folder)
{
	shader_textures.clear();
	global_size.clear();

	width = w;
	height = h;

	ExprParser parser;

	shader_folder = compute_folder;

	std::ifstream config(compute_folder + "/pipeline.cfg");
	std::string line;

	int element = 0;
	int cur_shader = 0;

	std::map<std::string, float> variables;
	variables["width"] = width;
	variables["height"] = height;

	std::vector<GLuint> stage_textures;
	std::string shader_file;
	vec2 global, tex_resolution;
	while (std::getline(config, line))
	{
		if (line.substr(0, 1) != "#")
		{
			parser.load_expr(line);
			switch (element++)
			{
			case 0:
				shader_file = compute_folder + "/" + line;
				LoadShader(shader_file);
				break;
			case 1:
				global.x = parser.Evaluate(variables);
				break;
			case 2:
				global.y = parser.Evaluate(variables);
				break;
			case 3:
				tex_resolution.x = parser.Evaluate(variables);
				break;
			case 4:
				tex_resolution.y = parser.Evaluate(variables);
				break;
			case 5:
				int tnum = parser.Evaluate(variables);
				for (int i = 0; i < tnum; i++)
				{
					stage_textures.push_back(GenerateTexture(tex_resolution.x, tex_resolution.y));
				}
				shader_textures.push_back(stage_textures);
				global_size.push_back(global);
				element = 0;
				break;
			}
		}
	}

	config.close();
}

void Renderer::LoadShader(std::string shader_file)
{
	shader_pipeline.push_back(ComputeShader(shader_file));
}

void Renderer::Render()
{
	for (int i = 0; i < global_size.size(); i++)
	{
		shader_pipeline[i].Run(global_size[i]);
	}
}

GLuint Renderer::GenerateTexture(float w, float h)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//HDR texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	return texture;
}
