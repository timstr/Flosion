#include "soundinputUI.h"
#include "soundoutputUI.h"
#include "soundwireUI.h"
#include "pi.h"
#include "boxUI.h"

namespace fui {

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
	bool SoundInput::onDropWindow(Window* window) {
		if (SoundWire::Head* wirehead = dynamic_cast<SoundWire::Head*>(window)){
			if (wire_in){
				SoundWire* oldwire = wire_in;
				setWireIn(nullptr);

				vec2 start = oldwire->head->pos;
				vec2 diff = oldwire->head->pos - oldwire->tail->pos;
				vec2 end = vec2(sf::Vector2i(oldwire->head->pos - (diff * (float)(50 / hypot(diff.x, diff.y)))));

				oldwire->head->startTransition(0.25, [=](double x){
					oldwire->head->pos = start + (end - start) * (float)sin(x * pi<double> / 2);
				});
			}

			wirehead->wire->ConnectHeadTo(this);
			return true;
		}
		return false;
	}
	void SoundInput::onLeftClick(int clicks){
		if (wire_in){
			wire_in->dragHead();
		} else {
			SoundWire* wire = new SoundWire;
			this->owner_object->getBox()->addObject(wire);
			wire->ConnectHeadTo(this);
			wire->dragTail();
		}
	}

	vec2 SoundInput::getWireDirection() const {
		return vec2(-1.0f, 0.0f); // TODO: make customizable or automatic
	}

} // namespace fui