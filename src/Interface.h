#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <map>

//the object parameters
struct State
{
	sf::Vector2f position = sf::Vector2f(0,0);
	sf::Vector2f size = sf::Vector2f(0.1f, 0.1f);
	float border_thickness = 0.f;
	sf::Color color_main = sf::Color::Black;
	sf::Color color_second = sf::Color::White;
	sf::Color color_border = sf::Color(128,128,128);
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
	std::vector<Allign> object_alligns;
	std::vector<int> object_ids;
	float cur_shift_x1, cur_shift_x2; //left and right shifts
	float cur_shift_y;
	

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