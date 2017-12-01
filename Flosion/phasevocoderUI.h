#pragma once

#include "FlosionUI.h"
#include "phasevocoder.h"

namespace fui {

	struct PhaseVocoderObject : ProcessingObject {
		PhaseVocoderObject(){
			size = {150, 150};

			auto soundin = new SoundInput(&phasevocoder.input, this);
			auto soundout = new SoundOutput(&phasevocoder, this);
			auto timespeed = new NumberInput(&phasevocoder.timespeed.input, this, "Time Speed");

			addChildWindow(timespeed, leftOf(this), insideTop(this));

			addChildWindow(soundin, leftOf(this), middleOfY(this));
			addChildWindow(soundout, rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Phase Vocoder", getFont()), middleOfX(this), middleOfY(this));
		}

		void onLeftClick(int clicks) override {
			if (keyDown(sf::Keyboard::LShift) || keyDown(sf::Keyboard::RShift)){
				auto menu = new Menu(this);
				addChildWindow(menu, middleOfX(this), middleOfY(this));
				menu->grabFocus();
			}
		}

		private:

		struct Menu : ui::Window {
			Menu(PhaseVocoderObject* _parent) : parent(_parent) {
				size = {200, 200};
				auto caption = new ui::Text("Window Size:", getFont());
				addChildWindow(caption, middleOfX(this), insideTop(this, 5.0f));
				auto b1 = new Button(this, 256);
				auto b2 = new Button(this, 512);
				auto b3 = new Button(this, 1024);
				auto b4 = new Button(this, 2048);
				auto b5 = new Button(this, 4096);
				addChildWindow(b1, insideLeft(this, 5.0f), below(caption, 5.0f));
				addChildWindow(b2, rightOf(b1, 5.0f), below(caption, 5.0f));
				addChildWindow(b3, rightOf(b2, 5.0f), below(caption, 5.0f));
				addChildWindow(b4, insideLeft(this, 5.0f), below(b1, 5.0f));
				addChildWindow(b5, rightOf(b4, 5.0f), below(b1, 5.0f));
			}

			void onLoseFocus() override {
				close();
			}

			struct Button : ui::Window {
				Button(Menu* _menu, unsigned int _windowsize) : menu(_menu), windowsize(_windowsize) {
					size = {50, 30};
					addChildWindow(new ui::Text(std::to_string(windowsize), getFont()), middleOfX(this), middleOfY(this));
				}

				void onLeftClick(int clicks) override {
					menu->parent->phasevocoder.setWindowSize(windowsize);
					menu->close();
				}

				Menu* const menu;
				const unsigned int windowsize;
			};

			PhaseVocoderObject* const parent;
		};


		musical::PhaseVocoder phasevocoder;
	};
	fuiRegisterObject(PhaseVocoderObject, "phase vocoder");

}