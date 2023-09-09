#pragma once
#include <GUI/Helpers/ToggleButton.hpp>
#include "ObjectFactory.h"
#include "DAC.h"

namespace fui {

	struct DACObject : Object {
		DACObject(){
			setMinSize({150, 150});
			write("DAC", getFont());
			add<SoundInput>(this->thisAs<Object>(), dac.input.input, "Sound Input");
			add<ui::ToggleButton>(false, getFont(), [this](bool val){
				if (val){
					dac.play();
				} else {
					dac.pause();
				}
			}, std::pair<std::string, std::string>{"play", "pause"});
			add<AmpMeter>(*this);
		}

	private:

		struct AmpMeter : ui::InlineElement {
			AmpMeter(DACObject& _parent) : parent(_parent){
				setSize({50, 100}, true);
				setBackgroundColor(sf::Color(0xFF));
				setBorderRadius(3);
				setBorderColor(sf::Color(0x80));
				setBorderThickness(2);
			}
			void render(sf::RenderWindow& rw) override {
				Element::render(rw);

				flo::Sample ampl = parent.dac.getCurrentAmp();

				float lheight = ampl.l * height();
				float rheight = ampl.r * height();

				sf::RectangleShape rect;
				rect.setSize(vec2(20, lheight));
				rect.setPosition(vec2(0, height() - lheight));
				rect.setFillColor(getColor(ampl.l));
				rect.setOutlineThickness(0);
				rw.draw(rect);

				rect.setSize(vec2(20, rheight));
				rect.setPosition(vec2(30, height() - rheight));
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

		flo::DAC dac;
	};
	RegisterFactoryObject(DACObject, "dac");
}
