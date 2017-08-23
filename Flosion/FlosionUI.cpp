#include "FlosionUI.h"
#include "Font.h"

namespace fui {

	// Object
	Object::Object(){
		container = nullptr;
		size = {100, 100};
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
	void Container::render(sf::RenderWindow& renderwin){
		sf::RectangleShape rect;
		rect.setSize(size);
		rect.setFillColor(sf::Color(0xC0C0C0FF));
		renderwin.draw(rect);
		renderChildWindows(renderwin);
	}
	void Container::onLeftClick(int clicks){
		if (clicks == 2){
			// TODO
		}
	}

	// Master Container
	void MasterContainer::render(sf::RenderWindow& renderwin){
		pos = {0, 0};
		size = ui::getScreenSize();

		sf::RectangleShape rect;
		rect.setSize(size);
		rect.setFillColor(sf::Color(0x808080FF));
		renderwin.draw(rect);
		renderChildWindows(renderwin);
	}


	// NumberInput
	NumberInput::NumberInput(musical::NumberInput* _target, Object* _parent, const std::string& _caption, sf::Vector2f _pos){
		pos = _pos;
		size = {30, 30};
		target = _target;
		parent = _parent;
		wire_in = nullptr;
		addChildWindow(caption = new ui::Text(_caption, fui::getFont()));
		caption->pos.x = -5 - caption->size.x;
		caption->pos.y = 0;
		hover_timestamp = ui::getProgramTime() - 1;
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
				sf::Vector2f start = wirehead->pos;
				sf::Vector2f diff = wirehead->pos - wirehead->wire->tail->pos;
				sf::Vector2f end = sf::Vector2f(sf::Vector2i(wirehead->pos - (diff * (float)(50 / hypot(diff.x, diff.y)))));

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
			this->parent->getContainer()->addObject(wire);
			wire->ConnectHeadTo(this);
			wire->dragTail();
		}
	}


	// NumberOutput
	NumberOutput::NumberOutput(musical::NumberSource* _target, Object* _parent, const std::string& _caption, sf::Vector2f _pos){
		pos = _pos;
		size = {30, 30};
		target = _target;
		parent = _parent;
		addChildWindow(caption = new ui::Text(_caption, fui::getFont()));
		caption->pos.x = 0;
		caption->pos.y = -30;
		hover_timestamp = ui::getProgramTime() - 1;
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
				sf::Vector2f start = wiretail->pos;
				sf::Vector2f diff = wiretail->pos - wiretail->wire->head->pos;
				sf::Vector2f end = sf::Vector2f(sf::Vector2i(wiretail->pos - (diff * (float)(50 / hypot(diff.x, diff.y)))));

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
		this->parent->getContainer()->addObject(wire);
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
		if (input){
			dst = input;
			input->setWireIn(this);
			if (src){
				input->target->setSource(src->target);
			}
		}
	}
	void NumberWire::ConnectTailTo(NumberOutput* output){
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
	void NumberWire::render(sf::RenderWindow& rw){
		if (dst){
			head->pos = dst->absPos() - absPos();
		}
		if (src){
			tail->pos = src->absPos() - absPos();
		}

		sf::Vertex line[] = {
			sf::Vertex(head->pos + sf::Vector2f(10, 10), sf::Color(0x0000FFFF)),
			sf::Vertex(tail->pos + sf::Vector2f(10, 10), sf::Color(0xFF))
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
	SoundInput::SoundInput(musical::SoundInput* _target, Object* _parent, sf::Vector2f _pos){
		pos = _pos;
		size = {30, 30};
		target = _target;
		parent = _parent;
		wire_in = nullptr;
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
			this->parent->getContainer()->addObject(wire);
			wire->ConnectHeadTo(this);
			wire->dragTail();
		}
	}


	// SoundOutput
	SoundOutput::SoundOutput(musical::SoundSource* _target, Object* _parent, sf::Vector2f _pos){
		pos = _pos;
		size = {30, 30};
		target = _target;
		parent = _parent;
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
		this->parent->getContainer()->addObject(wire);
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
			sf::Vertex(head->pos + sf::Vector2f(10, 10), sf::Color(0x00FF00FF)),
			sf::Vertex(tail->pos + sf::Vector2f(10, 10), sf::Color(0xFF))
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
	void ProcessingObject::addSoundInput(SoundInput* si){
		for (auto it = sound_inputs.begin(); it != sound_inputs.end(); it++){
			if (*it == si){
				throw std::runtime_error("The SoundInput has already been added");
			}
		}
		sound_inputs.push_back(si);
		addChildWindow(si);
	}
	void ProcessingObject::removeSoundInput(SoundInput* si){
		for (auto it = sound_inputs.begin(); it != sound_inputs.end(); it++){
			if (*it == si){
				sound_inputs.erase(it);
				return;
			}
		}
		throw std::runtime_error("The SoundInput could not be found.");
		releaseChildWindow(si);
	}
	void ProcessingObject::addSoundOutput(SoundOutput* so){
		for (auto it = sound_outputs.begin(); it != sound_outputs.end(); it++){
			if (*it == so){
				throw std::runtime_error("The SoundOutput has already been added");
			}
		}
		sound_outputs.push_back(so);
		addChildWindow(so);
	}
	void ProcessingObject::removeSoundOutput(SoundOutput* so){
		for (auto it = sound_outputs.begin(); it != sound_outputs.end(); it++){
			if (*it == so){
				sound_outputs.erase(it);
				return;
			}
		}
		throw std::runtime_error("The SoundOutput could not be found.");
		releaseChildWindow(so);
	}
	void ProcessingObject::addNumberInput(NumberInput* ni){
		for (auto it = number_inputs.begin(); it != number_inputs.end(); it++){
			if (*it == ni){
				throw std::runtime_error("The NumberInput has already been added");
			}
		}
		number_inputs.push_back(ni);
		addChildWindow(ni);
	}
	void ProcessingObject::removeNumberInput(NumberInput* ni){
		for (auto it = number_inputs.begin(); it != number_inputs.end(); it++){
			if (*it == ni){
				number_inputs.erase(it);
				return;
			}
		}
		throw std::runtime_error("The NumberInput could not be found.");
		releaseChildWindow(ni);
	}
	void ProcessingObject::addNumberOutput(NumberOutput* no){
		for (auto it = number_outputs.begin(); it != number_outputs.end(); it++){
			if (*it == no){
				throw std::runtime_error("The NumberOutput has already been added");
			}
		}
		number_outputs.push_back(no);
		addChildWindow(no);
	}
	void ProcessingObject::removeNumberOutput(NumberOutput* no){
		for (auto it = number_outputs.begin(); it != number_outputs.end(); it++){
			if (*it == no){
				number_outputs.erase(it);
				return;
			}
		}
		throw std::runtime_error("The NumberOutput could not be found.");
		releaseChildWindow(no);
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