#pragma once

//the object parameters
class State
{
public:
	sf::Vector2f position;
	sf::Vector2f size;
	sf::Vector2f color_main;
	sf::Vector2f color_second;
	sf::Vector2f border_color;
	sf::Vector2f position_cur;
	sf::Vector2f size_cur;
};

//the object base class
class Object
{
public:
	virtual void SetPosition(float x, float y);
	virtual void SetSize(float x, float y);
	virtual void Draw(sf::RenderWindow *window);
	virtual void Update(sf::Vector2f mouse, bool all_keys[], float time);
	void SetCallbackFunction(void (*fun)(void*));

private:
	

	sf::Vecror2i window_size;
	void(*callbk)(void*);
protected:

};

//a box to add stuff in
class Box: public Object
{
public:
	enum Align
	{
		LEFT, CENTER, RIGHT
	};
	void SetMargin(float S);
	void AddObject(Object* something, Align a = LEFT);
	void SetBackgroundColor(sf::Color color);
	void SetBorderColor(sf::Color color);
	void SetBorderWidth(float S);

	void SetSize(float x, float y);
	void SetPosition(float x, float y);
	void Draw(sf::RenderWindow *window);
};

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
