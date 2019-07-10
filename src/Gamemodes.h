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

extern sf::Music menu_music;
extern sf::Music level_music[num_level_music];
extern sf::Music credits_music;

sf::Sound sound_hover;
sf::SoundBuffer buff_hover;
sf::Sound sound_click;
sf::SoundBuffer buff_click;
sf::Sound sound_count;
sf::SoundBuffer buff_count;
sf::Sound sound_go;
sf::SoundBuffer buff_go;

//Global variables
extern sf::Vector2i mouse_pos, mouse_prev_pos;
extern bool all_keys[sf::Keyboard::KeyCount] = { 0 };
extern bool mouse_clicked = false;
extern bool show_cheats = false;
extern InputState io_state;

//Constants
extern float mouse_sensitivity = 0.005f;
extern float wheel_sensitivity = 0.2f;
extern float music_vol = 75.0f;
extern float target_fps = 60.0f;

extern GameMode game_mode = MAIN_MENU;

void OpenMainMenu(Scene * scene, Overlays * overlays);

void OpenEditor(Scene * scene, Overlays * overlays, int level);
void PlayLevel(Scene * scene, sf::RenderWindow * window, int level);

void OpenTestWindow(sf::Font & font);
void OpenLevelMenu(Scene* scene, Overlays* overlays);
void ConfirmLevelDeletion(int lvl, Scene* scene);


float GetVol() {
	if (game_settings.mute) {
		return 0.0f;
	}
	else if (game_mode == PAUSED) {
		return music_vol / 4;
	}
	else {
		return music_vol;
	}
}

void LockMouse(sf::RenderWindow& window) {
	window.setMouseCursorVisible(false);
	const sf::Vector2u size = window.getSize();
	mouse_pos = sf::Vector2i(size.x / 2, size.y / 2);
	sf::Mouse::setPosition(mouse_pos);
}
void UnlockMouse(sf::RenderWindow& window) {
	window.setMouseCursorVisible(true);
}

void PauseGame(sf::RenderWindow& window, Scene& scene) {
	game_mode = PAUSED;
	scene.GetCurMusic().setVolume(GetVol());
	UnlockMouse(window);
	scene.SetExposure(0.5f);
}

int DirExists(const char *path) {
	struct stat info;
	if (stat(path, &info) != 0) {
		return 0;
	}
	else if (info.st_mode & S_IFDIR) {
		return 1;
	}
	return 0;
}
