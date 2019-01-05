/* This file is part of the Marble Marcher (https://github.com/HackerPoet/MarbleMarcher).
* Copyright(C) 2018 CodeParade
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
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
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

struct Resolution {
  Resolution(int w, int h, const char* i) : width(w), height(h), info(i) {}
  int width;
  int height;
  const char* info;
};
static const int num_resolutions = 7;
const extern Resolution all_resolutions[num_resolutions];

class SelectRes {
public:
  SelectRes(const sf::Font* _font);

  bool FullScreen() const { return is_fullscreen; }

  int Select(const sf::Vector2i& mouse_pos);
  void Draw(sf::RenderWindow& window, const sf::Vector2i& mouse_pos);
  sf::Text MakeText(const char* str, float x, float y, int size, bool selected=true, bool centered=true) const;
  const Resolution* Run();

private:
  const sf::Font* font;

  bool is_fullscreen;

  sf::Sound sound_hover;
  sf::SoundBuffer buff_hover;
};
