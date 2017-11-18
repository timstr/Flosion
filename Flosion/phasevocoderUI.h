#pragma once

#include "FlosionUI.h"
#include "phasevocoder.h"

namespace fui {

	struct PhaseVocoderObject : ProcessingObject {
		PhaseVocoderObject(){
			size = {150, 150};

			addChildWindow(new SoundInput(&phasevocoder.input, this), leftOf(this), middleOfY(this));
			addChildWindow(new SoundOutput(&phasevocoder, this), rightOf(this), middleOfY(this));

			addChildWindow(new NumberInput(&phasevocoder.timespeed.input, this, "Time Speed"), leftOf(this), insideTop(this));

			addChildWindow(new ui::Text("Phase Vocoder", getFont()), middleOfX(this), middleOfY(this));
		}

		private:

		musical::PhaseVocoder phasevocoder;
	};
	fuiRegisterObject(PhaseVocoderObject, "phase vocoder");

}