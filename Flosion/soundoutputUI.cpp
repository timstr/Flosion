#include "soundoutputUI.h"
#include "soundwireUI.h"
#include "soundinputUI.h"
#include "boxUI.h"

namespace fui {

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
	bool SoundOutput::onDropWindow(Window* window){
		if (SoundWire::Tail* wiretail = dynamic_cast<SoundWire::Tail*>(window)){
			wiretail->wire->ConnectTailTo(this);
			return true;
		}
		return true;
	}
	void SoundOutput::onLeftClick(int clicks){
		SoundWire* wire = new SoundWire;
		this->owner_object->getContainer()->addObject(wire);
		wire->ConnectTailTo(this);
		wire->dragHead();
	}

	vec2 SoundOutput::getWireDirection() const {
		return vec2(1.0f, 0.0f); // TODO: make customizable or automatic
	}

} // namespace fui