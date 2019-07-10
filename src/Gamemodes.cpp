#include "Gamemodes.h"

void OpenTestWindow(sf::Font & font)
{
	Window test(200, 200, 500, 500, sf::Color(0, 0, 0, 128), LOCAL["Window"], font);
	Text button(LOCAL["Button"], font, 30, sf::Color::White);
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
	sf::Vector2f wsize = default_view.getSize();
	MenuBox levels(wsize.x*0.85, wsize.y*0.95, wsize.x*0.075, wsize.y*0.025);
	game_mode = LEVELS;
	std::vector<std::string> names = scene->levels.getLevelNames();
	std::vector<std::string> desc = scene->levels.getLevelDesc();
	std::vector<int> ids = scene->levels.getLevelIds();

	Box Bk2Menu(100, 40);
	Text button(LOCAL["Back2Main"], LOCAL("default"), 35, sf::Color::White);
	Bk2Menu.SetCallbackFunction([scene](sf::RenderWindow * window, InputState & state)
	{
		RemoveAllObjects();
		game_mode = MAIN_MENU;
		scene->SetExposure(1.0f);
	});
	Bk2Menu.AddObject(&button, Object::Allign::CENTER);
	levels.AddObject(&Bk2Menu, Object::Allign::LEFT);

	for (int i = 0; i < scene->levels.GetLevelNum(); i++)
	{
		Box lvlbtton(wsize.x*0.8, 60);
		Text lvlname(utf8_to_wstring(names[i]), LOCAL("default"), 35, sf::Color::White);
		lvlname.SetCallbackFunction([scene, selected = ids[i]](sf::RenderWindow * window, InputState & state)
		{
			RemoveAllObjects();
			//play level
			game_mode = PLAYING;
			menu_music.stop();
			scene->SetExposure(1.0f);
			scene->levels.GetLevelMusic(selected)->setVolume(GetVol());
			scene->levels.GetLevelMusic(selected)->play();
			scene->StartSingle(selected);
			LockMouse(*window);
		});
		lvlbtton.AddObject(&lvlname, Object::Allign::LEFT);


		levels.AddObject(&lvlbtton, Object::Allign::LEFT);
	}

	Box Newlvl(100, 40);
	Text newlvl(LOCAL["CreateNewLvl"], LOCAL("default"), 35, sf::Color::White);
	Newlvl.SetCallbackFunction([scene, overlays](sf::RenderWindow * window, InputState & state)
	{
		RemoveAllObjects();
		//go to level editor
		game_mode = LEVEL_EDITOR;
		menu_music.stop();
		scene->SetExposure(1.0f);
		overlays->TWBAR_ENABLED = true;
		TwDefine("LevelEditor visible=true position='20 20'");
		TwDefine("FractalEditor visible=true position='20 500'");
		TwDefine("Settings iconified=true");
		TwDefine("Statistics iconified=true");
		scene->StartLevelEditor(-1);
	});
	Newlvl.AddObject(&newlvl, Object::Allign::CENTER);
	levels.AddObject(&Newlvl, Object::Allign::LEFT);

}