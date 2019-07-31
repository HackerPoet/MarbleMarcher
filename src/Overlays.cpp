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
#include "Res.h"
#include <Localization.h>

static const float PI = 3.14159265359f;
static const int num_level_pages = 1 + (num_levels - 1) / Overlays::LEVELS_PER_PAGE;
Settings game_settings;

Overlays::Overlays(sf::Font* _font, sf::Font* _font_mono, Scene* scene) :
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

  ReloadLevelMenu(scene);
}

void Overlays::ReloadLevelMenu(Scene* scene)
{
	
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
  //MakeText(LOCAL["Marble_Marcher"], 60, 20, 72, sf::Color::White, all_text[TITLE]);
 /* MakeText(LOCAL["Play"], 80, 230, 60, sf::Color::White, all_text[PLAY]);
  MakeText(LOCAL["Levels"], 80, 300, 60, sf::Color::White, all_text[LEVELS]);
  MakeText(LOCAL["Controls"], 80, 370, 60, sf::Color::White, all_text[CONTROLS]);
  MakeText(LOCAL["Screen_Saver"], 80, 440, 60, sf::Color::White, all_text[SCREEN_SAVER]);
  MakeText(LOCAL["Exit"], 80, 510, 60, sf::Color::White, all_text[EXIT]);
  */
  //MakeText(LOCAL["About"], 16, 652, 32, sf::Color::White, all_text[CREDITS], true);
 // all_text[TITLE].setLineSpacing(0.76f);
  //all_text[CREDITS].setLineSpacing(0.9f);

  //Check if mouse intersects anything
 // UpdateHover(PLAY, EXIT, mouse_x, mouse_y);
}

void Overlays::UpdateControls(float mouse_x, float mouse_y) {
  //Update text boxes
  MakeText(LOCAL["DetailControls"], 40, 200, 46, sf::Color::White, all_text[CONTROLS_L]);
  MakeText(LOCAL["Back"], 60, 550, 40, sf::Color::White, all_text[BACK]);

  //A little extra vertical spacing
  all_text[CONTROLS_L].setLineSpacing(1.1f);
  all_text[CONTROLS_R].setLineSpacing(1.1f);

  //Check if mouse intersects anything
  UpdateHover(BACK, BACK, mouse_x, mouse_y);
}

void Overlays::UpdateLevels(float mouse_x, float mouse_y) {
  
}

void Overlays::UpdateLevelMenu(float mouse_x, float mouse_y, float scroll)
{

}

void Overlays::UpdatePaused(float mouse_x, float mouse_y) {
  //Update text boxes
  MakeText(LOCAL["Paused"], 540, 288, 54, sf::Color::White, all_text[PAUSED]);
  MakeText(LOCAL["Continue"], 370, 356, 40, sf::Color::White, all_text[CONTINUE]);
  MakeText(LOCAL["Restart"], 620, 356, 40, sf::Color::White, all_text[RESTART]);
  MakeText(LOCAL["Quit"], 845, 356, 40, sf::Color::White, all_text[QUIT]);

  //Update music setting
  std::wstring music_txt = LOCAL["Music"] + L": " + std::wstring(game_settings.mute ? LOCAL["On"] : LOCAL["Off"]);
  MakeText(music_txt, 410, 500, 40, sf::Color::White, all_text[MUSIC]);

  //Update mouse sensitivity setting
  std::wstring mouse_txt = LOCAL["Mouse_sensitivity"] + L": ";
  if (game_settings.mouse_sensitivity == 1) 
  {
    mouse_txt += LOCAL["Medium"];
  } 
  else if (game_settings.mouse_sensitivity == 2) 
  {
	mouse_txt += LOCAL["Low"];
  }	
  else
  {
	mouse_txt += LOCAL["High"];
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

void Overlays::DrawLevelDesc(sf::RenderWindow& window, std::string desc) {
  sf::Text text;
  MakeText(desc.c_str(), 640, 60, 48, sf::Color::White, text);
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
    arrow_spr.setRotation(90.0f + v3.x * 180.0f / PI);
    arrow_spr.setPosition(draw_scale * x, draw_scale * y);
    arrow_spr.setColor(sf::Color(255, 255, 255, alpha));
    window.draw(arrow_spr);
  }
}

void Overlays::DrawCredits(sf::RenderWindow& window, bool fullrun, int t) {
  std::wstring txt = LOCAL["CongratsEnd"];
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
  std::wstring txt = LOCAL["CongratsMid"];
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
  
}

void Overlays::DrawSumTime(sf::RenderWindow& window, int t) {
  sf::Text text;
  MakeTime(t, 10, 680, 32, sf::Color::White, text);
  window.draw(text);
}

void Overlays::DrawCheatsEnabled(sf::RenderWindow& window) {
  sf::Text text;
  MakeText(LOCAL["CheatsON"], 10, 680, 32, sf::Color::White, text);
  window.draw(text);
}

void Overlays::DrawCheats(sf::RenderWindow& window) {
  sf::Text text;
  std::wstring txt = LOCAL["CheatsInfo"];
  MakeText(txt, 460, 160, 32, sf::Color::White, text, true);
  window.draw(text);
}


template<class T> void Overlays::MakeText(T str, float x, float y, float size, const sf::Color& color, sf::Text& text, bool mono) {
  text.setString(str);
  text.setFont(mono ? LOCAL("mono"): LOCAL("default"));
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
    sf::FloatRect bounds = all_text[i].getGlobalBounds();
    if (bounds.contains(mouse_x, mouse_y)) {
      all_text[i].setFillColor(sf::Color(255, 64, 64));
      if (!all_hover[i]) {
        //sound_hover.play();
        all_hover[i] = true;
      }
    } else {
      all_hover[i] = false;
    }
  }
}


Scene *scene_ptr;

extern bool confirmed = false;
extern bool canceled = false;
int music_id = 0;
bool music_play = false;


void TW_CALL Confirm(void *data)
{
	confirmed = true;
}

void TW_CALL Cancel(void *data)
{
	canceled = true;
}

void TW_CALL MarbleSet(void *data)
{
	scene_ptr->cur_ed_mode = Scene::EditorMode::PLACE_MARBLE;
}

void TW_CALL FlagSet(void *data)
{
	scene_ptr->cur_ed_mode = Scene::EditorMode::PLACE_FLAG;
}

void TW_CALL PlayMusic(void *data)
{
	scene_ptr->levels.StopAllMusic();
	music_play = !music_play;
	if (music_play)
	{
		scene_ptr->levels.GetMusicByID(music_id)->play();
	}
}

void TW_CALL SaveLevel(void *data)
{
	Level* copy = &scene_ptr->level_copy;
	int lvlid = scene_ptr->GetLevel();

	std::vector<std::string> music_list = scene_ptr->levels.GetMusicNames();
	std::vector<int> lvlnum = scene_ptr->levels.getLevelIds();
	copy->use_music = music_list[music_id];
	bool same_level = scene_ptr->original_level_name == copy->txt;
	if (lvlid < 0 || !same_level)
		lvlid = time(NULL);
	copy->level_id = lvlid;
	copy->SaveToFile(std::string(level_folder) + "/" + ConvertSpaces2_(copy->txt) + ".lvl", lvlid, copy->link_level);
	scene_ptr->levels.ReloadLevels();
	if (!(scene_ptr->GetLevel() >= 0 && same_level))
	{
		scene_ptr->WriteLVL(lvlid);
		scene_ptr->original_level_name = copy->txt;
	}
}


void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString)
{
	// Copy the content of souceString handled by the AntTweakBar library to destinationClientString handled by your application
	destinationClientString = sourceLibraryString;
}


void Overlays::SetAntTweakBar(int Width, int Height, float &fps, Scene *scene, bool *vsync, float *mouse_sensitivity, float *wheel_sensitivity, float *music_vol, float *target_fps)
{
	//TW interface
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(Width, Height);
	scene_ptr = scene;

	stats = TwNewBar("Statistics" );
	TwDefine(" GLOBAL help='Marble Marcher Community Edition. Work in progress.' ");

	// Change bar position
	int barPos[2] = { 16, 60 };
	TwSetParam(stats, NULL, "position", TW_PARAM_INT32, 2, &barPos);
	TwAddVarRO(stats, "FPS", TW_TYPE_FLOAT, &fps, " label='FPS' ");
	TwAddVarRO(stats, "Marble velocity", TW_TYPE_DIR3F, scene->marble_vel.data(),  " ");
	TwAddVarRO(stats, "Marble position", TW_TYPE_DIR3F, scene->marble_pos.data(), " ");
	
	settings = TwNewBar("Settings");

	TwAddVarRW(settings, "VSYNC", TW_TYPE_BOOLCPP, vsync, "group='Graphics settings'");
	TwAddVarRW(settings, "PBR", TW_TYPE_BOOLCPP, &scene->PBR_Enabled, "group='Graphics settings'");
	TwAddVarRW(settings, "Sun direction", TW_TYPE_DIR3F, scene->LIGHT_DIRECTION.data(), " group='Graphics settings'");
	TwAddVarRW(settings, "Shadows", TW_TYPE_BOOLCPP, &scene->Shadows_Enabled, "group='Graphics settings'");
	TwAddVarRW(settings, "Reflection and Refraction", TW_TYPE_BOOLCPP, &scene->Refl_Refr_Enabled, "group='Graphics settings'");

	TwEnumVal marble_type[] = { { 0, "Glass"  },
								{ 1,  "Metal" } };

	TwType Marble_type = TwDefineEnum("Marble type", marble_type, 2);
	TwAddVarRW(settings, "Marble type", Marble_type, &scene->MarbleType, "group='Gameplay settings'");
	TwAddVarRW(settings, "Mouse sensitivity", TW_TYPE_FLOAT, mouse_sensitivity, "min=0.001 max=0.02 step=0.001 group='Gameplay settings'");
	TwAddVarRW(settings, "Wheel sensitivity", TW_TYPE_FLOAT, wheel_sensitivity, "min=0.01 max=0.5 step=0.01 group='Gameplay settings'");
	TwAddVarRW(settings, "Music volume", TW_TYPE_FLOAT, music_vol, "min=0 max=100 step=1 group='Gameplay settings'");
	TwAddVarRW(settings, "Target FPS", TW_TYPE_FLOAT, target_fps, "min=24 max=144 step=1 group='Gameplay settings'");
	TwAddVarRW(settings, "Camera size", TW_TYPE_FLOAT, &scene->camera_size, "min=0 max=10 step=0.001 group='Gameplay settings'");
	TwAddVarRW(settings, "Camera speed(Free mode)", TW_TYPE_FLOAT, &scene->free_camera_speed, "min=0 max=10 step=0.001 group='Gameplay settings'");

	int barPos1[2] = { 16, 250 };

	TwSetParam(settings, NULL, "position", TW_PARAM_INT32, 2, &barPos1);

	TwCopyStdStringToClientFunc(CopyStdStringToClient);

	level_editor = TwNewBar("LevelEditor");
	Level *copy = &scene->level_copy;

	TwAddVarRW(level_editor, "Level Name", TW_TYPE_STDSTRING, &copy->txt, "");
	TwAddVarRW(level_editor, "Level Description", TW_TYPE_STDSTRING, &copy->desc, "");

	TwAddButton(level_editor, "Save", SaveLevel, NULL,
		" label='Save Level'  ");

	TwAddButton(level_editor, "Set Marble", MarbleSet, NULL,
		" label='Set Marble Position'  help='Click on the fractal to place' ");

	TwAddButton(level_editor, "Set Flag", FlagSet, NULL,
		" label='Set Flag Position'  help='Click on the fractal to place' ");

	TwAddVarRW(level_editor, "Flag Position", TW_TYPE_DIR3F, copy->end_pos.data(), "");
	TwAddVarRW(level_editor, "Marble Position", TW_TYPE_DIR3F, copy->start_pos.data(), "");
	TwAddVarRW(level_editor, "Marble Radius(Scale)", TW_TYPE_FLOAT, &copy->marble_rad, "min=0 max=10 step=0.001 ");

	std::vector<std::string> music_list = scene->levels.GetMusicNames();
	TwEnumVal *music_enums = new TwEnumVal[music_list.size()];
	for (int i = 0; i < music_list.size(); i++)
	{
		TwEnumVal enumval;
		enumval.Label = music_list[i].c_str();
		enumval.Value = i;
		music_enums[i] = enumval;
	}

	TwType Level_music = TwDefineEnum("Level music", music_enums, music_list.size());
	TwAddVarRW(level_editor, "Level music", Level_music, &music_id, "");

	TwAddButton(level_editor, "Play", PlayMusic, NULL, " label='Play/Stop current music'  ");

	std::map<int, std::string> level_list = scene->levels.getLevelNames();
	TwEnumVal *level_enums = new TwEnumVal[level_list.size()+1];
	TwEnumVal enumval;
	enumval.Label = "None";
	enumval.Value = -1;
	level_enums[0] = enumval;
	int i = 0;
	for (auto &name:level_list)
	{
		enumval.Label = name.second.c_str();
		enumval.Value = i;
		level_enums[i+1] = enumval;
		i++;
	}

	TwType Levels = TwDefineEnum("levels", level_enums, level_list.size()+1);
	TwAddVarRW(level_editor, "Play level after finish(TODO)", Levels, &copy->link_level, "");

	TwAddVarRW(level_editor, "Sun direction", TW_TYPE_DIR3F, copy->light_dir.data(), "group='Level parameters'");
	TwAddVarRW(level_editor, "Sun color", TW_TYPE_DIR3F, copy->light_col.data(), "group='Level parameters'");
	TwAddVarRW(level_editor, "Background color", TW_TYPE_DIR3F, copy->background_col.data(), "group='Level parameters'");
	TwAddVarRW(level_editor, "Gravity strenght", TW_TYPE_FLOAT, &copy->gravity, "min=0 max=0.5 step=0.0001 group='Level parameters'");
	fractal_editor = TwNewBar("FractalEditor");

	TwAddVarRW(fractal_editor, "PBR roughness", TW_TYPE_FLOAT, &copy->PBR_roughness, "min=0 max=1 step=0.001 ");
	TwAddVarRW(fractal_editor, "PBR metallic", TW_TYPE_FLOAT, &copy->PBR_metal, "min=0 max=1 step=0.001");
	float *p = copy->params.data();
	TwAddVarRW(fractal_editor, "Fractal Iterations", TW_TYPE_INT32, &copy->FractalIter, "min=1 max=20 step=1");
	TwAddVarRW(fractal_editor, "Fractal Scale", TW_TYPE_FLOAT, p, "min=0 max=5 step=0.0001");
	TwAddVarRW(fractal_editor, "Fractal Angle1", TW_TYPE_FLOAT, p + 1, "min=-10 max=10 step=0.0001 ");
	TwAddVarRW(fractal_editor, "Fractal Angle2", TW_TYPE_FLOAT, p + 2, "min=-10 max=10 step=0.0001  ");
	TwAddVarRW(fractal_editor, "Fractal Shift", TW_TYPE_DIR3F, p + 3, "");
	TwAddVarRW(fractal_editor, "Fractal Color", TW_TYPE_DIR3F, p + 6, "");

	//TwAddButton(stats, "Info1.1", NULL, NULL, string);
	
	confirmation_box = TwNewBar("confirm");

	TwAddVarRW(confirmation_box, "OK", TW_TYPE_BOOLCPP, &copy->txt, "");
	TwAddVarRW(confirmation_box, "Cancel", TW_TYPE_BOOLCPP, &copy->desc, "");

	TwDefine("confirm visible=false size='300 100' color='255 50 0' alpha=255 label='Are you sure?'");

	TwDefine(" GLOBAL fontsize=3 ");
	TwDefine("LevelEditor visible=false size='420 350' color='0 80 230' alpha=210 label='Level editor' valueswidth=200");
	TwDefine("FractalEditor visible=false size='420 350' color='0 120 200' alpha=210 label='Fractal editor' valueswidth=200");
	TwDefine("Settings color='255 128 0' alpha=210 size='420 350' valueswidth=200");
	TwDefine("Statistics color='0 128 255' alpha=210 size='420 160' valueswidth=200");
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

bool Overlays::TwManageEvent(sf::Event *event)
{
	bool handled = 0;

	TwMouseAction mouseAction;
	int key = 0;
	static int s_KMod = 0;
	static bool s_PreventTextHandling = false;
	static int s_WheelPos = 0;
	if(TWBAR_ENABLED)
	{
		switch (event->type)
		{
		case sf::Event::KeyPressed:
			s_PreventTextHandling = false;
			s_KMod = 0;
			if (event->key.shift)   s_KMod |= TW_KMOD_SHIFT;
			if (event->key.alt)     s_KMod |= TW_KMOD_ALT;
			if (event->key.control) s_KMod |= TW_KMOD_CTRL;
			key = 0;
			switch (event->key.code)
			{
			case sf::Keyboard::Escape:
				key = TW_KEY_ESCAPE;
				break;
			case sf::Keyboard::Return:
				key = TW_KEY_RETURN;
				break;
			case sf::Keyboard::Tab:
				key = TW_KEY_TAB;
				break;
			case sf::Keyboard::BackSpace:
				key = TW_KEY_BACKSPACE;
				break;
			case sf::Keyboard::PageUp:
				key = TW_KEY_PAGE_UP;
				break;
			case sf::Keyboard::PageDown:
				key = TW_KEY_PAGE_DOWN;
				break;
			case sf::Keyboard::Up:
				key = TW_KEY_UP;
				break;
			case sf::Keyboard::Down:
				key = TW_KEY_DOWN;
				break;
			case sf::Keyboard::Left:
				key = TW_KEY_LEFT;
				break;
			case sf::Keyboard::Right:
				key = TW_KEY_RIGHT;
				break;
			case sf::Keyboard::End:
				key = TW_KEY_END;
				break;
			case sf::Keyboard::Home:
				key = TW_KEY_HOME;
				break;
			case sf::Keyboard::Insert:
				key = TW_KEY_INSERT;
				break;
			case sf::Keyboard::Delete:
				key = TW_KEY_DELETE;
				break;
			case sf::Keyboard::Space:
				key = TW_KEY_SPACE;
				break;
			default:
				if (event->key.code >= sf::Keyboard::F1 && event->key.code <= sf::Keyboard::F15)
					key = TW_KEY_F1 + event->key.code - sf::Keyboard::F1;
				else if (s_KMod & TW_KMOD_ALT)
				{
					if (event->key.code >= sf::Keyboard::A && event->key.code <= sf::Keyboard::Z)
					{
						if (s_KMod & TW_KMOD_SHIFT)
							key = 'A' + event->key.code - sf::Keyboard::A;
						else
							key = 'a' + event->key.code - sf::Keyboard::A;
					}
				}
			}
			if (key != 0)
			{
				handled = TwKeyPressed(key, s_KMod);
				s_PreventTextHandling = true;
			}
			break;
		case sf::Event::KeyReleased:
			s_PreventTextHandling = false;
			s_KMod = 0;
			break;
		case sf::Event::TextEntered:
			if (!s_PreventTextHandling && event->text.unicode != 0 && (event->text.unicode & 0xFF00) == 0)
			{
				if ((event->text.unicode & 0xFF) < 32) // CTRL+letter
					handled = TwKeyPressed((event->text.unicode & 0xFF) + 'a' - 1, TW_KMOD_CTRL | s_KMod);
				else
					handled = TwKeyPressed(event->text.unicode & 0xFF, 0);
			}
			s_PreventTextHandling = false;
			break;
		case sf::Event::MouseMoved:
			handled = TwMouseMotion(event->mouseMove.x, event->mouseMove.y);
			break;
		case sf::Event::MouseButtonPressed:
		case sf::Event::MouseButtonReleased:
			mouseAction = (event->type == sf::Event::MouseButtonPressed) ? TW_MOUSE_PRESSED : TW_MOUSE_RELEASED;
			switch (event->mouseButton.button)
			{
			case sf::Mouse::Left:
				handled = TwMouseButton(mouseAction, TW_MOUSE_LEFT);
				break;
			case sf::Mouse::Middle:
				handled = TwMouseButton(mouseAction, TW_MOUSE_MIDDLE);
				break;
			case sf::Mouse::Right:
				handled = TwMouseButton(mouseAction, TW_MOUSE_RIGHT);
				break;
			default:
				break;
			}
			break;
		case sf::Event::MouseWheelMoved:
			s_WheelPos += event->mouseWheel.delta;
			handled = TwMouseWheel(s_WheelPos);
			break;
		default:
			break;
		}
	}

	if (sf::Event::Resized == event->type)
	{
		TwWindowSize(event->size.width, event->size.height);
	}

	return handled;
}

void Overlays::SetTWBARResolution(int Width, int Height)
{
	TwWindowSize(Width, Height);
}
