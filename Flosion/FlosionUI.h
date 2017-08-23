#pragma once
#include "gui.h"
#include "NumberInput.h"

#define fuiRegisterObject(type, ...) \
namespace { \
	::fui::Factory::RegisterObject<type> register_object_##type({##__VA_ARGS__}); \
}

// TODO: reorganize this file into appropriate .h and .cpp files so I can breathe

namespace fui {

	struct Object;
	struct Container;
	struct MasterContainer;

	struct Object;

	struct NumberInput;
	struct NumberOutput;

	struct SoundInput;
	struct SoundOutput;

	struct NumberWire;
	struct SoundWire;

	const sf::Font& getFont(){
		static bool init;
		static sf::Font font;
		if (!init){
			font.loadFromFile("fonts/JosefinSans-Bold.ttf");
			init = true;
		}
		return font;
	}

	// Object is the base class to all gui components representing sound and
	// numerical processing, and their related components
	struct Object : ui::Window {
		Object(){
			container = nullptr;
			size = {100, 100};
		}

		Container* getContainer(){
			return container;
		}

		private:
		// the FGUI container that holds the current object
		// on if this is the master container should this ever be null
		Container* container;

		friend struct Container;
	};

	// Container is what holds all objects
	// allows convenient clustering of objects and collapsing into a more compact view
	// TODO:
	// Enable selections
	struct Container : Object {

		// add a new object
		void addObject(Object* obj){
			addChildWindow(obj);
			objects.push_back(obj);
			obj->container = this;
		}
		// remove an object, without destroying
		void releaseObject(Object* obj){
			releaseChildWindow(obj);
			for (int i = 0; i < objects.size(); i++){
				if (objects[i] == obj){
					objects.erase(objects.begin() + i);
					break;
				}
			}
			obj->container = nullptr;
		}

		// collapse the container such that its contained objects aren't rendered
		// and it takes up much less screen space
		void collapse(){
			std::cout << "time to collapse!\n";
		}
		// expand the container to show its contents
		void expand(){
			std::cout << "time to expand!\n";
		}

		void render(sf::RenderWindow& renderwin) override {
			sf::RectangleShape rect;
			rect.setSize(size);
			rect.setFillColor(sf::Color(0xC0C0C0FF));
			renderwin.draw(rect);
			renderChildWindows(renderwin);
		}

		void onLeftClick(int clicks) override {
			if (clicks == 2){
				std::cout << "Time to open a menu\n";
			}
		}

		private:
		std::vector<Object*> objects;

		friend struct Object;
	};

	// MasterContainer is the singleton-intended container that holds everything in the current work area
	struct MasterContainer : Container {

		void render(sf::RenderWindow& renderwin) override {
			pos = {0, 0};
			size = ui::getScreenSize();

			sf::RectangleShape rect;
			rect.setSize(size);
			rect.setFillColor(sf::Color(0x808080FF));
			renderwin.draw(rect);
			renderChildWindows(renderwin);
		}
	};

	struct Factory {
		template<class ObjectType>
		struct RegisterObject {
			RegisterObject(std::vector<std::string> names){
				for (auto name : names){
					objmap[name] = instantiate<ObjectType>;
				}
			}
		};

		static Object* createObject(const std::string& name){
			auto it = objmap.find(name);
			if (it == objmap.end()){
				return nullptr;
			} else {
				return it->second();
			}
		}

		private:
		Factory() = delete;

		static std::map<std::string, Object*(*)(void)> objmap;

		template<class ObjectType>
		static Object* instantiate(){
			return new ObjectType();
		}
	};
	std::map<std::string, Object*(*)(void)> Factory::objmap;

	// Object is the base class to all further-defined sound and number processing objects
	// defines functionality for adding/removing sound/number inputs/outputs
	// each sound/number input/output must be added when it is desired to be used in the current project
	// either in the constructor or dynamically
	// 
	struct ProcessingObject : Object {

		void onRightClick(int clicks) override {
			startDrag();
		}

		void onDropDragWindow(Window* window) override;

		void addSoundInput(SoundInput* si);
		void removeSoundInput(SoundInput* si);

		void addSoundOutput(SoundOutput* so);
		void removeSoundOutput(SoundOutput* so);

		void addNumberInput(NumberInput* ni);
		void removeNumberInput(NumberInput* ni);

		void addNumberOutput(NumberOutput* no);
		void removeNumberOutput(NumberOutput* no);

		private:
		std::vector<SoundInput*> sound_inputs;
		std::vector<SoundOutput*> sound_outputs;
		std::vector<NumberInput*> number_inputs;
		std::vector<NumberOutput*> number_outputs;
	};

	// the destination for a numberwire
	// corresponds to the input of a processing object
	struct NumberInput : ui::Window {
		NumberInput(musical::NumberInput* _target, Object* _parent, const std::string& _caption = "", sf::Vector2f _pos = {0,0}){
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

		void setWireIn(NumberWire* wire);

		void onDropDragWindow(Window* window) override;

		void onHover() override {
			hover_timestamp = ui::getProgramTime();
		}
		void onHoverWithDrag(Window* win) override;

		void render(sf::RenderWindow& rw) override {
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

		void onLeftClick(int clicks) override;

		private:
		Object* parent;
		musical::NumberInput* target;
		NumberWire* wire_in;
		ui::Text* caption;
		double hover_timestamp;

		friend struct NumberWire;
	};

	// the source of a numberwire
	// corresponds to a number source
	struct NumberOutput : ui::Window {
		NumberOutput(musical::NumberSource* _target, Object* _parent, const std::string& _caption = "", sf::Vector2f _pos = {0,0}){
			pos = _pos;
			size = {30, 30};
			target = _target;
			parent = _parent;
			addChildWindow(caption = new ui::Text(_caption, fui::getFont()));
			caption->pos.x = 0;
			caption->pos.y = -30;
			hover_timestamp = ui::getProgramTime() - 1;
		}

		void addWireOut(NumberWire* wire);
		void removeWireOut(NumberWire* wire);

		void render(sf::RenderWindow& rw) override {
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

		void onDropDragWindow(Window* window) override;

		void onHover() override {
			hover_timestamp = ui::getProgramTime();
		}
		void onHoverWithDrag(Window* win) override;

		void onLeftClick(int clicks) override;

		private:
		Object* parent;
		musical::NumberSource* target;
		std::vector<NumberWire*> wires_out;
		ui::Text* caption;
		double hover_timestamp;

		friend struct NumberWire;
	};

	// the destination for a sound wire
	// corresponds to a sound input of a processing object
	struct SoundInput : ui::Window {
		SoundInput(musical::SoundInput* _target, Object* _parent, sf::Vector2f _pos = {0,0}){
			pos = _pos;
			size = {30, 30};
			target = _target;
			parent = _parent;
			wire_in = nullptr;
		}

		void setWireIn(SoundWire* wire);

		void onDropDragWindow(Window* window) override;

		void render(sf::RenderWindow& rw) override {
			sf::RectangleShape rect;
			rect.setFillColor(sf::Color(0x00FF0080));
			rect.setSize(size);
			rect.setOutlineColor(sf::Color(0x000000FF));
			rect.setOutlineThickness(1);
			rw.draw(rect);
		}

		void onLeftClick(int clicks) override;

		private:
		Object* parent;
		musical::SoundInput* target;
		SoundWire* wire_in;

		friend struct SoundWire;
	};

	// the source of a sound wire
	// corresponds to a sound source
	struct SoundOutput : ui::Window {
		SoundOutput(musical::SoundSource* _target, Object* _parent, sf::Vector2f _pos = {0,0}){
			pos = _pos;
			size = {30, 30};
			target = _target;
			parent = _parent;
		}

		void addWireOut(SoundWire* wire);
		void removeWireOut(SoundWire* wire);

		void render(sf::RenderWindow& rw) override {
			sf::RectangleShape rect;
			rect.setFillColor(sf::Color(0x00800080));
			rect.setSize(size);
			rect.setOutlineColor(sf::Color(0x000000FF));
			rect.setOutlineThickness(1);
			rw.draw(rect);
		}

		void onDropDragWindow(Window* window) override;

		void onLeftClick(int clicks) override;

		private:
		Object* parent;
		musical::SoundSource* target;
		std::vector<SoundWire*> wires_out;

		friend struct SoundWire;
	};

	// NumberWire connects a NumberInput to a NumberOutput
	struct NumberWire : Object {
		struct Head : ui::Window {
			Head(NumberWire* _wire){
				wire = _wire;
				size = {20, 20};
			}
			void onLeftClick(int clicks){
				wire->ConnectHeadTo(nullptr);
				startDrag();
			}
			NumberWire* wire;
		};
		struct Tail : ui::Window {
			Tail(NumberWire* _wire){
				wire = _wire;
				size = {20, 20};
			}
			void onLeftClick(int clicks){
				wire->ConnectTailTo(nullptr);
				startDrag();
			}
			NumberWire* wire;
		};

		NumberWire(){
			size = {0, 0};
			addChildWindow(head = new Head(this));
			addChildWindow(tail = new Tail(this));
			dst = nullptr;
			src = nullptr;
		}

		bool safeToConnect(NumberInput* input){
			if (!src){
				return true;
			}
			if (!input){
				return true;
			}
			return input->target->isSafeSource(src->target);
		}
		bool safeToConnect(NumberOutput* output){
			if (!dst){
				return true;
			}
			if (!output){
				return true;
			}
			return dst->target->isSafeSource(output->target);
		}
		void ConnectHeadTo(NumberInput* input){
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
		void ConnectTailTo(NumberOutput* output){
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

		void render(sf::RenderWindow& rw) override {
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

		void dragHead(){
			ConnectHeadTo(nullptr);
			head->pos = ui::getMousePos() - this->absPos();
			head->startDrag();
		}
		void dragTail(){
			ConnectTailTo(nullptr);
			tail->pos = ui::getMousePos() - this->absPos();
			tail->startDrag();
		}

		private:
		NumberInput* dst;
		NumberOutput* src;
		Head* head;
		Tail* tail;

		friend struct NumberInput;
		friend struct NumberOutput;
	};

	// SoundWire connects a SoundInput to a SoundOutput
	struct SoundWire : Object {
		struct Head : ui::Window {
			Head(SoundWire* _wire){
				wire = _wire;
				size = {20, 20};
			}
			void onLeftClick(int clicks){
				wire->ConnectHeadTo(nullptr);
				startDrag();
			}
			SoundWire* wire;
		};
		struct Tail : ui::Window {
			Tail(SoundWire* _wire){
				wire = _wire;
				size = {20, 20};
			}
			void onLeftClick(int clicks){
				wire->ConnectTailTo(nullptr);
				startDrag();
			}
			SoundWire* wire;
		};

		SoundWire(){
			size = {0, 0};
			addChildWindow(head = new Head(this));
			addChildWindow(tail = new Tail(this));
			dst = nullptr;
			src = nullptr;
		}

		void ConnectHeadTo(SoundInput* input){
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
		void ConnectTailTo(SoundOutput* output){
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

		void render(sf::RenderWindow& rw) override {
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

		void dragHead(){
			ConnectHeadTo(nullptr);
			head->pos = ui::getMousePos() - this->absPos();
			head->startDrag();
		}
		void dragTail(){
			ConnectTailTo(nullptr);
			tail->pos = ui::getMousePos() - this->absPos();
			tail->startDrag();
		}

		private:
		SoundInput* dst;
		SoundOutput* src;
		Head* head;
		Tail* tail;

		friend struct SoundInput;
		friend struct SoundOutput;
	};





	// Pesky forward declarations
	void ProcessingObject::onDropDragWindow(Window* window){
		if (NumberWire::Head* wirehead = dynamic_cast<NumberWire::Head*>(window)){
			std::cout << "Time to show all the number inputs\n";
			return;
		}
		if (NumberWire::Tail* wiretail = dynamic_cast<NumberWire::Tail*>(window)){
			std::cout << "Time to show all the number outputs\n";
			return;
		}
		if (SoundWire::Head* wirehead = dynamic_cast<SoundWire::Head*>(window)){
			std::cout << "Time to show all the sound inputs\n";
			return;
		}
		if (SoundWire::Tail* wiretail = dynamic_cast<SoundWire::Tail*>(window)){
			std::cout << "Time to show all the sound outputs\n";
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

}