#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <map>

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

//the object parameters
struct State
{
	sf::Vector2f position = sf::Vector2f(0,0);
	sf::Vector2f size = sf::Vector2f(0.1f, 0.1f);
	float border_thickness = 0.f;
	float margin = 0.f;
	ColorFloat color_main = ToColorF(sf::Color::Black);
	ColorFloat color_second = ToColorF(sf::Color::White);
	ColorFloat color_border = ColorFloat(128,128,128);
};

//interpolate between states for animation effects
State interpolate(State a, State b, float t);

void UpdateAllObjects(sf::RenderWindow * window, sf::Vector2i mouse, bool RMB, bool LMB, bool all_keys[], float dt);

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

	void SetCallbackFunction(void(*fun)(void*));
	void SetHoverFunction(void(*fun)(void*));

	void clone_states();

	virtual void Draw(sf::RenderWindow * window);

	void Update(sf::RenderWindow * window, sf::Vector2i mouse, bool RMB, bool LMB, bool all_keys[], float dt);

	Object();
	~Object();

	State curstate;
	State activestate;
	State hoverstate;
	State defaultstate;
	States curmode;

	int id;

	void(*callback)(void*);
	void(*hoverfn)(void*);

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

	void Draw(sf::RenderWindow *window);

	Box(float x, float y, float dx, float dy, sf::Color color_main);

private:

	sf::RectangleShape rect;

	//objects inside the box
	std::map<int, Object*> objects;
	std::map<int, Allign> object_alligns;
	std::vector<int> object_ids;
	
};

/*
class Text : public Object
{
public:
	void SetFontSize(float rel_size);
	void UseFont(sf::Font font);

	void SetPosition(float x, float y);
	void Draw(sf::RenderWindow *window);
};

class Image : public Object
{
	void LoadFromFile(std::string filename);

	virtual void SetPosition(float x, float y);
	virtual void SetSize(float x, float y);
	virtual void Draw(sf::RenderWindow *window);
};

class Menu : public Box
{

};
*/