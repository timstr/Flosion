#include "soundwireUI.h"
#include "soundinputUI.h"
#include "soundoutputUI.h"

namespace fui {

	// SoundWire Head
	SoundWire::Head::Head(SoundWire* _wire){
		wire = _wire;
		size = {20, 20};
	}
	void SoundWire::Head::onLeftClick(int clicks){
		wire->ConnectHeadTo(nullptr);
		startDrag();
	}
	bool SoundWire::Head::onDropWindow(Window* window){
		if (wire->dst){
			return wire->dst->onDropWindow(window);
		}
		return false;
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
	bool SoundWire::Tail::onDropWindow(Window* window){
		if (wire->src){
			return wire->src->onDropWindow(window);
		}
		return false;
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

} // namespace fui