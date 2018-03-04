#include "soundwireUI.h"
#include "soundinputUI.h"
#include "soundoutputUI.h"
#include "pi.h"

namespace fui {

	// SoundWire Head
	SoundWire::Head::Head(SoundWire* _wire){
		wire = _wire;
		size = {20, 20};
	}
	void SoundWire::Head::onLeftClick(int clicks){
		wire->connectHeadTo(nullptr);
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
		wire->connectTailTo(nullptr);
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
	void SoundWire::connectHeadTo(SoundInput* input){
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
	void SoundWire::connectTailTo(SoundOutput* output){
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
		update();
		rw.draw(vertices.data(), vertices.size(), sf::LineStrip);
		renderChildWindows(rw);
	}
	void SoundWire::dragHead(){
		connectHeadTo(nullptr);
		head->pos = ui::getMousePos() - this->absPos();
		head->startDrag();
	}
	void SoundWire::dragTail(){
		connectTailTo(nullptr);
		tail->pos = ui::getMousePos() - this->absPos();
		tail->startDrag();
	}

	void SoundWire::update(){
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

		for (int v = 0; v < vertices.size(); v++) {
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
				(uint8_t)(255.0f * (0.5f + 0.5f * sin((phase * pi<float> * 2.0f) + (t * dist * 0.1f)))),
				0);
		}
	}

} // namespace fui