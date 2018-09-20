#pragma once

#include "FlosionUI.h"
#include "phasevocoder.h"
#include "gui/forms.h"

namespace fui {

	struct PhaseVocoderObject : ProcessingObject {
		PhaseVocoderObject(){
			size = {150, 150};

			auto soundin = new SoundInput(&phasevocoder.input, this);
			auto soundout = new SoundOutput(&phasevocoder, this);
			auto timespeed = new NumberInput(&phasevocoder.timespeed, this, "Time Speed");

			addChildWindow(timespeed, leftOf(this), insideTop(this));

			addChildWindow(soundin, leftOf(this), middleOfY(this));
			addChildWindow(soundout, rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Phase Vocoder", getFont()), middleOfX(this), middleOfY(this));
		}

		void onLeftClick(int clicks) override {
			if (keyDown(sf::Keyboard::LShift) || keyDown(sf::Keyboard::RShift)){
				showMenu(localMousePos());
			}
		}

		void showMenu(vec2 center){
			ui::forms::Model model;

			model["Window Size"] = ui::forms::PullDownProperty<unsigned>(
				{
					{256, "256"},
					{512, "512"},
					{1024, "1024"},
					{2048, "2048"},
					{4096, "4096"},
					{8192, "8192"},
				},
				phasevocoder.getWindowSize());

			ui::forms::Form* form = new ui::forms::Form(model, getFont());

			form->onSubmit([this](ui::forms::Model m){
				phasevocoder.setWindowSize(m["Window Size"]);
			});

			addChildWindow(form);
			form->pos = center - form->size * 0.5f;
			form->pos = vec2(round(form->pos.x), round(form->pos.y));
		}

		private:

		musical::PhaseVocoder phasevocoder;
	};
	fuiRegisterObject(PhaseVocoderObject, "phase vocoder");

}