#pragma once

#include "ObjectFactory.h"
#include "wavetable.h"
#include <vector>

namespace fui {

	// TODO: add lock button

	struct WavetableObject : Object {
		WavetableObject(){
			setSize({200, 100}, true);
			grabber = add<Grabber>(*this);
			auto self = thisAs<Object>();
			add<NumberInput>(self, wavetable.input, "Input");
			add<NumberOutput>(self, wavetable, "Output");
			redraw();
		}

		void redraw(){
			int w = (int)width();
			vertices.resize(w* 2 + 2);
			float min = wavetable.getMinY();
			float max = wavetable.getMaxY();
			for (int i = 0; i <= w; i++){
				float progress = (float)i / w;
				float y = (wavetable.getValueAt(progress) - min) / (max - min);
				float h = (1.0f - y) * height();
				vertices[i * 2].position = vec2((float)i, h);
				vertices[i * 2 + 1].position = vec2((float)i, height());
				vertices[i * 2].color = sf::Color(0x00FF0080);
				vertices[i * 2 + 1].color = sf::Color(0x00FF0080);
			}
		}

		void render(sf::RenderWindow& rw) override {
			Object::render(rw);
			rw.draw(vertices.data(), vertices.size(), sf::PrimitiveType::TriangleStrip);
		}

		void fillLine(vec2 start, vec2 end){
			float x0 = start.x / left();
			float t = 1.0f - (start.y / top());
			float y0 = t * (wavetable.getMaxY() - wavetable.getMinY()) + wavetable.getMinY();

			float x1 = end.x / left();
			t = 1.0f - (end.y / top());
			float y1 = t * (wavetable.getMaxY() - wavetable.getMinY()) + wavetable.getMinY();
			wavetable.fillLine(x0, x1, y0, y1);
			redraw();
		}

		bool onLeftClick(int clicks){
			if (keyDown(sf::Keyboard::LShift) || keyDown(sf::Keyboard::RShift)){
				// TODO: show menu
			} else {
				grabber->setPos(localMousePos());
				grabber->old_pos = grabber->pos();
				grabber->startDrag();
			}
			return true;
		}

		private:

		struct Grabber : ui::FreeElement {
			Grabber(WavetableObject& _parent) : parent(_parent) {
				setSize({0, 0});
				setVisible(false);
				old_pos = pos();
			}

			void render(sf::RenderWindow& rw) override {

			}

			void onDrag() override {
				setLeft(std::min(std::max(left(), 0.0f), parent.left()));
				setTop(std::min(std::max(top(), 0.0f), parent.top()));
				parent.fillLine(old_pos, pos());
				old_pos = pos();
			}

			vec2 old_pos;
			WavetableObject& parent;
		};
		
		ui::Ref<Grabber> grabber;

		flo::WaveTable wavetable;
		std::vector<sf::Vertex> vertices;
	};
	RegisterFactoryObject(WavetableObject, "wavetable");

}