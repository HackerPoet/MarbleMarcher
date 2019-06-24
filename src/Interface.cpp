#include <Interface.h>

std::map<int, Object*> global_objects;
int obj_id = 0;
float animation_sharpness = 4.f;


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
	C.margin = a.margin*(1.f - t) + b.margin*t;
	C.scroll = a.scroll*(1.f - t) + b.scroll*t;
	C.font_size = a.font_size*(1.f - t) + b.font_size*t;
	C.color_main = a.color_main*(1.f - t) + b.color_main*t;
	C.color_second = a.color_second*(1.f - t) + b.color_second*t;
	C.color_border = a.color_border*(1.f - t) + b.color_border*t;
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

void Object::SetMargin(float x)
{
	defaultstate.margin = x;
	activestate.margin = x;
	hoverstate.margin = x;
}

void Object::SetScroll(float x)
{
	defaultstate.scroll = x;
	activestate.scroll = x;
	hoverstate.scroll = x;
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

void Object::KeyboardAction(bool all_keys[])
{
}

void Object::Draw(sf::RenderWindow * window)
{
	//nothing to draw
}

void Object::Update(sf::RenderWindow * window, sf::Vector2i mouse, bool RMB, bool LMB, bool all_keys[], float dt)
{
	window->setView(used_view);
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

	KeyboardAction(all_keys);
	Draw(window);
}

Object::Object() : callback(NULL), hoverfn(NULL), curmode(DEFAULT)
{
	used_view = sf::View(sf::FloatRect(0, 0, 1600, 900));
	id = obj_id;
	obj_id++;
	global_objects[id] = this;//add it to the global list
}

Object::~Object()
{
	global_objects.erase(id);
}

void Box::AddObject(Object * something, Allign a)
{
	objects[something->id] = something;
	object_alligns[something->id] = a;
	object_ids.push_back(something->id);
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

	sf::Vector2f default_size = sf::Vector2f(1600, 900);
	boxView.reset(sf::FloatRect(curstate.position, curstate.size));
	boxView.setViewport(sf::FloatRect(curstate.position.x/ default_size.x, curstate.position.y / default_size.y, curstate.size.x / default_size.x, curstate.size.y / default_size.y));

	float line_height = 0;
	float cur_shift_x1 = curstate.margin, cur_shift_x2 = curstate.margin;
	float cur_shift_y = curstate.margin + curstate.scroll;

	//update all the stuff inside the box
	for (auto &obj : objects)
	{
		Allign A = object_alligns[obj.first];
		bool not_placed = true; 
		int tries = 0;
		obj.second->used_view = boxView;
		line_height = std::max(obj.second->curstate.size.y, line_height);
		while (not_placed && tries < 3) //try to place the object somewhere
		{
			float space_left = defaultstate.size.x - cur_shift_x1 - cur_shift_x2;
			float obj_width = obj.second->curstate.size.x;
			
			if (space_left > obj_width)
			{
				not_placed = false;
				switch (A)
				{
				case LEFT:
					obj.second->SetPosition(curstate.position.x + cur_shift_x1, curstate.position.y + cur_shift_y);
					cur_shift_x1 += obj_width + curstate.margin;
					break;
				case CENTER:
					obj.second->SetPosition(curstate.position.x + defaultstate.size.x * 0.5f - obj_width * 0.5f, curstate.position.y + cur_shift_y);
					cur_shift_y += line_height + curstate.margin;
					line_height = 0;
					cur_shift_x1 = curstate.margin;
					cur_shift_x2 = curstate.margin;
					break;
				case RIGHT:
					obj.second->SetPosition(curstate.position.x + defaultstate.size.x - obj_width - cur_shift_x2, curstate.position.y + cur_shift_y);
					cur_shift_x2 += obj_width + curstate.margin;
					break;
				}
			}
			else
			{
				cur_shift_y += line_height + curstate.margin;
				line_height = 0;
				cur_shift_x1 = curstate.margin;
				cur_shift_x2 = curstate.margin;
				tries++;
			}
		}
	}
}

Box::Box(float x, float y, float dx, float dy, sf::Color color_main)
{
	defaultstate.position.x = x;
	defaultstate.position.y = y;
	defaultstate.size.x = dx;
	defaultstate.size.y = dy;
	defaultstate.color_main = ToColorF(color_main);
	clone_states();
}

Box::Box()
{

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

void Text::Draw(sf::RenderWindow * window)
{
	text.setPosition(curstate.position);
	text.setCharacterSize(curstate.font_size);
	text.setFillColor(ToColor(curstate.color_main));
	text.setOutlineThickness(curstate.border_thickness);
	text.setOutlineColor(ToColor(curstate.color_border));

	window->draw(text);

	SetSize(text.getLocalBounds().width, text.getLocalBounds().height);
}

Text::Text(std::string str, sf::Font & f, float size, sf::Color col)
{
	text.setString(str);
	text.setFont(f);
	defaultstate.font_size = size;
	defaultstate.color_main = col;

	clone_states();
}

Text::Text(sf::Text t)
{
	text = t;
	defaultstate.font_size = t.getCharacterSize();
	defaultstate.color_main = t.getFillColor();
	clone_states();
}

Window::Window(float x, float y, float dx, float dy, sf::Color color_main, sf::Text title)
{
	defaultstate.position.x = x;
	defaultstate.position.y = y;
	defaultstate.size.x = dx;
	defaultstate.size.y = dy;
	defaultstate.color_main = ToColorF(color_main);
	clone_states();

	Bar.reset(new Box(0, 0, dx, 30, sf::Color(0, 100, 200, 128)));
	Inside.reset(new Box(0, 0, dx-30, dy-30, sf::Color(0, 100, 100, 128))); 
	Scroll.reset(new Box(0, 0, 30, dy - 30, sf::Color(0, 100, 100, 128)));
	Scroll_Slide.reset(new Box(0, 0, 30, 30, sf::Color(200, 100, 0, 128)));
	Title.reset(new Text(title));

	this->AddObject(Bar.get(), Box::CENTER);
	this->AddObject(Inside.get(), Box::LEFT);
	this->AddObject(Scroll.get(), Box::RIGHT);
	Bar.get()->AddObject(Title.get(), Box::LEFT);
	Scroll.get()->AddObject(Scroll_Slide.get(), Box::CENTER);

	//use lambda funtion
	Scroll_Slide.get()->SetCallbackFunction();
}
