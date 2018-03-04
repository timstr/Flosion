#include "numberinputUI.h"
#include "pi.h"
#include "boxUI.h"

namespace fui {

	NumberInput::NumberInput(musical::NumberInput* _target, Object* _parent, const std::string& _caption, const std::function<void(NumberOutput*)>& _onConnect)
		: target(_target), owner_object(_parent) {
		caption_str = _caption;
		size = {30, 30};
		owner_object->number_inputs.push_back(this);
		wire_in = nullptr;
		addChildWindow(caption = new ui::Text(_caption, fui::getFont()));
		caption->pos.x = -5 - caption->size.x;
		caption->pos.y = 0;
		hover_timestamp = ui::getProgramTime() - 1;
		onConnect = _onConnect;
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
	bool NumberInput::onDropWindow(Window* window) {
		if (NumberWire::Head* wirehead = dynamic_cast<NumberWire::Head*>(window)){
			if (wirehead->wire->safeToConnect(this)){
				if (wire_in){
					NumberWire* oldwire = wire_in;
					setWireIn(nullptr);

					vec2 start = oldwire->head->pos;
					vec2 diff = oldwire->head->pos - oldwire->tail->pos;
					vec2 end = vec2(sf::Vector2i(oldwire->head->pos - (diff * (float)(50 / hypot(diff.x, diff.y)))));

					oldwire->head->startTransition(0.25, [=](double x){
						oldwire->head->pos = start + (end - start) * (float)sin(x * pi<double> / 2);
					});
				}

				wirehead->wire->ConnectHeadTo(this);
			} else {
				vec2 start = wirehead->pos;
				vec2 diff = wirehead->pos - wirehead->wire->tail->pos;
				vec2 end = vec2(sf::Vector2i(wirehead->pos - (diff * (float)(50 / hypot(diff.x, diff.y)))));

				wirehead->startTransition(0.25, [=](double x){
					wirehead->pos = start + (end - start) * (float)sin(x * pi<double> / 2);
				});
			}
			return true;
		}
		return false;
	}
	void NumberInput::onHoverWithWindow(Window* win){
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
	const std::string& NumberInput::getCaption() const {
		return caption_str;
	}
	const musical::NumberInput* NumberInput::getInput() const {
		return target;
	}

	vec2 NumberInput::getWireDirection() const {
		return vec2(-1.0f, 0.0f); // TODO: make customizable or automatic
	}

} // namespace fui