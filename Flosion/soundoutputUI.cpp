#include "soundoutputUI.h"
#include "soundwireUI.h"
#include "soundinputUI.h"
#include "boxUI.h"
#include "Font.h"

namespace fui {

	// SoundOutput
	SoundOutput::SoundOutput(musical::SoundSource* _target, Object* _parent, std::string _caption)
		: target(_target), owner_object(_parent) {
		size = {30, 30};
		owner_object->sound_outputs.push_back(this);
		addChildWindow(caption = new ui::Text(_caption, fui::getFont()));
		caption->pos.x = 0;
		caption->pos.y = -30;
		hover_timestamp = ui::getProgramTime() - 1;
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
		caption->visible = (ui::getProgramTime() - hover_timestamp < 0.25);
		renderChildWindows(rw);
	}
	void SoundOutput::addWireOut(SoundWire* wire){
		for (SoundWire* w : wires_out){
			if (w == wire){
				return;
			}
		}
		wires_out.push_back(wire);
		if (wire->src != this){
			wire->connectTailTo(this);
		}
	}
	void SoundOutput::removeWireOut(SoundWire* wire){
		for (auto it = wires_out.begin(); it != wires_out.end(); it++){
			if (*it == wire){
				wires_out.erase(it);
				wire->connectTailTo(nullptr);
				return;
			}
		}
	}
	bool SoundOutput::onDropWindow(Window* window){
		if (SoundWire::Tail* wiretail = dynamic_cast<SoundWire::Tail*>(window)){
			wiretail->wire->connectTailTo(this);
			return true;
		}
		return true;
	}
	void SoundOutput::onHover(){
		hover_timestamp = ui::getProgramTime();
	}
	void SoundOutput::onHoverWithWindow(Window* win){
		if (dynamic_cast<SoundWire::Tail*>(win)){
			hover_timestamp = ui::getProgramTime();
		}
	}
	void SoundOutput::onLeftClick(int clicks){
		SoundWire* wire = new SoundWire;
		this->owner_object->getBox()->addObject(wire);
		wire->connectTailTo(this);
		wire->dragHead();
	}
	std::string SoundOutput::getCaption() const {
		return caption->getText();
	}
	musical::SoundSource* SoundOutput::getSource(){
		return target;
	}
	vec2 SoundOutput::getWireDirection() const {
		return vec2(1.0f, 0.0f); // TODO: make customizable or automatic
	}

} // namespace fui