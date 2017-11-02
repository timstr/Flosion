#pragma once
#include "FlosionUI.h"
#include "DAC.h"

namespace fui {

	struct DACObject : ProcessingObject {
		DACObject(){
			size = {150, 150};
			addChildWindow(new ui::Text("DAC", getFont()));
			addChildWindow(new SoundInput(&dac.input.input, this), leftOf(this), middleOfY(this));
			addChildWindow(new PlayButton(this), {25, 20});
			addChildWindow(new AmpMeter(this), {90, 10});
		}

		private:
		struct PlayButton : ui::Window {
			PlayButton(DACObject* _parent){
				parent = _parent;
				size = {50, 50};
				addChildWindow(caption = new ui::Text("Play", getFont()));
			}

			void onLeftClick(int clicks) override {
				if (parent->dac.isPlaying()){
					parent->dac.pause();
					caption->setText("Play");
				} else {
					parent->dac.play();
					caption->setText("Pause");
				}
			}

			private:
			ui::Text* caption;
			DACObject* parent;
		};

		struct AmpMeter : ui::Window {
			AmpMeter(DACObject* _parent) : parent(_parent){
				size = {50, 100};
			}
			void render(sf::RenderWindow& rw) override {
				sf::RectangleShape rect;
				rect.setSize(size);
				rect.setFillColor(sf::Color(0xFF));
				rect.setOutlineColor(sf::Color(0xFF));
				rect.setOutlineThickness(1);
				rw.draw(rect);

				musical::Sample ampl = parent->dac.getCurrentAmp();

				double lheight = ampl.l * size.y;
				double rheight = ampl.r * size.y;

				rect.setSize(sf::Vector2f(20, lheight));
				rect.setPosition(sf::Vector2f(0, size.y - lheight));
				rect.setFillColor(getColor(ampl.l));
				rect.setOutlineThickness(0);
				rw.draw(rect);

				rect.setSize(sf::Vector2f(20, rheight));
				rect.setPosition(sf::Vector2f(30, size.y - rheight));
				rect.setFillColor(getColor(ampl.r));
				rw.draw(rect);
			}

			sf::Color getColor(double ampl){
				if (ampl < 0.5){
					return sf::Color(0x00FF00FF);
				} else if (ampl < 0.75){
					return sf::Color(0xFFFF00FF);
				} else {
					return sf::Color(0xFF0000FF);
				}
			}
			private:
			DACObject* parent;
		};

		musical::DAC dac;
	};
	fuiRegisterObject(DACObject, "dac");
}
