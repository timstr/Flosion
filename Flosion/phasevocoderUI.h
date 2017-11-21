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

		private:

		musical::PhaseVocoder phasevocoder;
	};
	fuiRegisterObject(PhaseVocoderObject, "phase vocoder");

}