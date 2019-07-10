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

extern GameMode game_mode = MAIN_MENU;

void EnableEditor(int level);

void OpenTestWindow(sf::Font & font);


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
