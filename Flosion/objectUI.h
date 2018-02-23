#pragma once

#include <gui/gui.h>
#include "Font.h"

namespace fui {

	struct NumberInput;
	struct NumberOutput;
	struct NumberWire;
	struct SoundInput;
	struct SoundOutput;
	struct SoundWire;
	struct Container;

	// Object is the base class to all gui components representing sound and
	// numerical processing, and their related components
	struct Object : ui::Window {
		Object();

		Container* getContainer();

		private:

		// TODO: move to CPP file
		struct WireDropList : ui::Window {
			WireDropList(const std::vector<std::pair<std::function<void()>, std::string>>& items){
				size.x = 200.0f;
				size.y = 30.0f * items.size();
				int count = 0;
				for (auto& item : items){
					addChildWindow(new ListItem(this, item.first, item.second), vec2(0, count * 30.0f));
					count++;
				}
			}

			void onLoseFocus() override {
				close();
			}

			void render(sf::RenderWindow& rw) override {
				renderChildWindows(rw);
			}

			struct ListItem : ui::Window {
				ListItem(WireDropList* _parent, const std::function<void()>& fn, const std::string& str) : callback(fn), parent(_parent) {
					clipping = true;
					size = vec2(200.0f, 30.0f);
					addChildWindow(new ui::Text(str, getFont(), sf::Color(0xFFFFFFFF), 20), insideLeft(this, 5.0f), insideTop(this, 0.0f));
				}

				void onLeftClick(int clicks) override {
					callback();
					parent->close();
				}

				void render(sf::RenderWindow& rw) override {
					sf::RectangleShape rect;
					rect.setPosition(vec2(1.0f, 1.0f));
					rect.setSize(size - vec2(2.0f, 2.0f));
					rect.setFillColor(sf::Color(0x000000C0));
					rect.setOutlineColor(sf::Color(0x80808080));
					rect.setOutlineThickness(1.0f);
					rw.draw(rect);
					renderChildWindows(rw);
				}

				WireDropList* const parent;
				const std::function<void()> callback;
			};
		};

		// TODO: hide invalid inputs/outputs
		void showNumberInputList(NumberWire* wire, vec2 pos);
		void showNumberOutputList(NumberWire* wire, vec2 pos);
		void showSoundInputList(SoundWire* wire, vec2 pos);
		void showSoundOutputList(SoundWire* wire, vec2 pos);

		void showWireDropList(const std::vector<std::pair<std::function<void()>, std::string>>& items, vec2 pos);

		// the FUI container that holds the current object
		// or if this is the master container should this ever be null
		Container* container;
		std::vector<SoundInput*> sound_inputs;
		std::vector<SoundOutput*> sound_outputs;
		std::vector<NumberInput*> number_inputs;
		std::vector<NumberOutput*> number_outputs;

		friend struct Container;
		friend struct SoundInput;
		friend struct SoundOutput;
		friend struct NumberInput;
		friend struct NumberOutput;

		friend struct ProcessingObject;
	};

} // namespace fui