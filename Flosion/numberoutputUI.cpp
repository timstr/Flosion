#include "numberoutputUI.h"
#include "numberinputUI.h"
#include "numberwireUI.h"
#include "pi.h"
#include "boxUI.h"

namespace fui {

	NumberOutput::NumberOutput(musical::NumberSource* _target, Object* _parent, std::string _caption, std::function<void(NumberInput*)> _onConnect)
		: target(_target), owner_object(_parent) {
		size = {30, 30};
		owner_object->number_outputs.push_back(this);
		addChildWindow(caption = new ui::Text(_caption, fui::getFont()));
		caption->pos.x = 0;
		caption->pos.y = -30;
		hover_timestamp = ui::getProgramTime() - 1;
		onConnect = _onConnect;
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
		caption->visible = (ui::getProgramTime() - hover_timestamp < 0.25);
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
			wire->connectTailTo(this);
		}
	}
	void NumberOutput::removeWireOut(NumberWire* wire){
		for (auto it = wires_out.begin(); it != wires_out.end(); it++){
			if (*it == wire){
				wires_out.erase(it);
				wire->connectTailTo(nullptr);
				return;
			}
		}
	}
	bool NumberOutput::onDropWindow(Window* window){
		if (NumberWire::Tail* wiretail = dynamic_cast<NumberWire::Tail*>(window)){
			if (wiretail->wire->safeToConnect(this)){
				wiretail->wire->connectTailTo(this);
			} else {
				vec2 start = wiretail->pos;
				vec2 diff = wiretail->pos - wiretail->wire->head->pos;
				vec2 end = vec2(sf::Vector2i(wiretail->pos - (diff * (float)(50 / hypot(diff.x, diff.y)))));

				wiretail->startTransition(0.25, [=](double x){
					wiretail->pos = start + (end - start) * (float)sin(x * pi<double> / 2);
				});
			}
			return true;
		}
		return false;
	}
	void NumberOutput::onHoverWithWindow(Window* win){
		if (dynamic_cast<NumberWire::Tail*>(win)){
			hover_timestamp = ui::getProgramTime();
		}
	}
	void NumberOutput::onLeftClick(int clicks){
		NumberWire* wire = new NumberWire;
		this->owner_object->getBox()->addObject(wire);
		wire->connectTailTo(this);
		wire->dragHead();
	}
	std::string NumberOutput::getCaption() const {
		return caption->getText();
	}
	musical::NumberSource* NumberOutput::getSource() const {
		return target;
	}

	vec2 NumberOutput::getWireDirection() const {
		return vec2(1.0f, 0.0f); // TODO: make customizable or automatic
	}

} // namespace fui