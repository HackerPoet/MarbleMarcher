#include <Interface.h>

std::map<int, Object*> global_objects;
int obj_id = 0;
float animation_sharpness = 4.f;


sf::FloatRect overlap(sf::FloatRect a, sf::FloatRect b)
{
	sf::FloatRect c;
	c.top = std::max(a.top, b.top);
	c.left = std::max(a.left, b.left);
	c.height = std::max(std::min(a.top + a.height, b.top + b.height) - c.top, 0.f);
	c.width = std::max(std::min(a.left + a.width, b.left + b.width) - c.left, 0.f);

	return c;
}

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
	defaultstate.border_thickness = S;
	activestate.border_thickness = S;
	hoverstate.border_thickness = S;
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

void Object::Move(sf::Vector2f dx)
{
	defaultstate.position += dx;
	curstate.position += dx;
	activestate.position += dx;
	hoverstate.position += dx;
}

void Object::SetDefaultFunction(std::function<void(sf::RenderWindow * window, InputState&state)> fun)
{
	defaultfn = fun;
}

void Object::SetCallbackFunction(std::function<void(sf::RenderWindow * window, InputState&state)> fun)
{
	callback = fun;
}

void Object::SetHoverFunction(std::function<void(sf::RenderWindow * window, InputState&state)> fun)
{
	hoverfn = fun;
}

void Object::clone_states()
{
	curstate = defaultstate;
	activestate = defaultstate;
	hoverstate = defaultstate;
}

void Object::Draw(sf::RenderWindow * window, InputState& state)
{
	//nothing to draw
}

void Object::Update(sf::RenderWindow * window, InputState& state)
{
	window->setView(used_view);
	//callback stuff
	sf::Vector2f worldPos = window->mapPixelToCoords(sf::Vector2i(state.mouse_pos.x, state.mouse_pos.y));
	sf::FloatRect obj(curstate.position.x, curstate.position.y, curstate.size.x, curstate.size.y);

	state.mouse_speed = window->mapPixelToCoords(sf::Vector2i(state.mouse_pos.x, state.mouse_pos.y)) -
						window->mapPixelToCoords(sf::Vector2i(state.mouse_prev.x, state.mouse_prev.y));

	curmode = DEFAULT;
	//if mouse is inside the object 
	if (obj.contains(worldPos))
	{
		if (state.mouse[0] || state.mouse[2]) //if click
		{
			if (callback != NULL)
				callback(window, state); //run callback with state info
			curmode = ACTIVE;
		}
		else // if hover
		{
			if (hoverfn != NULL)
				hoverfn(window, state); //run callback with state info
			curmode = ONHOVER;
		}
	}
	else
	{
		if (defaultfn != NULL)
			defaultfn(window, state); //run callback with state info
		curmode = DEFAULT;
	}

	float t = 1.f - exp(-animation_sharpness * state.dt);

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

	Draw(window, state);
}


Object::Object() : callback(NULL), hoverfn(NULL), defaultfn(NULL), curmode(DEFAULT)
{
	used_view = sf::View(sf::FloatRect(0, 0, 1600, 900));
}

Object::~Object()
{
	global_objects.erase(id);
}

void Box::AddObject(Object * something, Allign a)
{
	objects.push_back(std::pair<Allign,Object*>(a,something));
}

void Box::Draw(sf::RenderWindow * window, InputState& state)
{
	//update the box itself
	rect.setPosition(curstate.position);
	rect.setSize(curstate.size);
	rect.setFillColor(ToColor(curstate.color_main));
	rect.setOutlineThickness(curstate.border_thickness);
	rect.setOutlineColor(ToColor(curstate.color_border));
	window->draw(rect);
	
	sf::Vector2f default_size = sf::Vector2f(1600, 900);
	sf::FloatRect global_view = sf::FloatRect(window->getView().getCenter() - window->getView().getSize()*0.5f, window->getView().getSize());
	sf::FloatRect this_view = overlap(global_view, sf::FloatRect(curstate.position, curstate.size));
	boxView.reset(this_view);
	sf::FloatRect global_viewport = window->getView().getViewport();
	sf::FloatRect local_viewport = sf::FloatRect(curstate.position.x / default_size.x, curstate.position.y / default_size.y, curstate.size.x / default_size.x, curstate.size.y / default_size.y);
	sf::FloatRect this_viewport = overlap(global_viewport, local_viewport);
	boxView.setViewport(this_viewport);

	if (this_viewport.width > 0.f && this_viewport.height > 0.f)
	{
		float line_height = 0;
		float cur_shift_x1 = curstate.margin, cur_shift_x2 = curstate.margin;
		float cur_shift_y = curstate.margin + curstate.scroll;

		//update all the stuff inside the box
		for (auto &obj : objects)
		{
			Allign A = obj.first;
			bool not_placed = true;
			int tries = 0;
			obj.second->used_view = boxView;
			line_height = std::max(obj.second->curstate.size.y, line_height);
			while (not_placed && tries < 3) //try to place the object somewhere
			{
				float space_left = defaultstate.size.x - cur_shift_x1 - cur_shift_x2;
				float obj_width = obj.second->curstate.size.x;

				if (space_left >= obj_width)
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

			obj.second->Update(window, state);
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

void UpdateAllObjects(sf::RenderWindow * window, InputState& state)
{
	for (auto &obj : global_objects)
	{
		obj.second->Update(window, state);
	}
}

ColorFloat::ColorFloat(float red, float green, float blue, float alpha): r(red), g(green), b(blue), a(alpha)
{

}

void ColorFloat::operator=(sf::Color a)
{
	*this = ToColorF(a);
}

void Text::Draw(sf::RenderWindow * window, InputState& state)
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

void Window::Add(Object * something, Allign a)
{
	Inside.get()->AddObject(something, a);
}

Window::Window(float x, float y, float dx, float dy, sf::Color color_main, std::string title, sf::Font & font)
{
	defaultstate.position.x = x;
	defaultstate.position.y = y;
	defaultstate.size.x = dx;
	defaultstate.size.y = dy;
	defaultstate.color_main = ToColorF(color_main);
	clone_states();

	Bar.reset(new Box(0, 0, dx, 30, sf::Color(0, 100, 200, 128)));
	Scroll.reset(new Box(0, 0, 30, dy - 30, sf::Color(150, 150, 150, 128)));
	Scroll_Slide.reset(new Box(0, 0, 27, 60, sf::Color(255, 150, 0, 128)));
	Scroll_Slide.get()->hoverstate.color_main = sf::Color(255, 50, 0, 128);
	Scroll_Slide.get()->activestate.color_main = sf::Color(255, 100, 100, 255);
	Inside.reset(new Box(0, 0, dx - 30, dy - 30, sf::Color(100, 100, 100, 128)));
	Title.reset(new Text(title, font, 25, sf::Color::White));

	this->AddObject(Bar.get(), Box::CENTER);
	this->AddObject(Inside.get(), Box::LEFT);
	this->AddObject(Scroll.get(), Box::RIGHT);
	Bar.get()->AddObject(Title.get(), Box::LEFT);
	Scroll.get()->AddObject(Scroll_Slide.get(), Box::CENTER);

	//use lambda funtion
	Scroll_Slide.get()->SetCallbackFunction([parent = this](sf::RenderWindow * window, InputState & state)
	{
		float scroll_a = parent->Scroll.get()->defaultstate.scroll + state.mouse_speed.y;
		float scroll_b = parent->Inside.get()->defaultstate.scroll - state.mouse_speed.y;
		parent->Inside.get()->SetScroll(scroll_b);
		parent->Scroll.get()->SetScroll(scroll_a);
	});

	//drag callback
	Bar.get()->SetCallbackFunction([parent = this](sf::RenderWindow * window, InputState & state)
	{
		parent->Move(state.mouse_speed);
	});
	    
	this->SetHoverFunction([parent = this](sf::RenderWindow * window, InputState & state)
	{
		//wheel scroll 
		if (state.wheel != 0.f)
		{
			float ds = 20.f;
			float scroll_a = parent->Scroll.get()->defaultstate.scroll + state.wheel*ds;
			float scroll_b = parent->Inside.get()->defaultstate.scroll - state.wheel*ds;
			parent->Inside.get()->SetScroll(scroll_b);
			parent->Scroll.get()->SetScroll(scroll_a);
		}
	});

	//add this to the global map to update it automatically
	id = obj_id;
	obj_id++;
	global_objects[id] = this;//add it to the global list
}

InputState::InputState(): mouse_pos(sf::Vector2f(0,0)), mouse_speed(sf::Vector2f(0, 0))
{
	std::fill(keys, keys + sf::Keyboard::KeyCount - 1, false);
	std::fill(mouse, mouse + 2, false);
}

InputState::InputState(bool a[sf::Keyboard::KeyCount], bool b[3], sf::Vector2f c, sf::Vector2f d):
	mouse_pos(c), mouse_speed(d)
{
	std::copy(a, a + sf::Keyboard::KeyCount - 1, keys);
	std::copy(b, b + 2, mouse);
}
