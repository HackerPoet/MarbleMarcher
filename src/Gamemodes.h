#pragma once

#include<Localization.h>
#include<Interface.h>
#include<Scene.h>
#include<Overlays.h>
#include<Settings.h>
#include <AntTweakBar.h>

enum GameMode {
	MAIN_MENU,
	PLAYING,
	PAUSED,
	SCREEN_SAVER,
	CONTROLS,
	LEVELS,
	LEVEL_EDITOR,
	CREDITS,
	MIDPOINT
};

//Global variables
extern sf::Vector2i mouse_pos, mouse_prev_pos;
extern bool all_keys[sf::Keyboard::KeyCount];
extern bool mouse_clicked;
extern bool show_cheats;
extern InputState io_state;

//Constants
extern float mouse_sensitivity;
extern float wheel_sensitivity;
extern float music_vol;
extern float target_fps;

extern GameMode game_mode;

void OpenMainMenu(Scene * scene, Overlays * overlays);

void OpenEditor(Scene * scene, Overlays * overlays, int level);
void PlayLevel(Scene * scene, sf::RenderWindow * window, int level);

void OpenControlMenu(Scene * scene, Overlays * overlays);

void OpenScreenSaver(Scene * scene, Overlays * overlays);

void PlayNewGame(Scene * scene, sf::RenderWindow * window, int level);

void OpenTestWindow(sf::Font & font);
void OpenLevelMenu(Scene* scene, Overlays* overlays);
void ConfirmLevelDeletion(int lvl, Scene* scene, Overlays* overlays);

void ConfirmEditorExit(Scene * scene, Overlays * overlays);

float GetVol();
void LockMouse(sf::RenderWindow& window);
void UnlockMouse(sf::RenderWindow& window);
void PauseGame(sf::RenderWindow& window, Scene& scene);
int DirExists(const char *path);

template < typename T > std::string num2str(const T& n)
{
	std::ostringstream stm;
	if (n < 10) stm << "0";
	stm << n;
	return stm.str();
}


class Sounds
{

};