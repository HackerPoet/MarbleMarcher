#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <map>
#include <stack>
#include <algorithm>
#include <iterator>
#include <functional>
#include <stack>

//default interface parameters
extern sf::Color default_main_color;
extern sf::Color default_hover_main_color;
extern sf::Color default_active_main_color;
extern float default_margin;
extern sf::View default_view ;

extern float animation_sharpness;
extern float action_dt;

static const std::string close_png = "images/clear.png";

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
	float margin = default_margin;
	float font_size = 1.f;
	float scroll = 0.f;
	float inside_size = 0.f;
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

	enum Allign
	{
		LEFT, CENTER, RIGHT
	};

	void ApplyScroll(float x);
	void SetPosition(float x, float y);
	void SetSize(float x, float y);
	void SetHeigth(float x);
	void SetWidth(float x);
	void SetBackgroundColor(sf::Color color);
	void SetBorderColor(sf::Color color);
	void SetBorderWidth(float S);
	void SetMargin(float x);
	void SetInsideSize(float x);
	void SetScroll(float x);
	void Move(sf::Vector2f dx);

	void SetDefaultFunction(std::function<void(sf::RenderWindow * window, InputState & state)> fun);
	void SetCallbackFunction(std::function<void(sf::RenderWindow * window, InputState & state)> fun);
	void SetHoverFunction(std::function<void(sf::RenderWindow * window, InputState & state)> fun);

	void clone_states();

	virtual void Draw(sf::RenderWindow * window, InputState& state);
	virtual void AddObject(Object* a, Allign b);
	void Update(sf::RenderWindow * window, InputState& state);
	void UpdateAction(sf::RenderWindow * window, InputState& state);
	
	Object();
	~Object();

	Object(Object& A);
	Object(Object&& A);
	Object(Object* A);

	virtual void operator=(Object& A);
	virtual void operator=(Object&& A);

	void copy(Object& A);
	void copy(Object&& A);

	virtual Object* GetCopy();

	State curstate;
	State activestate;
	State hoverstate;
	State defaultstate;
	States curmode;

	Allign obj_allign;

	sf::View used_view;
	
	std::function<void(sf::RenderWindow * window, InputState & state)> callback, hoverfn, defaultfn;

	//objects inside this object
	std::vector<std::unique_ptr<Object>> objects;
	std::stack<int> z_buffer;

	//operation time limiter
	float action_time;

	int id;
};

//a box to add stuff in
class Box: public Object
{
public:
	void SetBackground(const sf::Texture & texture);
	void Draw(sf::RenderWindow *window, InputState& state);

	Box(float dx, float dy, float x = 0, float y = 0, sf::Color color_main = default_main_color);
	Box();

	Box(Box& A);
	Box(Box&& A);

	void operator=(Box& A);
	void operator=(Box&& A);

	virtual Object* GetCopy();
private:
	sf::Texture image;
	sf::RectangleShape rect;
	sf::View boxView;
};


class Text: public Object
{
public:
	sf::Text text;

	void Draw(sf::RenderWindow *window, InputState& state);

	template<class T>
	Text(T str, sf::Font &f, float size, sf::Color col);
	Text(sf::Text t);

	Text(Text& A);
	Text(Text&& A);

	void operator=(Text& A);
	void operator=(Text&& A);

	virtual Object* GetCopy();
};

class Window: public Box
{
public:
	sf::Vector2f dmouse;

	void Add(Object * something, Allign a = LEFT);
	void ScrollBy(float dx);

	template<class T>
	Window(float x, float y, float dx, float dy, sf::Color color_main, T title, sf::Font & font);

	Window(Window& A);
	Window(Window&& A);

	void CreateCallbacks();

	void operator=(Window& A);
	void operator=(Window&& A);

	virtual Object* GetCopy();
};

class MenuBox : public Box
{
public:
	std::unique_ptr<Box> Inside, Scroll, Scroll_Slide;
	int cursor_id;

	sf::Vector2f dmouse;

	void Add(Object * something, Allign a);
	void Cursor(int d);
	void ScrollBy(float dx);

	MenuBox(float x, float y, float dx, float dy, sf::Color color_main, std::string title, sf::Font & font);

	MenuBox(MenuBox& A);
	MenuBox(MenuBox&& A);

	void CreateCallbacks();

	void operator=(MenuBox& A);
	void operator=(MenuBox&& A);

	virtual Object* GetCopy();
};

void AddGlobalObject(Object & a);
void RemoveGlobalObject(int id);

template<class T>
inline Window::Window(float x, float y, float dx, float dy, sf::Color color_main, T title, sf::Font & font)
{
	defaultstate.position.x = x;
	defaultstate.position.y = y;
	defaultstate.size.x = dx;
	defaultstate.size.y = dy;
	defaultstate.color_main = ToColorF(color_main);
	clone_states();

	Box Bar(0, 0, dx, 30, sf::Color(0, 100, 200, 128)),
		CloseBx(0, 0, 30, 30, sf::Color(255, 255, 255, 255)),
		Inside(0, 0, dx - 30, dy - 30, sf::Color(100, 100, 100, 128)),
		Scroll(0, 0, 30, dy - 30, sf::Color(150, 150, 150, 128)),
		Scroll_Slide(0, 0, 27, 60, sf::Color(255, 150, 0, 128));
	Text Title(title, font, 25, sf::Color::White);

	CloseBx.hoverstate.color_main = sf::Color(255, 0, 0, 255);
	Scroll_Slide.hoverstate.color_main = sf::Color(255, 50, 0, 128);
	Scroll_Slide.activestate.color_main = sf::Color(255, 100, 100, 255);

	sf::Image close; close.loadFromFile(close_png);
	sf::Texture closetxt; closetxt.loadFromImage(close);
	closetxt.setSmooth(true);
	CloseBx.SetBackground(closetxt);

	Bar.AddObject(&Title, Box::LEFT);
	Bar.AddObject(&CloseBx, Box::RIGHT);
	Scroll.AddObject(&Scroll_Slide, Box::CENTER);
	this->AddObject(&Bar, Box::CENTER);
	this->AddObject(&Inside, Box::LEFT);
	this->AddObject(&Scroll, Box::RIGHT);

	CreateCallbacks();
}

template<class T>
inline Text::Text(T str, sf::Font & f, float size, sf::Color col)
{
	text.setString(str);
	text.setFont(f);
	defaultstate.font_size = size;
	defaultstate.color_main = col;

	clone_states();
}
