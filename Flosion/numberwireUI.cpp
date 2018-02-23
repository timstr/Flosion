#include "numberwireUI.h"

namespace fui {

	// NumberWire Head
	NumberWire::Head::Head(NumberWire* _wire){
		wire = _wire;
		size = {20, 20};
	}
	void NumberWire::Head::onLeftClick(int clicks){
		wire->ConnectHeadTo(nullptr);
		startDrag();
	}
	bool NumberWire::Head::onDropWindow(Window* window){
		if (wire->dst){
			return wire->dst->onDropWindow(window);
		}
		return false;
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
	bool NumberWire::Tail::onDropWindow(Window* window){
		if (wire->src){
			return wire->src->onDropWindow(window);
		}
		return false;
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
				if (src->onConnect){
					src->onConnect(dst);
				}
				if (dst->onConnect){
					dst->onConnect(src);
				}
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
				if (src->onConnect){
					src->onConnect(dst);
				}
				if (dst->onConnect){
					dst->onConnect(src);
				}
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

} // namespace fui