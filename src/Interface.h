#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <map>
#include <stack>
#include <algorithm>
#include <iterator>
#include <functional>

struct ColorFloat
{
	float r, g, b, a;
	ColorFloat(float red = 0.f, float green = 0.f, float blue = 0.f, float alpha = 255.f);

	void operator=(sf::Color a);
};

ColorFloat operator+(ColorFloat a, ColorFloat b);

ColorFloat operator-(ColorFloat a, ColorFloat b);

ColorFloat operator*(ColorFloat a, float b);

sf::Color ToColor(ColorFloat a);
ColorFloat ToColorF(sf::Color a);


struct InputState
{
	bool keys[sf::Keyboard::KeyCount] = { false };
	bool mouse[3] = { false };
	float wheel = 0.f;
	sf::Vector2f mouse_pos = sf::Vector2f(0,0);
	sf::Vector2f mouse_prev = sf::Vector2f(0, 0);
	sf::Vector2f mouse_speed = sf::Vector2f(0, 0);
	sf::Vector2f window_size = sf::Vector2f(0, 0);
	float time = 0, dt = 0;

	InputState();
	InputState(bool keys[sf::Keyboard::KeyCount], bool mouse[3], sf::Vector2f mouse_pos, sf::Vector2f mouse_speed);
};


//the object parameters
struct State
{
	sf::Vector2f position = sf::Vector2f(0,0);
	sf::Vector2f size = sf::Vector2f(0.1f, 0.1f);
	float border_thickness = 0.f;
	float margin = 0.f;
	float font_size = 1.f;
	float scroll = 0.f;
	ColorFloat color_main = ToColorF(sf::Color::Black);
	ColorFloat color_second = ToColorF(sf::Color::Transparent);
	ColorFloat color_border = ColorFloat(128,128,128);
};

//interpolate between states for animation effects
State interpolate(State a, State b, float t);

void UpdateAllObjects(sf::RenderWindow * window, InputState& state);

//the object base class
class Object
{
public:
	enum States
	{
		DEFAULT, ONHOVER, ACTIVE
	};

	void SetPosition(float x, float y);
	void SetSize(float x, float y);
	void SetBackgroundColor(sf::Color color);
	void SetBorderColor(sf::Color color);
	void SetBorderWidth(float S);
	void SetMargin(float x);

	void SetScroll(float x);
	void ApplyScroll(float x);

	void Move(sf::Vector2f dx);

	void SetDefaultFunction(std::function<void(sf::RenderWindow * window, InputState & state)> fun);
	void SetCallbackFunction(std::function<void(sf::RenderWindow * window, InputState & state)> fun);
	void SetHoverFunction(std::function<void(sf::RenderWindow * window, InputState & state)> fun);

	void clone_states();

	virtual void Draw(sf::RenderWindow * window, InputState& state);

	void Update(sf::RenderWindow * window, InputState& state);

	Object();
	~Object();

	State curstate;
	State activestate;
	State hoverstate;
	State defaultstate;
	States curmode;

	sf::View used_view;

	int id;

	std::function<void(sf::RenderWindow * window, InputState & state)> callback, hoverfn, defaultfn;

	//operation time limiter
	float action_time;
};

//a box to add stuff in
class Box: public Object
{
public:
	enum Allign
	{
		LEFT, CENTER, RIGHT
	};
	void AddObject(Object* something, Allign a = LEFT);

	void Draw(sf::RenderWindow *window, InputState& state);

	Box(float x, float y, float dx, float dy, sf::Color color_main);
	Box();

	std::vector< std::pair<Allign, Object*>> objects;
private:
	sf::RectangleShape rect;
	sf::RectangleShape scroll1, scroll2;
	sf::View boxView;

	//objects inside the box
};


class Text: public Object
{
public:
	void Draw(sf::RenderWindow *window, InputState& state);
	sf::Text text;

	Text(std::string text, sf::Font &f, float size, sf::Color col);
	Text(sf::Text t);
};

class Window: public Box
{
public:
	std::unique_ptr<Box> Bar, Inside, Scroll, Scroll_Slide;
	std::unique_ptr<Text> Title;

	sf::Vector2f dmouse;

	void Add(Object* something, Allign a = LEFT);
	void ScrollBy(float dx);

	Window(float x, float y, float dx, float dy, sf::Color color_main, std::string title, sf::Font & font);
};

class MenuBox : public Box
{
public:
	std::unique_ptr<Box> Inside, Scroll, Scroll_Slide;
	int cursor_id;

	sf::Vector2f dmouse;

	void Add(Object* something, Allign a = LEFT);
	void Cursor(int d);
	void ScrollBy(float dx);

	MenuBox(float x, float y, float dx, float dy, sf::Color color_main, std::string title, sf::Font & font);
};


/*

class Image : public Object
{
	Image();


	void Draw(sf::RenderWindow *window);
};

*/