#include <Interface.h>

std::map<int, Object*> global_objects;
int obj_id = 0;
float animation_sharpness = 2.f;


ColorFloat operator+(ColorFloat a, ColorFloat b)
{
	ColorFloat c;
	c.r = a.r + b.r;
	c.g = a.g + b.g;
	c.b = a.b + b.b;
	c.a = a.a + b.a;
	return c;
}

ColorFloat operator-(ColorFloat a, ColorFloat b)
{
	ColorFloat c;
	c.r = a.r - b.r;
	c.g = a.g - b.g;
	c.b = a.b - b.b;
	c.a = a.a - b.a;
	return c;
}

ColorFloat operator*(ColorFloat a, float b)
{
	ColorFloat c;
	c.r = a.r * b;
	c.g = a.g * b;
	c.b = a.b * b;
	c.a = a.a * b;
	return c;
}

sf::Color ToColor(ColorFloat a)
{
	return sf::Color(a.r, a.g, a.b, a.a);
}

ColorFloat ToColorF(sf::Color a)
{
	return ColorFloat(a.r, a.g, a.b, a.a);
}

State interpolate(State a, State b, float t)
{
	State C;
	C.position.x = a.position.x*(1.f - t) + b.position.x*t;
	C.position.y = a.position.y*(1.f - t) + b.position.y*t;
	C.size.x = a.size.x*(1.f - t) + b.size.x*t;
	C.size.y = a.size.y*(1.f - t) + b.size.y*t;
	C.border_thickness = a.border_thickness*(1.f - t) + b.border_thickness*t;
	C.color_main.r = a.color_main.r*(1.f - t) + b.color_main.r*t;
	C.color_main.g = a.color_main.g*(1.f - t) + b.color_main.g*t;
	C.color_main.b = a.color_main.b*(1.f - t) + b.color_main.b*t;
	C.color_second.r = a.color_second.r*(1.f - t) + b.color_second.r*t;
	C.color_second.g = a.color_second.g*(1.f - t) + b.color_second.g*t;
	C.color_second.b = a.color_second.b*(1.f - t) + b.color_second.b*t;
	C.color_border.r = a.color_border.r*(1.f - t) + b.color_border.r*t;
	C.color_border.g = a.color_border.g*(1.f - t) + b.color_border.g*t;
	C.color_border.b = a.color_border.b*(1.f - t) + b.color_border.b*t;
	return C;
}

State operator+(State a, State b)
{
	State C;
	C.position.x = a.position.x+ b.position.x;
	C.position.y = a.position.y + b.position.y;
	C.size.x = a.size.x + b.size.x;
	C.size.y = a.size.y+ b.size.y;
	C.border_thickness = a.border_thickness + b.border_thickness;
	C.color_main.r = a.color_main.r + b.color_main.r;
	C.color_main.g = a.color_main.g + b.color_main.g;
	C.color_main.b = a.color_main.b* + b.color_main.b;
	C.color_second.r = a.color_second.r + b.color_second.r;
	C.color_second.g = a.color_second.g + b.color_second.g;
	C.color_second.b = a.color_second.b + b.color_second.b;
	C.color_border.r = a.color_border.r + b.color_border.r;
	C.color_border.g = a.color_border.g + b.color_border.g;
	C.color_border.b = a.color_border.b + b.color_border.b;
	return C;
}

State operator*(State a, float b)
{
	State C;
	C.position.x = a.position.x * b;
	C.position.y = a.position.y * b;
	C.size.x = a.size.x * b;
	C.size.y = a.size.y * b;
	C.border_thickness = a.border_thickness * b;
	C.color_main.r = a.color_main.r * b;
	C.color_main.g = a.color_main.g * b;
	C.color_main.b = a.color_main.b * b;
	C.color_second.r = a.color_second.r * b;
	C.color_second.g = a.color_second.g * b;
	C.color_second.b = a.color_second.b * b;
	C.color_border.r = a.color_border.r * b;
	C.color_border.g = a.color_border.g * b;
	C.color_border.b = a.color_border.b * b;
	return C;
}

void Object::SetPosition(float x, float y)
{
	defaultstate.position = sf::Vector2f(x, y);
	curstate.position = sf::Vector2f(x, y);
	activestate.position = sf::Vector2f(x, y);
	hoverstate.position = sf::Vector2f(x, y);
}

void Object::SetSize(float x, float y)
{
	defaultstate.size = sf::Vector2f(x, y);
	curstate.size = sf::Vector2f(x, y);
	activestate.size = sf::Vector2f(x, y);
	hoverstate.size = sf::Vector2f(x, y);
}

void Object::SetBackgroundColor(sf::Color color)
{
}

void Object::SetBorderColor(sf::Color color)
{
}

void Object::SetBorderWidth(float S)
{
}

void Object::SetCallbackFunction(void(*fun)(void *))
{
	callback = fun;
}

void Object::SetHoverFunction(void(*fun)(void *))
{
	hoverfn = fun;
}

void Object::clone_states()
{
	curstate = defaultstate;
	activestate = defaultstate;
	hoverstate = defaultstate;
}

void Object::Draw(sf::RenderWindow * window)
{
	//nothing to draw
}

void Object::Update(sf::RenderWindow * window, sf::Vector2i mouse, bool RMB, bool LMB, bool all_keys[], float dt)
{
	//callback stuff
	sf::Vector2f worldPos = window->mapPixelToCoords(mouse);
	sf::FloatRect obj(curstate.position.x, curstate.position.y, curstate.size.x, curstate.size.y);

	//if mouse is inside the object 
	if (obj.contains(worldPos))
	{
		if (RMB || LMB) //if click
		{
			if(callback != NULL)
				(*callback)((void*)&worldPos); //run callback with WorldPos info
			curmode = ACTIVE;
		}
		else // if hover
		{
			if (hoverfn != NULL)
				(*hoverfn)((void*)&worldPos); //run callback with WorldPos info
			curmode = ONHOVER;
		}
	}
	else
	{
		curmode = DEFAULT;
	}

	float t = 1.f - exp(-animation_sharpness*dt);

	//animation
	switch (curmode)
	{
	case DEFAULT:
		curstate = interpolate(curstate, defaultstate, t); 
		break;
	case ACTIVE:
		curstate = interpolate(curstate, activestate, t);
		break;
	case ONHOVER:
		curstate = interpolate(curstate, hoverstate, t);
		break;
	}

	Draw(window);
}

Object::Object() : callback(NULL), hoverfn(NULL), curmode(DEFAULT)
{
	id = obj_id;
	obj_id++;
	global_objects[id] = this;//add it to the global list
}

Object::~Object()
{
	
}

void Box::AddObject(Object * something, Allign a)
{
}

void Box::Draw(sf::RenderWindow * window)
{
	//update the box itself
	rect.setPosition(curstate.position);
	rect.setSize(curstate.size);
	rect.setFillColor(ToColor(curstate.color_main));
	rect.setOutlineThickness(curstate.border_thickness);
	rect.setOutlineColor(ToColor(curstate.color_border));
	window->draw(rect);

	float line_height = 0;
	//update all the stuff inside the box
	for (auto &obj : objects)
	{
		Allign A = object_alligns[obj.first];
		line_height = std::max(obj.second->curstate.size.y, line_height);
		float space_left = defaultstate.size.y - cur_shift_x1 - cur_shift_x2;
		float obj_width = obj.second->curstate.size.x;
		if (space_left > obj_width)
		{
			switch (A)
			{
			case LEFT:
				cur_shift_x1 += obj_width;
				obj.second->SetPosition(curstate.position.x + cur_shift_x1, curstate.position.y + cur_shift_y);
				break;
			case CENTER:
				obj.second->SetPosition(curstate.position.x + defaultstate.size.x * 0.5f - obj_width * 0.5f, curstate.position.y + cur_shift_y);
				cur_shift_y += line_height;
				line_height = 0;
				break;
			case RIGHT:
				cur_shift_x2 += obj_width;
				obj.second->SetPosition(curstate.position.x + defaultstate.size.x - obj_width - cur_shift_x1, curstate.position.y + cur_shift_y);
				break;
			}
		}
		else
		{
			cur_shift_y += line_height;
			line_height = 0;
		}	
	}
}

Box::Box(float x, float y, float dx, float dy, sf::Color color_main): cur_shift_x1(0.f), cur_shift_x2(0.f), cur_shift_y(0.f)
{
	defaultstate.position.x = x;
	defaultstate.position.y = y;
	defaultstate.size.x = dx;
	defaultstate.size.y = dy;
	defaultstate.color_main = ToColorF(color_main);
	clone_states();
}

void UpdateAllObjects(sf::RenderWindow * window, sf::Vector2i mouse, bool RMB, bool LMB, bool all_keys[], float dt)
{
	for (auto &obj : global_objects)
	{
		obj.second->Update(window, mouse, RMB, LMB, all_keys, dt);
	}
}

ColorFloat::ColorFloat(float red, float green, float blue, float alpha): r(red), g(green), b(blue), a(alpha)
{

}

void ColorFloat::operator=(sf::Color a)
{
	*this = ToColorF(a);
}
