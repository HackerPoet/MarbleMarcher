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
#include "Overlays.h"
#include "Level.h"
#include "Res.h"
#include "Scores.h"

static const float pi = 3.14159265359f;
int mouse_setting = 0;

Overlays::Overlays(const sf::Font* _font, const sf::Font* _font_mono) :
  font(_font),
  font_mono(_font_mono),
  draw_scale(1.0f),
  top_level(true) {
  memset(all_hover, 0, sizeof(all_hover));
  buff_hover.loadFromFile(menu_hover_wav);
  sound_hover.setBuffer(buff_hover);
  buff_click.loadFromFile(menu_click_wav);
  sound_click.setBuffer(buff_click);
  buff_count.loadFromFile(count_down_wav);
  sound_count.setBuffer(buff_count);
  buff_go.loadFromFile(count_go_wav);
  sound_go.setBuffer(buff_go);
  arrow_tex.loadFromFile(arrow_png);
  arrow_tex.setSmooth(true);
  arrow_spr.setTexture(arrow_tex);
  arrow_spr.setOrigin(arrow_spr.getLocalBounds().width / 2, arrow_spr.getLocalBounds().height / 2);
}

Overlays::Texts Overlays::GetOption(Texts from, Texts to) {
  for (int i = from; i <= to; ++i) {
    if (all_hover[i]) {
      sound_click.play();
      return Texts(i);
    }
  }
  return Texts::TITLE;
}

void Overlays::UpdateMenu(float mouse_x, float mouse_y) {
  //Update text boxes
  MakeText("Marble\nMarcher", 60, 20, 72, sf::Color::White, all_text[TITLE]);
  MakeText("Play", 80, 230, 60, sf::Color::White, all_text[PLAY]);
  MakeText("Levels", 80, 300, 60, sf::Color::White, all_text[LEVELS]);
  MakeText("Controls", 80, 370, 60, sf::Color::White, all_text[CONTROLS]);
  MakeText("Screen Saver", 80, 440, 60, sf::Color::White, all_text[SCREEN_SAVER]);
  MakeText("Exit", 80, 510, 60, sf::Color::White, all_text[EXIT]);
  MakeText("\xA9""2019 CodeParade\nMusic by PettyTheft", 16, 652, 32, sf::Color::White, all_text[CREDITS], true);
  all_text[TITLE].setLineSpacing(0.76f);
  all_text[CREDITS].setLineSpacing(0.9f);

  //Check if mouse intersects anything
  UpdateHover(PLAY, EXIT, mouse_x, mouse_y);
}

void Overlays::UpdateControls(float mouse_x, float mouse_y) {
  //Update text boxes
  MakeText("Roll\nCamera\nZoom\nRestart\nPause", 40, 200, 46, sf::Color::White, all_text[CONTROLS_L]);
  MakeText("WASD or Arrows\nMouse\nScroll Wheel\nR or Right-Click\nEsc", 280, 200, 46, sf::Color::White, all_text[CONTROLS_R]);
  MakeText("Back", 60, 550, 40, sf::Color::White, all_text[BACK]);

  //A little extra vertical spacing
  all_text[CONTROLS_L].setLineSpacing(1.1f);
  all_text[CONTROLS_R].setLineSpacing(1.1f);

  //Check if mouse intersects anything
  UpdateHover(BACK, BACK, mouse_x, mouse_y);
}

void Overlays::UpdateLevels(float mouse_x, float mouse_y) {
  //Update text boxes
  for (int i = 0; i < num_levels; ++i) {
    const float y = 80.0f + float(i/3) * 120.0f;
    const float x = 240.0f + float(i%3) * 400.0f;
    const char* txt = high_scores.Has(i) ? all_levels[i].txt : "???";
    MakeText(txt, x, y, 32, sf::Color::White, all_text[i + L0]);
    const sf::FloatRect text_bounds = all_text[i + L0].getLocalBounds();
    all_text[i + L0].setOrigin(text_bounds.width / 2, text_bounds.height / 2);
  }
  MakeText("Back", 590, 660, 40, sf::Color::White, all_text[BACK2]);

  //Check if mouse intersects anything
  UpdateHover(L0, BACK2, mouse_x, mouse_y);
}

void Overlays::UpdatePaused(float mouse_x, float mouse_y) {
  //Update text boxes
  MakeText("Paused", 540, 288, 54, sf::Color::White, all_text[PAUSED]);
  MakeText("Continue", 370, 356, 40, sf::Color::White, all_text[CONTINUE]);
  MakeText("Restart", 620, 356, 40, sf::Color::White, all_text[RESTART]);
  MakeText("Quit", 845, 356, 40, sf::Color::White, all_text[QUIT]);

  //Update mouse sensitivity setting
  const char* mouse_txt = "Mouse Sensitivity:  High";
  if (mouse_setting == 1) {
    mouse_txt = "Mouse Sensitivity:  Medium";
  } else if (mouse_setting == 2) {
    mouse_txt = "Mouse Sensitivity:  Low";
  }
  MakeText(mouse_txt, 410, 500, 40, sf::Color::White, all_text[MOUSE]);

  //Check if mouse intersects anything
  UpdateHover(CONTINUE, MOUSE, mouse_x, mouse_y);
}

void Overlays::DrawMenu(sf::RenderWindow& window) {
  for (int i = TITLE; i <= CREDITS; ++i) {
    window.draw(all_text[i]);
  }
}

void Overlays::DrawControls(sf::RenderWindow& window) {
  for (int i = CONTROLS_L; i <= BACK; ++i) {
    window.draw(all_text[i]);
  }
}

void Overlays::DrawTimer(sf::RenderWindow& window, int t, bool is_high_score) {
  sf::Text text;
  if (t < 0) {
    return;
  } else if (t < 3*60) {
    //Create text for the number
    char txt[] = "0";
    txt[0] = '3' - (t / 60);
    MakeText(txt, 640, 50, 140, sf::Color::White, text);

    //Play count sound if needed
    if (t % 60 == 0) {
      sound_count.play();
    }
  } else if (t < 4*60) {
    MakeText("GO!", 640, 50, 140, sf::Color::White, text);

    //Play go sound if needed
    if (t == 3*60) {
      sound_go.play();
    }
  } else {
    //Create timer text
    const int score = t - 3 * 60;
    const sf::Color col = (is_high_score ? sf::Color::Green : sf::Color::White);
    MakeTime(score, 530, 10, 60, col, text);
  }

  if (t < 4*60) {
    //Apply zoom animation
    const float fpart = float(t % 60) / 60.0f;
    const float zoom = 0.8f + fpart*0.2f;
    const sf::Uint8 alpha = sf::Uint8(255.0f*(1.0f - fpart*fpart));
    text.setScale(sf::Vector2f(zoom, zoom));
    text.setFillColor(sf::Color(255, 255, 255, alpha));
    text.setOutlineColor(sf::Color(0, 0, 0, alpha));

    //Center text
    const sf::FloatRect text_bounds = text.getLocalBounds();
    text.setOrigin(text_bounds.width / 2, text_bounds.height / 2);
  }

  //Draw the text
  window.draw(text);
}

void Overlays::DrawLevelDesc(sf::RenderWindow& window, int level) {
  sf::Text text;
  MakeText(all_levels[level].txt, 640, 60, 48, sf::Color::White, text);
  const sf::FloatRect text_bounds = text.getLocalBounds();
  text.setOrigin(text_bounds.width / 2, text_bounds.height / 2);
  window.draw(text);
}

void Overlays::DrawFPS(sf::RenderWindow& window, int fps) {
  sf::Text text;
  std::string fps_str = std::to_string(fps) + "fps";
  const sf::Color col = (fps < 50 ? sf::Color::Red : sf::Color::White);
  MakeText(fps_str.c_str(), 1280, 720, 24, col, text, false);
  const sf::FloatRect text_bounds = text.getLocalBounds();
  text.setOrigin(text_bounds.width, text_bounds.height);
  window.draw(text);
}

void Overlays::DrawPaused(sf::RenderWindow& window) {
  for (int i = PAUSED; i <= MOUSE; ++i) {
    window.draw(all_text[i]);
  }
}

void Overlays::DrawArrow(sf::RenderWindow& window, const sf::Vector3f& v3) {
  const float x_scale = 250.0f * v3.y + 520.0f * (1.0f - v3.y);
  const float x = 640.0f + x_scale * std::cos(v3.x);
  const float y = 360.0f + 250.0f * std::sin(v3.x);
  const sf::Uint8 alpha = sf::Uint8(102.0f * std::max(0.0f, std::min(1.0f, (v3.z - 5.0f) / 30.0f)));
  if (alpha > 0) {
    arrow_spr.setScale(draw_scale * 0.1f, draw_scale * 0.1f);
    arrow_spr.setRotation(90.0f + v3.x * 180.0f / pi);
    arrow_spr.setPosition(draw_scale * x, draw_scale * y);
    arrow_spr.setColor(sf::Color(255, 255, 255, alpha));
    window.draw(arrow_spr);
  }
}

void Overlays::DrawCredits(sf::RenderWindow& window) {
  const char* txt =
    "Congratulations, you beat all the levels!\n\n"
    "I hope it was as fun to play this demo as\n"
    "it was to make it. For more information about\n"
    "this game and other projects, check out my\n"
    "YouTube channel \"CodeParade\".\n\n"
    "Thanks for playing!";
  sf::Text text;
  MakeText(txt, 50, 100, 44, sf::Color::White, text);
  text.setLineSpacing(1.3f);
  window.draw(text);
}

void Overlays::DrawLevels(sf::RenderWindow& window) {
  //Draw the level names
  for (int i = L0; i <= BACK2; ++i) {
    window.draw(all_text[i]);
  }
  //Draw the times
  for (int i = 0; i < num_levels; ++i) {
    if (high_scores.Has(i)) {
      sf::Text text;
      const float y = 98.0f + float(i / 3) * 120.0f;
      const float x = 148.0f + float(i % 3) * 400.0f;
      MakeTime(high_scores.Get(i), x, y, 48, sf::Color(64, 255, 64), text);
      window.draw(text);
    }
  }
}

void Overlays::MakeText(const char* str, float x, float y, float size, const sf::Color& color, sf::Text& text, bool mono) {
  text.setString(str);
  text.setFont(mono ? *font_mono : *font);
  text.setCharacterSize(int(size * draw_scale));
  text.setLetterSpacing(0.8f);
  text.setPosition((x - 2.0f) * draw_scale, (y - 2.0f) * draw_scale);
  text.setFillColor(color);
  text.setOutlineThickness(3.0f * draw_scale);
  text.setOutlineColor(sf::Color::Black);
}

void Overlays::MakeTime(int t, float x, float y, float size, const sf::Color& color, sf::Text& text) {
  //Create timer text
  char txt[] = "00:00:00";
  const int t_all = std::min(t, 59 * (60 * 60 + 60 + 1));
  const int t_ms = t_all % 60;
  const int t_sec = (t_all / 60) % 60;
  const int t_min = t_all / (60 * 60);
  txt[0] = '0' + t_min / 10; txt[1] = '0' + t_min % 10;
  txt[3] = '0' + t_sec / 10; txt[4] = '0' + t_sec % 10;
  txt[6] = '0' + t_ms / 10;  txt[7] = '0' + t_ms % 10;
  MakeText(txt, x, y, size, color, text, true);
}

void Overlays::UpdateHover(Texts from, Texts to, float mouse_x, float mouse_y) {
  for (int i = from; i <= to; ++i) {
    const sf::FloatRect bounds = all_text[i].getGlobalBounds();
    if (bounds.contains(mouse_x, mouse_y)) {
      all_text[i].setFillColor(sf::Color(255, 64, 64));
      if (!all_hover[i]) {
        sound_hover.play();
        all_hover[i] = true;
      }
    } else {
      all_hover[i] = false;
    }
  }
}
