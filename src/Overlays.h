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
#include "Settings.h"
#include "Level.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>

#include <AntTweakBar.h>


extern Settings game_settings;

const int Element_Height = 50;
const int Descr_Height = 25;


//Generalized menu overlay 
class Menu
{
public:
	enum ElementType
	{
		Button,
		LevelButton
	};

	Menu();

	void AddFonts(sf::Font * a, sf::Font * b);

	void SetPosition(int posx, int posy);
	void SetScale(float scale);
	void AddButton(std::string text);
	void AddLevelButton(int LVL_ID, std::string name, std::string desc, std::string best_time);

	void UpdateState(sf::Vector2f mouse, bool all_keys[]);
	void UpdateMenu(int px, int py, int scroll);

	int WhichActive();
	int WhatLevelActive();
	int GetSelection();

	void SetText(std::string str, float x, float y, float size, const sf::Color & color, bool mono);
	bool IsEdit();
	void RenderMenu(sf::RenderWindow& window);

	void ClearAll();

private:

	int GetElementYPosition(int i);

	float draw_scale;

	//scroll parameters
	float scroll_value, scroll_velocity;

	//window size
	int w_size_x, w_size_y;

	//menu position
	int menu_x, menu_y;

	bool inside_edit;
	//y size of the menu
	int menu_size;
	int button_id;
	int active;
	int last_active;
	std::vector<int> lvl_id;
	std::vector<ElementType> types;
	std::vector<std::string> texts;
	std::vector<std::string> description;
	std::vector<std::string> bsttime;

	sf::Text text;
	sf::Font* font;
	sf::Font* font_mono;

	sf::Sound sound_hover;
	sf::SoundBuffer buff_hover;
	sf::Sound sound_click;
	sf::SoundBuffer buff_click;
	sf::Sound sound_count;
	sf::SoundBuffer buff_count;
	sf::Sound sound_go;
	sf::SoundBuffer buff_go;

	sf::Texture edit_tex;
	sf::Sprite edit_spr;

	sf::RectangleShape rectangle;
};

class Overlays {
public:
  enum Texts {
    TITLE,
    PLAY,
    LEVELS,
    CONTROLS,
    SCREEN_SAVER,
    EXIT,
    CREDITS,
    PAUSED,
    CONTINUE,
    RESTART,
    QUIT,
    MUSIC,
    MOUSE,
    CONTROLS_L,
    CONTROLS_R,
    BACK,
    L0, L1, L2, L3, L4,
    L5, L6, L7, L8, L9,
    L10, L11, L12, L13, L14,
    NEXT,
    PREV,
    BACK2,
    NUM_TEXTS
  };
  static const int LEVELS_PER_PAGE = 15;
  bool TWBAR_ENABLED;
  TwBar *stats, *settings, *fractal_editor, *level_editor, *confirmation_box;
  Menu level_menu;

  Overlays(sf::Font* _font, sf::Font* _font_mono, Scene* scene);

  void ReloadLevelMenu(Scene * scene);

  //Relative to 1280x720
  void SetScale(float scale) { draw_scale = scale;  level_menu.SetScale(scale); }
  void SetAntTweakBar(int Width, int Height, float &fps, Scene *scene, bool *vsync, float *mouse_sensitivity, float *wheel_sensitivity, float *music_vol, float *target_fps);
  void SetTWBARResolution(int Width, int Height);

  Texts GetOption(Texts from, Texts to);
  int& GetLevelPage() { return level_page; }

  void UpdateMenu(float mouse_x, float mouse_y);
  void UpdateControls(float mouse_x, float mouse_y);
  void UpdateLevels(float mouse_x, float mouse_y);
  void UpdateLevelMenu(float mouse_x, float mouse_y, float scroll);
  void UpdatePaused(float mouse_x, float mouse_y);

  void DrawMenu(sf::RenderWindow& window);
  void DrawControls(sf::RenderWindow& window);
  void DrawTimer(sf::RenderWindow& window, int t, bool is_high_score);
  void DrawLevelDesc(sf::RenderWindow& window, std::string desc);
  void DrawFPS(sf::RenderWindow& window, int fps);
  void DrawPaused(sf::RenderWindow& window);
  void DrawArrow(sf::RenderWindow& window, const sf::Vector3f& v3);
  void DrawCredits(sf::RenderWindow& window, bool fullrun, int t);
  void DrawMidPoint(sf::RenderWindow& window, bool fullrun, int t);
  void DrawLevels(sf::RenderWindow& window);
  void DrawSumTime(sf::RenderWindow& window, int t);
  void DrawCheatsEnabled(sf::RenderWindow& window);
  void DrawCheats(sf::RenderWindow& window);
  void DrawAntTweakBar();

  bool TwManageEvent(sf::Event &event);
protected:
  void MakeText(const char* str, float x, float y, float size, const sf::Color& color, sf::Text& text, bool mono=false);
  void MakeTime(int t, float x, float y, float size, const sf::Color& color, sf::Text& text);
  void UpdateHover(Texts from, Texts to, float mouse_x, float mouse_y);

private:
  sf::Text all_text[NUM_TEXTS];
  bool all_hover[NUM_TEXTS];

  sf::Sound sound_hover;
  sf::SoundBuffer buff_hover;
  sf::Sound sound_click;
  sf::SoundBuffer buff_click;
  sf::Sound sound_count;
  sf::SoundBuffer buff_count;
  sf::Sound sound_go;
  sf::SoundBuffer buff_go;

  sf::Texture arrow_tex;
  sf::Sprite arrow_spr;

  float draw_scale;
  bool top_level;
  int level_page;

  const sf::Font* font;
  const sf::Font* font_mono;
};
