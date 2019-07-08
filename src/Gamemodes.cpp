#include "Gamemodes.h"

void OpenTestWindow(sf::Font & font)
{
	Window test(200, 200, 500, 500, sf::Color(0, 0, 0, 128), "Test window", font);
	test.objects[1].get()->SetMargin(5);
	Text button("button", font, 30, sf::Color::White);
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
