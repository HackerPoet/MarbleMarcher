/* This file is part of the Marble Marcher (https://github.com/HackerPoet/MarbleMarcher).
* Copyright(C) 2018 CodeParade
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include <fstream>
#include <vector>
#include <Eigen/Dense>
#include <SFML/Audio.hpp>
#include <filesystem>
#include <map>
#include <algorithm>

#ifdef _WIN32
#include <Windows.h>
#define ERROR_MSG(x) MessageBox(nullptr, TEXT(x), TEXT("ERROR"), MB_OK);
#else
#define ERROR_MSG(x) std::cerr << x << std::endl;
#endif

namespace fs = std::filesystem;

static const int num_levels = 24;
static const int num_fractal_params = 9;
typedef Eigen::Matrix<float, num_fractal_params, 1> FractalParams;

struct LevelF
{
	int FractalIterations;
	float FractalParams[9];
	float PBR_roughness;	 //Fractal roughness
	float PBR_metal;         //Fractal metalicity

	float marble_rad;        //Radius of the marble
	float start_look_x;      //Camera direction on start
	float orbit_dist;        //Distance to orbit
	float start_pos[3];      //Starting position of the marble
	float end_pos[3];        //Ending goal flag position
	float kill_y;            //Below this height player is killed
	bool planet;             //True if gravity should be like a planet
	char txt[255];           //Title
	char desc[255];			 //Description
	float anim_1;            //Animation amount for angle1 parameter
	float anim_2;            //Animation amount for angle2 parameter
	float anim_3;            //Animation amount for offset_y parameter
	float light_dir[3];	     //Sun light direction
	float light_col[3];      //Sun light color
	float background_col[3];

	int level_id;	     	 //level identifier
	int link_level;			 //Play what level after finish
	char use_music[255];     //what track to use

	float ground_force;
	float air_force;
	float ground_friction;
	float air_friction;
	float gravity;
	float ground_ratio;

	float gravity_dir[3];

	//Advanced Animation
	float FractalParamsAmp[9];
	float FractalParamsFrq[9];
};

class Level {
public:
  Level() {}
  Level(float s, float a1, float a2,
        const Eigen::Vector3f& v,
        const Eigen::Vector3f& c,
        float rad,
        float look_x,
        float orbit_d,
        const Eigen::Vector3f& start,
        const Eigen::Vector3f& end,
        float kill,
        bool planet,
        const char* desc,
        float an1=0.0f, float an2=0.0f, float an3=0.0f);

  void LoadFromFile(fs::path path);
  void SaveToFile(std::string file, int ID, int Link);

  void LoadLevelF(LevelF lvl);
  LevelF GetLevelF();

  int FractalIter;			 //Fractal iterations
  FractalParams params;      //Fractal parameters

  float PBR_roughness;		 //Fractal roughness
  float PBR_metal;           //Fractal metalicity

  float marble_rad;          //Radius of the marble
  float start_look_x;        //Camera direction on start
  float orbit_dist;          //Distance to orbit
  Eigen::Vector3f start_pos; //Starting position of the marble
  Eigen::Vector3f end_pos;   //Ending goal flag position
  float kill_y;              //Below this height player is killed
  bool planet;               //True if gravity should be like a planet
  std::string txt;           //Title
  std::string desc;			 //Description
  float anim_1;              //Animation amount for angle1 parameter
  float anim_2;              //Animation amount for angle2 parameter
  float anim_3;              //Animation amount for offset_y parameter
  Eigen::Vector3f light_dir; //Sun light direction
  Eigen::Vector3f light_col; //Sun light color

  int level_id;				 //level identifier
  int link_level;			 //Play what level after finish
  std::string use_music;     //what track to use

  Eigen::Vector3f background_col;

  float ground_force;
  float air_force;
  float ground_friction;
  float air_friction;
  float gravity;
  float ground_ratio;

  Eigen::Vector3f gravity_dir;

  //Advanced Animation
  FractalParams FractalParamsAmp;
  FractalParams FractalParamsFrq;
};

extern Level all_levels[num_levels];
extern Level default_level;

std::vector<fs::path> GetFilesInFolder(std::string folder, std::string filetype);

struct Score
{
	int level_id = 0;
	int played_num = 0;
	float best_time = 0.f;
	float all_time = 0.f;
	float last_time = 0.f;
};

class All_Levels
{
public:
	All_Levels() {}

	void LoadLevelsFromFolder(std::string folder);
	void LoadMusicFromFolder(std::string folder);

	Level GetLevel(int ID);
	int GetLevelNum();
	std::vector<std::string> getLevelNames();
	std::vector<std::string> getLevelDesc();
	std::vector<int> getLevelIds();
	std::map<int, Score> getLevelScores();
	sf::Music* GetLevelMusic(int ID);

	void ReloadLevels();

	void LoadLevelFromFile(fs::path file);
	void LoadScoresFromFile(std::string file);
	float GetBest(int lvl);
	void SaveScoresToFile();

	bool UpdateScore(int lvl, float time);
	sf::Music* GetMusicByID(int ID);

	void StopAllMusic();

	std::vector<std::string> GetMusicNames();

private:
	std::map<int, Level> level_map;
	std::map<int, Score> score_map;
	std::map<int, int> level_id_map;
	std::vector<std::string> level_names;
	std::vector<std::string> level_descriptions;
	std::vector<int> level_ids;

	std::map<std::string, sf::Music*> music_map;
	std::vector<std::string> music_names;

	std::string lvl_folder;

	int level_num;
};

std::string ConvertSpaces2_(std::string text);