#include "FlosionUI.h"
#include "Font.h"

namespace fui {

	// Object
	Object::Object(){
		container = nullptr;
		size = {100, 100};
		bring_to_front = true;
	}
	Container* Object::getContainer(){
		return container;
	}


	// Container
	void Container::addObject(Object* obj){
		addChildWindow(obj);
		objects.push_back(obj);
		obj->container = this;
	}
	void Container::releaseObject(Object* obj){
		releaseChildWindow(obj);
		for (int i = 0; i < objects.size(); i++){
			if (objects[i] == obj){
				objects.erase(objects.begin() + i);
				break;
			}
		}
		obj->container = nullptr;
	}
	void Container::collapse(){
		// TODO
	}
	void Container::expand(){
		// TODO
	}
	void Container::render(sf::RenderWindow& rw){
		sf::Vertex vertices[] = {
			sf::Vertex({0, 0}, sf::Color(0x404040FF)),
			sf::Vertex({size.x, 0}, sf::Color(0x404040FF)),
			sf::Vertex({size.x, size.y}, sf::Color(0x202020FF)),
			sf::Vertex({0, size.y}, sf::Color(0x202020FF)),
		};
		rw.draw(vertices, 4, sf::PrimitiveType::Quads);
		renderChildWindows(rw);
	}
	void Container::onLeftClick(int clicks){
		if (clicks == 2){
			Menu* menu = new Menu(this);
			addChildWindow(menu);
			menu->pos = localMousePos() - menu->size * 0.5f;
			menu->beginTyping();
		}
	}
	Container::Menu::Menu(Container* _container){
		container = _container;
		size = {100, 20};
		addChildWindow(textentry = new TextField(this));
	}
	void Container::Menu::beginTyping(){
		textentry->grabFocus();
	}
	void Container::Menu::onLoseFocus(){
		close();
	}
	void Container::Menu::createObject(const std::string& str){
		Object* obj = Factory::createObject(str);
		if (!obj){
			if (listitems.size() > 0){
				obj = Factory::createObject(listitems.front()->str);
			} else {
				return;
			}
		}
		obj->pos = vec2(sf::Vector2i(pos + size * 0.5f - obj->size * 0.5f));
		container->addObject(obj);
	}
	void Container::Menu::render(sf::RenderWindow& rw){
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(0x404040FF));
		rect.setSize(size);
		rect.setOutlineColor(sf::Color(0x808080FF));
		rect.setOutlineThickness(1.0f);
		rw.draw(rect);
		renderChildWindows(rw);
	}
	void Container::Menu::refreshList(const std::string& text){
		for (ListItem* li : listitems){
			li->close();
		}
		listitems.clear();
		auto& objmap = Factory::getObjectMap();
		int y = size.y;
		int count = 0;
		const int max_items = 10;
		for (auto it = objmap.begin(); it != objmap.end() && count < max_items; it++){
			if (it->first.find(text) == 0){
				ListItem* li = new ListItem(this, it->first);
				listitems.push_back(li);
				addChildWindow(li);
				li->pos.y = y;
				y += li->size.y;
				count++;
			}
		}
	}
	Container::Menu::TextField::TextField(Menu* _menu) : TextEntry("", getFont(), 15, sf::Color(0xFFFFFFFF), sf::Color(0x0)){
		menu = _menu;
	}
	void Container::Menu::TextField::onType(const std::string& text){
		menu->refreshList(text);
	}
	void Container::Menu::TextField::onReturn(const std::string& text){
		menu->createObject(text);
		menu->close();
	}
	void Container::Menu::TextField::onKeyDown(sf::Keyboard::Key key){
		if (key == sf::Keyboard::Down){
			focusToNextWindow();
		} else if (key == sf::Keyboard::Up){
			focusToPreviousWindow();
		}
	}
	Container::Menu::ListItem::ListItem(Menu* _menu, const std::string& _str){
		menu = _menu;
		str = _str;
		size = {100, 20};
		addChildWindow(new ui::Text(str, getFont(), sf::Color(0xFFFFFFFF), 15));
	}
	void Container::Menu::ListItem::onLeftClick(int clicks){
		menu->createObject(str);
		menu->close();
	}
	void Container::Menu::ListItem::onKeyDown(sf::Keyboard::Key key){
		if (key == sf::Keyboard::Return){
			menu->createObject(str);
			menu->close();
		} else if (key == sf::Keyboard::Down){
			focusToNextWindow();
		} else if (key == sf::Keyboard::Up){
			focusToPreviousWindow();
		}
	}
	void Container::Menu::ListItem::render(sf::RenderWindow& rw){
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(isFocused() ? 0x808000FF : 0xFF));
		rect.setSize(size);
		rect.setOutlineColor(sf::Color(0x404040FF));
		rect.setOutlineThickness(1.0f);
		rw.draw(rect);
		renderChildWindows(rw);
	}

	// Master Container
	void MasterContainer::render(sf::RenderWindow& rw){
		pos = {0, 0};
		size = ui::getScreenSize();
		Container::render(rw);
	}


	// NumberInput
	NumberInput::NumberInput(musical::NumberInput* _target, Object* _parent, const std::string& _caption)
		: target(_target), owner_object(_parent) {
		size = {30, 30};
		owner_object->number_inputs.push_back(this);
		wire_in = nullptr;
		addChildWindow(caption = new ui::Text(_caption, fui::getFont()));
		caption->pos.x = -5 - caption->size.x;
		caption->pos.y = 0;
		hover_timestamp = ui::getProgramTime() - 1;
	}
	NumberInput::~NumberInput(){
		if (wire_in){
			wire_in->close();
		}
		for (auto it = owner_object->number_inputs.begin(); it != owner_object->number_inputs.end(); it++){
			if (*it == this){
				owner_object->number_inputs.erase(it);
				return;
			}
		}
	}
	void NumberInput::onHover(){
		hover_timestamp = ui::getProgramTime();
	}
	void NumberInput::render(sf::RenderWindow& rw){
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(0x4040FF80));
		rect.setSize(size);
		rect.setOutlineColor(sf::Color(0x000000FF));
		rect.setOutlineThickness(1);
		rw.draw(rect);
		if (ui::getProgramTime() - hover_timestamp < 0.25){
			caption->visible = true;
		} else {
			caption->visible = false;
		}
		renderChildWindows(rw);
	}
	void NumberInput::setWireIn(NumberWire* wire){
		if (wire_in){
			wire_in->ConnectHeadTo(nullptr);
		}
		wire_in = wire;
		if (wire && wire->dst != this){
			wire->ConnectHeadTo(this);
		}
	}
	void NumberInput::onDropDragWindow(Window* window) {
		if (NumberWire::Head* wirehead = dynamic_cast<NumberWire::Head*>(window)){
			if (wirehead->wire->safeToConnect(this)){
				wirehead->wire->ConnectHeadTo(this);
			} else {
				vec2 start = wirehead->pos;
				vec2 diff = wirehead->pos - wirehead->wire->tail->pos;
				vec2 end = vec2(sf::Vector2i(wirehead->pos - (diff * (float)(50 / hypot(diff.x, diff.y)))));

				wirehead->startTransition(0.25, [=](double x){
					wirehead->pos = start + (end - start) * (float)sin(x * PI / 2);
				});
			}
		}
	}
	void NumberInput::onHoverWithDrag(Window* win){
		if (dynamic_cast<NumberWire::Head*>(win)){
			hover_timestamp = ui::getProgramTime();
		}
	}
	void NumberInput::onLeftClick(int clicks){
		if (wire_in){
			wire_in->dragHead();
		} else {
			NumberWire* wire = new NumberWire;
			this->owner_object->getContainer()->addObject(wire);
			wire->ConnectHeadTo(this);
			wire->dragTail();
		}
	}


	// NumberOutput
	NumberOutput::NumberOutput(musical::NumberSource* _target, Object* _parent, const std::string& _caption)
		: target(_target), owner_object(_parent) {
		size = {30, 30};
		owner_object->number_outputs.push_back(this);
		addChildWindow(caption = new ui::Text(_caption, fui::getFont()));
		caption->pos.x = 0;
		caption->pos.y = -30;
		hover_timestamp = ui::getProgramTime() - 1;
	}
	NumberOutput::~NumberOutput(){
		while (wires_out.size() > 0){
			wires_out.back()->close();
		}
		for (auto it = owner_object->number_outputs.begin(); it != owner_object->number_outputs.end(); it++){
			if (*it == this){
				owner_object->number_outputs.erase(it);
				return;
			}
		}
	}
	void NumberOutput::render(sf::RenderWindow& rw){
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(0x00008080));
		rect.setSize(size);
		rect.setOutlineColor(sf::Color(0x000000FF));
		rect.setOutlineThickness(1);
		rw.draw(rect);
		if (ui::getProgramTime() - hover_timestamp < 0.25){
			caption->visible = true;
		} else {
			caption->visible = false;
		}
		renderChildWindows(rw);
	}
	void NumberOutput::onHover(){
		hover_timestamp = ui::getProgramTime();
	}
	void NumberOutput::addWireOut(NumberWire* wire){
		for (NumberWire* w : wires_out){
			if (w == wire){
				return;
			}
		}
		wires_out.push_back(wire);
		if (wire->src != this){
			wire->ConnectTailTo(this);
		}
	}
	void NumberOutput::removeWireOut(NumberWire* wire){
		for (auto it = wires_out.begin(); it != wires_out.end(); it++){
			if (*it == wire){
				wires_out.erase(it);
				wire->ConnectTailTo(nullptr);
				return;
			}
		}
	}
	void NumberOutput::onDropDragWindow(Window* window){
		if (NumberWire::Tail* wiretail = dynamic_cast<NumberWire::Tail*>(window)){
			if (wiretail->wire->safeToConnect(this)){
				wiretail->wire->ConnectTailTo(this);
			} else {
				vec2 start = wiretail->pos;
				vec2 diff = wiretail->pos - wiretail->wire->head->pos;
				vec2 end = vec2(sf::Vector2i(wiretail->pos - (diff * (float)(50 / hypot(diff.x, diff.y)))));

				wiretail->startTransition(0.25, [=](double x){
					wiretail->pos = start + (end - start) * (float)sin(x * PI / 2);
				});
			}
		}
	}
	void NumberOutput::onHoverWithDrag(Window* win){
		if (dynamic_cast<NumberWire::Tail*>(win)){
			hover_timestamp = ui::getProgramTime();
		}
	}
	void NumberOutput::onLeftClick(int clicks){
		NumberWire* wire = new NumberWire;
		this->owner_object->getContainer()->addObject(wire);
		wire->ConnectTailTo(this);
		wire->dragHead();
	}


	// NumberWire Head
	NumberWire::Head::Head(NumberWire* _wire){
		wire = _wire;
		size = {20, 20};
	}
	void NumberWire::Head::onLeftClick(int clicks){
		wire->ConnectHeadTo(nullptr);
		startDrag();
	}
	// NumberWire Tail
	NumberWire::Tail::Tail(NumberWire* _wire){
		wire = _wire;
		size = {20, 20};
	}
	void NumberWire::Tail::onLeftClick(int clicks){
		wire->ConnectTailTo(nullptr);
		startDrag();
	}
	// NumberWire
	NumberWire::NumberWire(){
		size = {0, 0};
		addChildWindow(head = new Head(this));
		addChildWindow(tail = new Tail(this));
		dst = nullptr;
		src = nullptr;
	}
	NumberWire::~NumberWire(){
		ConnectHeadTo(nullptr);
		ConnectTailTo(nullptr);
	}
	bool NumberWire::safeToConnect(NumberInput* input){
		if (!src){
			return true;
		}
		if (!input){
			return true;
		}
		return input->target->isSafeSource(src->target);
	}
	bool NumberWire::safeToConnect(NumberOutput* output){
		if (!dst){
			return true;
		}
		if (!output){
			return true;
		}
		return dst->target->isSafeSource(output->target);
	}
	void NumberWire::ConnectHeadTo(NumberInput* input){
		if (dst){
			NumberInput* temp = dst;
			dst = nullptr;
			temp->setWireIn(nullptr);
			temp->target->setSource(nullptr);
		}
		dst = input;
		if (dst){
			dst->setWireIn(this);
			if (src){
				dst->target->setSource(src->target);
			}
		}
	}
	void NumberWire::ConnectTailTo(NumberOutput* output){
		if (src){
			src->removeWireOut(this);
		}
		src = output;
		if (src){
			src->addWireOut(this);
			if (dst){
				dst->target->setSource(src->target);
			}
		} else {
			if (dst){
				dst->target->setSource(nullptr);
			}
		}
	}
	void NumberWire::render(sf::RenderWindow& rw){
		if (dst){
			head->pos = dst->absPos() - absPos();
		}
		if (src){
			tail->pos = src->absPos() - absPos();
		}

		sf::Vertex line[] = {
			sf::Vertex(head->pos + vec2(10, 10), sf::Color(0x0000FFFF)),
			sf::Vertex(tail->pos + vec2(10, 10), sf::Color(0xFF))
		};
		rw.draw(line, 2, sf::Lines);
		renderChildWindows(rw);
	}
	void NumberWire::dragHead(){
		ConnectHeadTo(nullptr);
		head->pos = ui::getMousePos() - this->absPos();
		head->startDrag();
	}
	void NumberWire::dragTail(){
		ConnectTailTo(nullptr);
		tail->pos = ui::getMousePos() - this->absPos();
		tail->startDrag();
	}


	// SoundInput
	SoundInput::SoundInput(musical::SoundInput* _target, Object* _parent)
		: target(_target), owner_object(_parent) {
		size = {30, 30};
		owner_object->sound_inputs.push_back(this);
		wire_in = nullptr;
	}
	SoundInput::~SoundInput(){
		for (auto it = owner_object->sound_inputs.begin(); it != owner_object->sound_inputs.end(); it++){
			if (*it == this){
				owner_object->sound_inputs.erase(it);
				return;
			}
		}
	}
	void SoundInput::render(sf::RenderWindow& rw){
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(0x00FF0080));
		rect.setSize(size);
		rect.setOutlineColor(sf::Color(0x000000FF));
		rect.setOutlineThickness(1);
		rw.draw(rect);
	}
	void SoundInput::setWireIn(SoundWire* wire){
		if (wire_in){
			wire_in->ConnectHeadTo(nullptr);
		}
		wire_in = wire;
		if (wire && wire->dst != this){
			wire->ConnectHeadTo(this);
		}
	}
	void SoundInput::onDropDragWindow(Window* window) {
		if (SoundWire::Head* wirehead = dynamic_cast<SoundWire::Head*>(window)){
			wirehead->wire->ConnectHeadTo(this);
		}
	}
	void SoundInput::onLeftClick(int clicks){
		if (wire_in){
			wire_in->dragHead();
		} else {
			SoundWire* wire = new SoundWire;
			this->owner_object->getContainer()->addObject(wire);
			wire->ConnectHeadTo(this);
			wire->dragTail();
		}
	}


	// SoundOutput
	SoundOutput::SoundOutput(musical::SoundSource* _target, Object* _parent)
		: target(_target), owner_object(_parent) {
		size = {30, 30};
		owner_object->sound_outputs.push_back(this);
	}
	SoundOutput::~SoundOutput(){
		for (auto it = owner_object->sound_outputs.begin(); it != owner_object->sound_outputs.end(); it++){
			if (*it == this){
				owner_object->sound_outputs.erase(it);
				return;
			}
		}
	}
	void SoundOutput::render(sf::RenderWindow& rw){
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(0x00800080));
		rect.setSize(size);
		rect.setOutlineColor(sf::Color(0x000000FF));
		rect.setOutlineThickness(1);
		rw.draw(rect);
	}
	void SoundOutput::addWireOut(SoundWire* wire){
		for (SoundWire* w : wires_out){
			if (w == wire){
				return;
			}
		}
		wires_out.push_back(wire);
		if (wire->src != this){
			wire->ConnectTailTo(this);
		}
	}
	void SoundOutput::removeWireOut(SoundWire* wire){
		for (auto it = wires_out.begin(); it != wires_out.end(); it++){
			if (*it == wire){
				wires_out.erase(it);
				wire->ConnectTailTo(nullptr);
				return;
			}
		}
	}
	void SoundOutput::onDropDragWindow(Window* window){
		if (SoundWire::Tail* wiretail = dynamic_cast<SoundWire::Tail*>(window)){
			wiretail->wire->ConnectTailTo(this);
		}
	}
	void SoundOutput::onLeftClick(int clicks){
		SoundWire* wire = new SoundWire;
		this->owner_object->getContainer()->addObject(wire);
		wire->ConnectTailTo(this);
		wire->dragHead();
	}


	// SoundWire Head
	SoundWire::Head::Head(SoundWire* _wire){
		wire = _wire;
		size = {20, 20};
	}
	void SoundWire::Head::onLeftClick(int clicks){
		wire->ConnectHeadTo(nullptr);
		startDrag();
	}
	// SoundWire Tail
	SoundWire::Tail::Tail(SoundWire* _wire){
		wire = _wire;
		size = {20, 20};
	}
	void SoundWire::Tail::onLeftClick(int clicks){
		wire->ConnectTailTo(nullptr);
		startDrag();
	}
	// SoundWire
	SoundWire::SoundWire(){
		size = {0, 0};
		addChildWindow(head = new Head(this));
		addChildWindow(tail = new Tail(this));
		dst = nullptr;
		src = nullptr;
	}
	void SoundWire::ConnectHeadTo(SoundInput* input){
		if (dst){
			SoundInput* temp = dst;
			dst = nullptr;
			temp->setWireIn(nullptr);
			temp->target->setSource(nullptr);
		}
		if (input){
			dst = input;
			input->setWireIn(this);
			if (src){
				input->target->setSource(src->target);
			}
		}
	}
	void SoundWire::ConnectTailTo(SoundOutput* output){
		if (src){
			src->removeWireOut(this);
		}

		if (output){
			output->addWireOut(this);
			if (dst){
				dst->target->setSource(output->target);
			}
		} else {
			if (dst){
				dst->target->setSource(nullptr);
			}
		}

		src = output;
	}
	void SoundWire::render(sf::RenderWindow& rw){
		if (dst){
			head->pos = dst->absPos() - absPos();
		}
		if (src){
			tail->pos = src->absPos() - absPos();
		}

		sf::Vertex line[] = {
			sf::Vertex(head->pos + vec2(10, 10), sf::Color(0x00FF00FF)),
			sf::Vertex(tail->pos + vec2(10, 10), sf::Color(0xFF))
		};
		rw.draw(line, 2, sf::Lines);
		renderChildWindows(rw);
	}
	void SoundWire::dragHead(){
		ConnectHeadTo(nullptr);
		head->pos = ui::getMousePos() - this->absPos();
		head->startDrag();
	}
	void SoundWire::dragTail(){
		ConnectTailTo(nullptr);
		tail->pos = ui::getMousePos() - this->absPos();
		tail->startDrag();
	}


	// ProcessingObject
	void ProcessingObject::onRightClick(int clicks){
		startDrag();
	}
	void ProcessingObject::onDropDragWindow(Window* window){
		if (NumberWire::Head* wirehead = dynamic_cast<NumberWire::Head*>(window)){
			// TODO: show all the number inputs
			return;
		}
		if (NumberWire::Tail* wiretail = dynamic_cast<NumberWire::Tail*>(window)){
			// TODO: show all the number outputs
			return;
		}
		if (SoundWire::Head* wirehead = dynamic_cast<SoundWire::Head*>(window)){
			// TODO: show all the sound inputs
			return;
		}
		if (SoundWire::Tail* wiretail = dynamic_cast<SoundWire::Tail*>(window)){
			// TODO: show all the sound outputs
			return;
		}
	}


	// factory for creating Objects from strings
	Object* Factory::createObject(const std::string& name){
		auto it = getObjectMap().find(name);
		if (it == getObjectMap().end()){
			return nullptr;
		} else {
			return it->second();
		}
	}
	std::map<std::string, std::function<Object*()>>& Factory::getObjectMap(){
		static std::map<std::string, std::function<Object*()>> objmap;
		return objmap;
	}

}