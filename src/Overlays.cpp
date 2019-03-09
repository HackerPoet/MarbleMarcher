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
static const int num_level_pages = 1 + (num_levels - 1) / Overlays::LEVELS_PER_PAGE;
Settings game_settings;

Overlays::Overlays(const sf::Font* _font, const sf::Font* _font_mono) :
  font(_font),
  font_mono(_font_mono),
  draw_scale(1.0f),
  level_page(0),
  top_level(true),
  TWBAR_ENABLED(false)
{
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
  MakeText("\xA9""2019 CodeParade 1.1.1\nMusic by PettyTheft", 16, 652, 32, sf::Color::White, all_text[CREDITS], true);
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
  const int page_start = level_page * LEVELS_PER_PAGE;
  const int page_end = page_start + LEVELS_PER_PAGE;
  for (int i = page_start; i < page_end; ++i) {
    const int j = i % LEVELS_PER_PAGE;
    if (i < num_levels) {
      const float y = 80.0f + float(j / 3) * 120.0f;
      const float x = 240.0f + float(j % 3) * 400.0f;
      const char* txt = high_scores.HasUnlocked(i) ? all_levels[i].txt : "???";
      MakeText(txt, x, y, 32, sf::Color::White, all_text[j + L0]);
      const sf::FloatRect text_bounds = all_text[j + L0].getLocalBounds();
      all_text[j + L0].setOrigin(text_bounds.width / 2, text_bounds.height / 2);
    } else {
      all_text[j + L0] = sf::Text();
    }
  }
  if (level_page > 0) {
    MakeText("<", 540, 652, 48, sf::Color::White, all_text[PREV]);
  } else {
    all_text[PREV] = sf::Text();
  }
  MakeText("Back", 590, 660, 40, sf::Color::White, all_text[BACK2]);
  if (level_page < num_level_pages - 1) {
    MakeText(">", 732, 652, 48, sf::Color::White, all_text[NEXT]);
  } else {
    all_text[NEXT] = sf::Text();
  }

  //Check if mouse intersects anything
  UpdateHover(L0, BACK2, mouse_x, mouse_y);
}

void Overlays::UpdatePaused(float mouse_x, float mouse_y) {
  //Update text boxes
  MakeText("Paused", 540, 288, 54, sf::Color::White, all_text[PAUSED]);
  MakeText("Continue", 370, 356, 40, sf::Color::White, all_text[CONTINUE]);
  MakeText("Restart", 620, 356, 40, sf::Color::White, all_text[RESTART]);
  MakeText("Quit", 845, 356, 40, sf::Color::White, all_text[QUIT]);

  //Update music setting
  const char* music_txt = (game_settings.mute ? "Music:  Off" : "Music:  On");
  MakeText(music_txt, 410, 500, 40, sf::Color::White, all_text[MUSIC]);

  //Update mouse sensitivity setting
  const char* mouse_txt = "Mouse Sensitivity:  High";
  if (game_settings.mouse_sensitivity == 1) {
    mouse_txt = "Mouse Sensitivity:  Medium";
  } else if (game_settings.mouse_sensitivity == 2) {
    mouse_txt = "Mouse Sensitivity:  Low";
  }
  MakeText(mouse_txt, 410, 550, 40, sf::Color::White, all_text[MOUSE]);

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

void Overlays::DrawCredits(sf::RenderWindow& window, bool fullrun, int t) {
  const char* txt =
    "  Congratulations, you beat all the levels!\n\n\n\n"
    "As a reward, cheats have been unlocked!\n"
    "Activate them with the F1 key during gameplay.\n\n"
    "Thanks for playing!";
  sf::Text text;
  MakeText(txt, 100, 100, 44, sf::Color::White, text);
  text.setLineSpacing(1.3f);
  window.draw(text);

  if (fullrun) {
    sf::Text time_txt;
    MakeTime(t, 640, 226, 72, sf::Color::White, time_txt);
    const sf::FloatRect text_bounds = time_txt.getLocalBounds();
    time_txt.setOrigin(text_bounds.width / 2, text_bounds.height / 2);
    window.draw(time_txt);
  }
}

void Overlays::DrawMidPoint(sf::RenderWindow& window, bool fullrun, int t) {
  const char* txt =
    "            You've done well so far.\n\n\n\n"
    "      But this is only the beginning.\n"
    "If you need a quick break, take it now.\n"
    "The challenge levels are coming up...";
  sf::Text text;
  MakeText(txt, 205, 100, 44, sf::Color::White, text);
  text.setLineSpacing(1.3f);
  window.draw(text);

  if (fullrun) {
    sf::Text time_txt;
    MakeTime(t, 640, 226, 72, sf::Color::White, time_txt);
    const sf::FloatRect text_bounds = time_txt.getLocalBounds();
    time_txt.setOrigin(text_bounds.width / 2, text_bounds.height / 2);
    window.draw(time_txt);
  }
}

void Overlays::DrawLevels(sf::RenderWindow& window) {
  //Draw the level names
  for (int i = L0; i <= BACK2; ++i) {
    window.draw(all_text[i]);
  }
  //Draw the times
  const int page_start = level_page * LEVELS_PER_PAGE;
  const int page_end = page_start + LEVELS_PER_PAGE;
  for (int i = page_start; i < page_end; ++i) {
    if (i < num_levels && high_scores.HasCompleted(i)) {
      sf::Text text;
      const int j = i % LEVELS_PER_PAGE;
      const float y = 98.0f + float(j / 3) * 120.0f;
      const float x = 148.0f + float(j % 3) * 400.0f;
      MakeTime(high_scores.Get(i), x, y, 48, sf::Color(64, 255, 64), text);
      window.draw(text);
    }
  }
}

void Overlays::DrawSumTime(sf::RenderWindow& window, int t) {
  sf::Text text;
  MakeTime(t, 10, 680, 32, sf::Color::White, text);
  window.draw(text);
}

void Overlays::DrawCheatsEnabled(sf::RenderWindow& window) {
  sf::Text text;
  MakeText("Cheats Enabled", 10, 680, 32, sf::Color::White, text);
  window.draw(text);
}

void Overlays::DrawCheats(sf::RenderWindow& window) {
  sf::Text text;
  const char* txt =
    "[ C ] Color change\n"
    "[ F ] Free camera\n"
    "[ G ] Gravity strength\n"
    "[ H ] Hyperspeed toggle\n"
    "[ I ] Ignore goal\n"
    "[ M ] Motion disable\n"
    "[ P ] Planet toggle\n"
    "[ Z ] Zoom to scale\n"
    "[1-9] Scroll fractal parameter\n";
  MakeText(txt, 460, 160, 32, sf::Color::White, text, true);
  window.draw(text);
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


void Overlays::SetAntTweakBar(int Width, int Height, float &fps, FractalParams *params)
{
	//TW interface
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(Width, Height);

	stats = TwNewBar("Statistics" );
	TwDefine(" GLOBAL help='Marble Marcher mod by Michael Moroz' ");

	// Change bar position
	int barPos[2] = { 16, 60 };
	TwSetParam(stats, NULL, "position", TW_PARAM_INT32, 2, &barPos);
	TwAddVarRO(stats, "FPS", TW_TYPE_FLOAT, &fps, " label='FPS' ");
	//TwAddVarRW(bar, "Camera speed", TW_TYPE_FLOAT, &sp, " min=0.005 max=0.5 step=0.005");
	//TwAddVarRW(bar, "Calculations per frame", TW_TYPE_INT32, &cpf, " min=1 max=500 step=1");
	settings = TwNewBar("Settings");
	float *p = params->data();
	TwAddVarRW(settings, "FractalScale", TW_TYPE_FLOAT, p, "min=0 max=5 step=0.01  group='Fractal parameter");
	TwAddVarRW(settings, "FractalAngle1", TW_TYPE_FLOAT, p+1, "min=-5 max=5 step=0.01  group='Fractal parameter");
	TwAddVarRW(settings, "FractalAngle2", TW_TYPE_FLOAT, p+2, "min=-5 max=5 step=0.01  group='Fractal parameter");
	TwAddVarRW(settings, "FractalShift", TW_TYPE_DIR3F, p+3, " group='Fractal parameter");
	TwAddVarRW(settings, "FractalColor", TW_TYPE_DIR3F, p+6, " group='Fractal parameter");

	int barPos1[2] = { 16, 450 };

	TwSetParam(settings, NULL, "position", TW_PARAM_INT32, 2, &barPos1);

	TwDefine(" GLOBAL fontsize=3 ");
	TwDefine("Settings color='255 128 0' alpha=210");
	TwDefine("Statistics color='0 128 255' alpha=210");
}

void Overlays::DrawAntTweakBar()
{
	//Refresh tweak bar
	if (TWBAR_ENABLED)
	{
		TwRefreshBar(stats);
		TwRefreshBar(settings);
		TwDraw();
	}
}

bool Overlays::TwManageEvent(sf::Event &event)
{
	if (TWBAR_ENABLED)
	{
		bool released = event.type == sf::Event::MouseButtonReleased;
		bool moved = event.type == sf::Event::MouseMoved;
		bool LMB = event.mouseButton.button == sf::Mouse::Left;
		bool RMB = event.mouseButton.button == sf::Mouse::Right;
		bool MMB = event.mouseButton.button == sf::Mouse::Middle;

		bool handl = 0;

		if (moved)
		{
			sf::Vector2i mouse = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
			handl = handl || TwMouseMotion(mouse.x, mouse.y);
		}

		if (LMB && !released)
		{
			handl = handl || TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
		}
		if (LMB && released)
		{
			handl = handl || TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
		}

		if (MMB && !released)
		{
			handl = handl || TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_MIDDLE);
		}
		if (MMB && released)
		{
			handl = handl || TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_MIDDLE);
		}

		if (RMB && !released)
		{
			handl = handl || TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_RIGHT);
		}
		if (RMB && released)
		{
			handl = handl || TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_RIGHT);
		}

		if (RMB && released)
		{
			handl = handl || TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_RIGHT);
		}

		bool keypress = event.type == sf::Event::KeyPressed;
		bool keyrelease = event.type == sf::Event::MouseButtonReleased;
		int keycode = event.key.code;

		if (keypress)
		{
			handl = handl || TwKeyPressed(TW_KEY_F1 + keycode - sf::Keyboard::F1, TW_KMOD_NONE);
		}

		return handl;
	}
	else
	{
		return 0;
	}
}


void Overlays::SetTWBARResolution(int Width, int Height)
{
	TwWindowSize(Width, Height);
}