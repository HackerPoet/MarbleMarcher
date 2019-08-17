#include "Renderer.h"

Renderer::Renderer(int w, int h,std::string compute_folder)
{
	Initialize(w, h, compute_folder);
}

Renderer::Renderer()
{
}

void Renderer::Initialize(int w, int h, std::string compute_folder)
{
	shader_textures.clear();
	global_size.clear();

	width = w;
	height = h;
	camera.SetResolution(vec2(w, h));
	camera.SetAspectRatio((float)w / (float)h);

	ExprParser parser;

	shader_folder = compute_folder;

	std::ifstream config(compute_folder + "/pipeline.cfg");
	if (config.fail())
	{
		ERROR_MSG("Error opening pipeline configuration");
		return;
	}
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
			parser.Parse(line);
			switch (element++)
			{
			case 0:
				shader_file = compute_folder + "/" + line;
				LoadShader(shader_file);
				break;
			case 1:
				global.x = ceil(parser.Evaluate(variables));
				break;
			case 2:
				global.y = ceil(parser.Evaluate(variables));
				break;
			case 3:
				tex_resolution.x = ceil(parser.Evaluate(variables));
				break;
			case 4:
				tex_resolution.y = ceil(parser.Evaluate(variables));
				break;
			case 5:
				int tnum = parser.Evaluate(variables);
				for (int i = 0; i < tnum; i++)
				{
					stage_textures.push_back(GenerateTexture(tex_resolution.x, tex_resolution.y));
				}
				shader_textures.push_back(stage_textures);
				stage_textures.clear();
				global_size.push_back(global);
				element = 0;
				break;
			}
		}
	}

	config.close();
}

void Renderer::ReInitialize(int w, int h)
{
	shader_textures.clear();
	global_size.clear();

	width = w;
	height = h;
	camera.SetResolution(vec2(w, h));
	camera.SetAspectRatio((float)w / (float)h);

	ExprParser parser;

	std::ifstream config(shader_folder + "/pipeline.cfg");
	if (config.fail())
	{
		ERROR_MSG("Error opening pipeline configuration");
		return;
	}
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
			parser.Parse(line);
			switch (element++)
			{
			case 0:
				//shader_file = compute_folder + "/" + line;
			//	LoadShader(shader_file);
				break;
			case 1:
				global.x = ceil(parser.Evaluate(variables));
				break;
			case 2:
				global.y = ceil(parser.Evaluate(variables));
				break;
			case 3:
				tex_resolution.x = ceil(parser.Evaluate(variables));
				break;
			case 4:
				tex_resolution.y = ceil(parser.Evaluate(variables));
				break;
			case 5:
				int tnum = parser.Evaluate(variables);
				for (int i = 0; i < tnum; i++)
				{
					stage_textures.push_back(GenerateTexture(tex_resolution.x, tex_resolution.y));
				}
				shader_textures.push_back(stage_textures);
				stage_textures.clear();
				global_size.push_back(global);
				element = 0;
				break;
			}
		}
	}

	config.close();
}

void Renderer::SetOutputTexture(sf::Texture & tex)
{
	shader_textures[shader_textures.size()-1][0] = tex.getNativeHandle();
}

void Renderer::LoadShader(std::string shader_file)
{
	shader_pipeline.push_back(ComputeShader(shader_file));
}

void Renderer::Render()
{
	int stages = global_size.size();
	for (int i = 0; i < stages; i++)
	{
		int tex_id = 0;

		//bind textures from the previous step
		if (i != 0)
		{
			for (int j = 0; j < shader_textures[i - 1].size(); j++)
			{
				glBindImageTexture(tex_id++, shader_textures[i - 1][j], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			}
		}

		//bind textures from the current step
		for (int j = 0; j < shader_textures[i].size(); j++)
		{
			glBindImageTexture(tex_id++, shader_textures[i][j], 0, GL_FALSE, 0, GL_READ_WRITE, (i == stages-1)?GL_RGBA8:GL_RGBA32F);
		}
		
		shader_pipeline[i].setCamera(camera.GetGLdata());
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
	return texture;
}
