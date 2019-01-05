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
#include "SelectRes.h"
#include "Res.h"

const Resolution all_resolutions[num_resolutions] = {
  Resolution(320, 180, "I have potato?"),
  Resolution(640, 360, "GTX 960 or lower:"),
  Resolution(960, 540, "GTX 970 or similar:"),
  Resolution(1280, 720, "GTX 980 or similar:"),
  Resolution(1600, 900, "GTX 1080 or similar:"),
  Resolution(1920, 1080, "GTX 1080 Ti or similar:"),
  Resolution(2560, 1440, "RTX 2080 Ti or higher:")
};

SelectRes::SelectRes(const sf::Font* _font) : font(_font), is_fullscreen(false) {
  buff_hover.loadFromFile(menu_hover_wav);
  sound_hover.setBuffer(buff_hover);
}

int SelectRes::Select(const sf::Vector2i& mouse_pos) {
  const int select_ix = (mouse_pos.y + 25) / 60 - 2;
  const int select_bounds = (mouse_pos.y + 25) % 60;
  if (select_ix < 0 || select_ix > num_resolutions) {
    return -1;
  }
  if (select_bounds > 42) {
    return -1;
  }
  return select_ix;
}

void SelectRes::Draw(sf::RenderWindow& window, const sf::Vector2i& mouse_pos) {
  const int sel_ix = Select(mouse_pos);
  window.draw(MakeText("Select Resolution", 320.0f, 26.0f, 48));
  for (int i = 0; i < num_resolutions; ++i) {
    const Resolution& res = all_resolutions[i];
    const float y = 90.0f + float(i)*60.0f;
    const bool is_sel = (i == sel_ix);
    window.draw(MakeText(res.info, 20.0f, y + 5.0f, 32, is_sel, false));
    const std::string res_str = std::to_string(res.width) + " x " + std::to_string(res.height);
    window.draw(MakeText(res_str.c_str(), 390.0f, y, 42, is_sel, false));
  }
  const char* ftxt = (is_fullscreen ? "Full Screen [X]" : "Full Screen [ ]");
  window.draw(MakeText(ftxt, 320.0f, 530.0f, 40, sel_ix == num_resolutions));
}

sf::Text SelectRes::MakeText(const char* str, float x, float y, int size, bool selected, bool centered) const {
  sf::Text text;
  text.setString(str);
  text.setFont(*font);
  text.setCharacterSize(size);
  text.setLetterSpacing(0.8f);
  text.setPosition(x, y);
  text.setFillColor(selected ? sf::Color::White : sf::Color(128,128,128));
  if (centered) {
    const sf::FloatRect text_bounds = text.getLocalBounds();
    text.setOrigin(text_bounds.width / 2, text_bounds.height / 2);
  }
  return text;
}

const Resolution* SelectRes::Run() {
  //Create the window
  sf::VideoMode window_size(640, 600, 24);
  sf::RenderWindow window(window_size, "Marble Marcher", sf::Style::Close);
  window.setVerticalSyncEnabled(true);
  window.requestFocus();

  sf::Vector2i mouse_pos;
  const Resolution* res = nullptr;
  int prev_ix = -1;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        break;
      } else if (event.type == sf::Event::KeyPressed) {
        const sf::Keyboard::Key keycode = event.key.code;
        if (event.key.code < 0 || event.key.code >= sf::Keyboard::KeyCount) { continue; }
        if (keycode == sf::Keyboard::Escape) {
          window.close();
          break;
        }
      } else if (event.type == sf::Event::MouseButtonPressed) {
        mouse_pos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
        const int sel_ix = Select(mouse_pos);
        if (sel_ix >= 0 && sel_ix < num_resolutions) {
          res = &all_resolutions[sel_ix];
          window.close();
        } else if (sel_ix == num_resolutions) {
          is_fullscreen = !is_fullscreen;
        }
      } else if (event.type == sf::Event::MouseButtonReleased) {
        mouse_pos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
      } else if (event.type == sf::Event::MouseMoved) {
        mouse_pos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
        int hover_ix = Select(mouse_pos);
        if (hover_ix >= 0 && hover_ix != prev_ix) {
          sound_hover.play();
        }
        prev_ix = hover_ix;
      }
    }

    //Finally display to the screen
    window.clear();
    Draw(window, mouse_pos);
    window.display();
  }

  return res;
}
