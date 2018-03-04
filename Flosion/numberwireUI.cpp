#include "numberwireUI.h"
#include "pi.h"

namespace fui {

	// NumberWire Head
	NumberWire::Head::Head(NumberWire* _wire){
		wire = _wire;
		size = {20, 20};
	}
	void NumberWire::Head::onLeftClick(int clicks){
		wire->connectHeadTo(nullptr);
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
		wire->connectTailTo(nullptr);
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
		connectHeadTo(nullptr);
		connectTailTo(nullptr);
	}
	bool NumberWire::safeToConnect(NumberInput* input) const {
		if (!src){
			return true;
		}
		if (!input){
			return true;
		}
		return input->target->isSafeSource(src->target);
	}
	bool NumberWire::safeToConnect(NumberOutput* output) const {
		if (!dst){
			return true;
		}
		if (!output){
			return true;
		}
		return dst->target->isSafeSource(output->target);
	}
	void NumberWire::connectHeadTo(NumberInput* input){
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
	void NumberWire::connectTailTo(NumberOutput* output){
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
		update();
		rw.draw(vertices.data(), vertices.size(), sf::LineStrip);
		renderChildWindows(rw);
	}
	void NumberWire::dragHead(){
		connectHeadTo(nullptr);
		head->pos = ui::getMousePos() - this->absPos();
		head->startDrag();
	}
	void NumberWire::dragTail(){
		connectTailTo(nullptr);
		tail->pos = ui::getMousePos() - this->absPos();
		tail->startDrag();
	}

	void NumberWire::update(){
		if (dst) {
			head->pos = dst->absPos() - absPos();
		}
		if (src) {
			tail->pos = src->absPos() - absPos();
		}

		float dist = hypot(head->pos.x - tail->pos.x, head->pos.y - tail->pos.y);

		vertices.resize((size_t)dist);

		float phase = 2.0f * (float)ui::getProgramTime();
		vec2 headdir = dst->getWireDirection();
		vec2 taildir = src->getWireDirection();

		headdir /= hypot(headdir.x, headdir.y);
		taildir /= hypot(taildir.x, taildir.y);

		vec2 A = head->pos + head->size * 0.5f;
		vec2 B = head->pos + head->size * 0.5f + headdir * dist * 0.5f;
		vec2 C = tail->pos + tail->size * 0.5f + taildir * dist * 0.5f;
		vec2 D = tail->pos + tail->size * 0.5f;

		for (int v = 0; v < vertices.size(); v++){
			const float t = (float)v / (float)(vertices.size() - 1);
			const float omt = 1.0f - t;

			vec2 P =
				A * omt * omt * omt +
				B * 3.0f * omt * omt * t +
				C * 3.0f * omt * t * t +
				D * t * t * t;

			vertices[v].position = P;

			vertices[v].color = sf::Color(
				0,
				0,
				(uint8_t)(255.0f * (0.5f + 0.5f * sin((phase * pi<float> * 2.0f) + (t * dist * 0.1f)))));
		}
	}

} // namespace fui