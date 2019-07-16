#include "Gamemodes.h"

//Global variables
sf::Vector2i mouse_pos, mouse_prev_pos;
InputState io_state;

bool all_keys[sf::Keyboard::KeyCount] = { 0 };
bool mouse_clicked = false;
bool show_cheats = false;

//Constants
float mouse_sensitivity = 0.005f;
float wheel_sensitivity = 0.2f;
float music_vol = 75.0f;
float target_fps = 60.0f;

GameMode game_mode = MAIN_MENU;

void OpenMainMenu(Scene * scene, Overlays * overlays)
{
	RemoveAllObjects();
	game_mode = MAIN_MENU;
	scene->SetExposure(1.0f);
}

void OpenEditor(Scene * scene, Overlays * overlays, int level)
{
	RemoveAllObjects();
	//go to level editor
	game_mode = LEVEL_EDITOR;
	scene->SetExposure(1.0f);
	overlays->TWBAR_ENABLED = true;
	TwDefine("LevelEditor visible=true position='20 20'");
	TwDefine("FractalEditor visible=true position='20 500'");
	TwDefine("Settings iconified=true");
	TwDefine("Statistics iconified=true");
	scene->StartLevelEditor(level);
}

void PlayLevel(Scene * scene, sf::RenderWindow * window, int level)
{
	RemoveAllObjects();
	//play level
	game_mode = PLAYING;
	scene->SetExposure(1.0f);
	scene->levels.GetLevelMusic(level)->setVolume(GetVol());
	scene->levels.GetLevelMusic(level)->play();
	scene->StartSingle(level);
	LockMouse(*window);
}

void OpenTestWindow(sf::Font & font)
{
	Window test(200, 200, 500, 500, sf::Color(0, 0, 0, 128), LOCAL["Window"], LOCAL("default"));
	Text button(LOCAL["Button"], LOCAL("default"), 30, sf::Color::White);
	Box sbox(0, 0, 420, 200, sf::Color(128, 128, 128, 240));
	Box sbox2(0, 0, 240, 40, sf::Color(0, 64, 128, 240));
	Box sbox3(0, 0, 30, 30, sf::Color(0, 64, 128, 240));

	sbox2.hoverstate.color_main = sf::Color(230, 40, 20, 200);
	sbox2.AddObject(&button, Box::CENTER);
	button.hoverstate.font_size = 40;
	test.Add(&sbox, Box::CENTER);
	test.Add(&sbox2, Box::CENTER);
	test.Add(&sbox, Box::CENTER);
	test.Add(&sbox2, Box::CENTER);
	test.Add(&sbox, Box::CENTER);
	test.Add(&sbox2, Box::CENTER);
	
	AddGlobalObject(test);
}

void OpenLevelMenu(Scene* scene, Overlays* overlays)
{
	RemoveAllObjects();
	sf::Vector2f wsize = default_view.getSize();

	MenuBox levels(wsize.x*0.95f, wsize.y*0.95f, wsize.x*0.025f, wsize.y*0.025f);
	//make the menu static
	levels.static_object = true;

	game_mode = LEVELS;
	std::vector<std::string> names = scene->levels.getLevelNames();
	std::vector<std::string> desc = scene->levels.getLevelDesc();
	std::vector<int> ids = scene->levels.getLevelIds();
	std::map<int, Score> scores = scene->levels.getLevelScores();
	Text lvl(LOCAL["Levels"], LOCAL("default"), 60, sf::Color::White);
	levels.AddObject(&lvl, Object::Allign::CENTER);
	Box Bk2Menu(600, 50);
	Text button(LOCAL["Back2Main"], LOCAL("default"), 40, sf::Color::White);
	Bk2Menu.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	Bk2Menu.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		OpenMainMenu(scene, overlays);
		overlays->sound_click.play();
	});
	Bk2Menu.AddObject(&button, Object::Allign::CENTER);
	levels.AddObject(&Bk2Menu, Object::Allign::LEFT);

	Box Newlvl(600, 50);
	Text newlvl(LOCAL["CreateNewLvl"], LOCAL("default"), 40, sf::Color::White);
	Newlvl.hoverstate.color_main = sf::Color(200, 40, 0, 255);
	Newlvl.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		OpenEditor(scene, overlays, -1);
		overlays->sound_click.play();
	});
	Newlvl.AddObject(&newlvl, Object::Allign::CENTER);
	levels.AddObject(&Newlvl, Object::Allign::LEFT);
	
	sf::Image edit; edit.loadFromFile(edit_png);
	sf::Texture edittxt; edittxt.loadFromImage(edit);
	edittxt.setSmooth(true);

	sf::Image remove; remove.loadFromFile(delete_png);
	sf::Texture removetxt; removetxt.loadFromImage(remove);
	removetxt.setSmooth(true);

	for (int i = 0; i < scene->levels.GetLevelNum(); i++)
	{
		Box lvlbtton(wsize.x*0.95f - 60, 60);
		Box lvltext(500, 60);
		lvltext.SetBackgroundColor(sf::Color::Transparent);
		Box lvltitle(500, 40);
		lvltitle.SetBackgroundColor(sf::Color::Transparent);
		Text lvlname(utf8_to_wstring(names[i]), LOCAL("default"), 35, sf::Color::White);
		Text lvldescr(utf8_to_wstring(desc[i]), LOCAL("default"), 18, sf::Color::White);
		lvlname.hoverstate.color_main = sf::Color(255, 0, 0, 255);
		lvlname.SetCallbackFunction([scene, overlays, selected = ids[i]](sf::RenderWindow * window, InputState & state)
		{
			PlayLevel(scene, window, selected);
			overlays->sound_click.play();
		});
		lvltitle.AddObject(&lvlname, Object::Allign::LEFT);
		lvltext.AddObject(&lvltitle, Object::Allign::LEFT);
		lvltext.AddObject(&lvldescr, Object::Allign::LEFT);
		lvlbtton.AddObject(&lvltext, Object::Allign::LEFT);


		Box lvlscore(500, 40);
		lvlscore.SetBackgroundColor(sf::Color::Transparent);
		std::string score_text = "--:--:--";
		if (scores[ids[i]].best_time != 0)
		{
			float time = scores[ids[i]].best_time;
			float minutes = floor(time / 60.f);
			float seconds = floor(time) - minutes*60;
			float mili = floor(time*100) - seconds*100 - minutes*6000;
			//convrt mili to frames
			score_text = num2str(minutes) + ":" + num2str(seconds) + ":" + num2str(floor(mili*0.6f));
		}
		Text lvlscorev(score_text, LOCAL("default"), 35, sf::Color::White);
		lvlscorev.SetBackgroundColor(sf::Color::Green);
		lvlscore.AddObject(&lvlscorev, Object::Allign::CENTER);
		lvlbtton.AddObject(&lvlscore, Object::Allign::LEFT);

		Box lvlavg(500, 40);
		lvlavg.SetBackgroundColor(sf::Color::Transparent);
		score_text = "--:--:--";
		if (scores[ids[i]].best_time != 0)
		{
			float time = scores[ids[i]].all_time/scores[ids[i]].played_num;
			float minutes = floor(time / 60.f);
			float seconds = floor(time) - minutes * 60;
			float mili = floor(time * 100) - seconds * 100 - minutes * 6000;
			score_text = num2str(minutes) + ":" + num2str(seconds) + ":" + num2str(floor(mili*0.6f));
		}
		Text lvlavgtxt(score_text, LOCAL("default"), 35, sf::Color::White);
		lvlavgtxt.SetBackgroundColor(sf::Color::White);
		lvlavg.AddObject(&lvlavgtxt, Object::Allign::CENTER);
		lvlbtton.AddObject(&lvlavg, Object::Allign::LEFT);

		Box buttons(120, 60);
		buttons.SetBackgroundColor(sf::Color::Transparent);
		Box bedit(60, 60);
		bedit.defaultstate.color_main = sf::Color(255, 255, 255, 255);
		bedit.hoverstate.color_main = sf::Color(0, 255, 0, 255);
		bedit.SetBackground(edittxt);
		bedit.SetCallbackFunction([scene, overlays, id = ids[i]](sf::RenderWindow * window, InputState & state)
		{
			OpenEditor(scene, overlays, id);
			overlays->sound_click.play();
		}, true);


		Box bremove(60, 60);
		bremove.defaultstate.color_main = sf::Color(255, 255, 255, 255);
		bremove.hoverstate.color_main = sf::Color(255, 0, 0, 255);
		bremove.SetBackground(removetxt);

		bremove.SetCallbackFunction([scene, overlays, id = ids[i]](sf::RenderWindow * window, InputState & state)
		{
			ConfirmLevelDeletion(id, scene, overlays);
			overlays->sound_click.play();
		}, true);

		buttons.AddObject(&bremove, Object::Allign::RIGHT);
		buttons.AddObject(&bedit, Object::Allign::RIGHT);

		lvlbtton.AddObject(&buttons, Object::Allign::RIGHT);

		levels.AddObject(&lvlbtton, Object::Allign::LEFT);
	}

	AddGlobalObject(levels);
}

void ConfirmLevelDeletion(int lvl, Scene* scene, Overlays* overlays)
{
	sf::Vector2f wsize = default_view.getSize();
	Window confirm(wsize.x*0.45f, wsize.y*0.45f, 500, 215, sf::Color(0, 0, 0, 128), LOCAL["You_sure"], LOCAL("default"));
	Text button1(LOCAL["Yes"], LOCAL("default"), 30, sf::Color::White);
	Text button2(LOCAL["No"], LOCAL("default"), 30, sf::Color::White);
	Text text(LOCAL["You_sure"], LOCAL("default"), 30, sf::Color::White);
	 
	Box but1(0, 0, 240, 40, sf::Color(0, 64, 128, 240));
	Box but2(0, 0, 240, 40, sf::Color(0, 64, 128, 240));

	but1.hoverstate.color_main = sf::Color(230, 40, 20, 200);
	but2.hoverstate.color_main = sf::Color(40, 230, 20, 200);
	but1.AddObject(&button1, Box::CENTER);
	but2.AddObject(&button2, Box::CENTER);

	confirm.Add(&text, Box::CENTER);
	confirm.Add(&but1, Box::RIGHT);
	confirm.Add(&but2, Box::RIGHT);

	int id = AddGlobalObject(confirm);

	get_glob_obj(id).objects[1].get()->objects[0].get()->objects[1].get()->SetCallbackFunction([scene, overlays, id](sf::RenderWindow * window, InputState & state)
	{
		//remove lvl
		Add2DeleteQueue(id);
		overlays->sound_click.play();
	});

	get_glob_obj(id).objects[1].get()->objects[0].get()->objects[2].get()->SetCallbackFunction([scene, overlays, id](sf::RenderWindow * window, InputState & state)
	{
		Add2DeleteQueue(id);
		overlays->sound_click.play();
	});
}


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