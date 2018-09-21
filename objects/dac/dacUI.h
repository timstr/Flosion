#pragma once
#include "FlosionUI.h"
#include "DAC.h"

namespace fui {

	struct DACObject : Object {
		DACObject(){
			setMinSize({150, 150});
			write("DAC", getFont());
			add<SoundInput>(thisAs<Object>(), dac.input.input, "Sound Input");
			add<PlayButton>(*this);
			add<AmpMeter>(*this);
		}

	private:

		struct PlayButton : ui::InlineElement {
			PlayButton(DACObject& _parent) :
				parent(_parent) {
				setMinSize({50, 50});
				caption = add<ui::Text>("Play", getFont());
			}

			bool onLeftClick(int clicks) override {
				if (parent.dac.isPlaying()){
					parent.dac.pause();
					caption->setText("Play");
				} else {
					parent.dac.play();
					caption->setText("Pause");
				}
				return true;
			}

			private:
			ui::Ref<ui::Text> caption;
			DACObject& parent;
		};

		struct AmpMeter : ui::InlineElement {
			AmpMeter(DACObject& _parent) : parent(_parent){
				setSize({50, 100});
			}
			void render(sf::RenderWindow& rw) override {
				sf::RectangleShape rect;
				rect.setSize(size());
				rect.setFillColor(sf::Color(0xFF));
				rect.setOutlineColor(sf::Color(0xFF));
				rect.setOutlineThickness(1);
				rw.draw(rect);

				musical::Sample ampl = parent.dac.getCurrentAmp();

				float lheight = ampl.l * top();
				float rheight = ampl.r * top();

				rect.setSize(vec2(20, lheight));
				rect.setPosition(vec2(0, top() - lheight));
				rect.setFillColor(getColor(ampl.l));
				rect.setOutlineThickness(0);
				rw.draw(rect);

				rect.setSize(vec2(20, rheight));
				rect.setPosition(vec2(30, top() - rheight));
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
			DACObject& parent;
		};

		musical::DAC dac;
	};
	RegisterFactoryObject(DACObject, "dac");
}
