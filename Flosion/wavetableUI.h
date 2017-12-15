#pragma once

#include "FlosionUI.h"
#include "wavetable.h"
#include <vector>

namespace fui {

	// TODO: add lock button

	struct WavetableObject : ProcessingObject {
		WavetableObject(){
			size = {200, 100};
			addChildWindow(grabber = new Grabber(this));
			addChildWindow(new NumberInput(&wavetable.input, this, "Input"), leftOf(this), middleOfY(this));
			addChildWindow(new NumberOutput(&wavetable, this, "Output"), rightOf(this), middleOfY(this));
			redraw();
		}

		void redraw(){
			int width = (int)size.x;
			vertices.resize(width * 2 + 2);
			float min = wavetable.getMinY();
			float max = wavetable.getMaxY();
			for (int i = 0; i <= width; i++){
				float progress = (float)i / width;
				float y = (wavetable.getValueAt(progress) - min) / (max - min);
				float height = (1.0f - y) * size.y;
				vertices[i * 2].position = vec2((float)i, height);
				vertices[i * 2 + 1].position = vec2((float)i, size.y);
				vertices[i * 2].color = sf::Color(0x00FF0080);
				vertices[i * 2 + 1].color = sf::Color(0x00FF0080);
			}
		}

		void render(sf::RenderWindow& rw) override {
			sf::RectangleShape rect;
			rect.setSize(size);
			rect.setFillColor(sf::Color(0x40404080));
			rect.setOutlineColor(sf::Color(0xFF));
			rect.setOutlineThickness(1.0f);
			rw.draw(rect);
			rw.draw(vertices.data(), vertices.size(), sf::PrimitiveType::TriangleStrip);
			renderChildWindows(rw);
		}

		void fillLine(vec2 start, vec2 end){
			float x0 = start.x / size.x;
			float t = 1.0f - (start.y / size.y);
			float y0 = t * (wavetable.getMaxY() - wavetable.getMinY()) + wavetable.getMinY();

			float x1 = end.x / size.x;
			t = 1.0f - (end.y / size.y);
			float y1 = t * (wavetable.getMaxY() - wavetable.getMinY()) + wavetable.getMinY();
			wavetable.fillLine(x0, x1, y0, y1);
			redraw();
		}

		void onLeftClick(int clicks){
			if (keyDown(sf::Keyboard::LShift) || keyDown(sf::Keyboard::RShift)){
				// TODO: show menu
			} else {
				grabber->pos = localMousePos();
				grabber->old_pos = grabber->pos;
				grabber->startDrag();
			}
		}

		private:

		struct Grabber : ui::Window {
			Grabber(WavetableObject* _parent) : parent(_parent) {
				size = {0, 0};
				old_pos = pos;
			}

			void render(sf::RenderWindow& rw) override {

			}

			void onDrag() override {
				pos.x = std::min(std::max(pos.x, 0.0f), parent->size.x);
				pos.y = std::min(std::max(pos.y, 0.0f), parent->size.y);
				parent->fillLine(old_pos, pos);
				old_pos = pos;
			}

			vec2 old_pos;
			WavetableObject* const parent;
		}* grabber;

		musical::WaveTable wavetable;
		std::vector<sf::Vertex> vertices;
	};
	fuiRegisterObject(WavetableObject, "wavetable");

}