#pragma once

#include "FlosionUI.h"
#include "ensemble.h"

namespace fui {

	struct EnsembleObject : ProcessingObject {
		EnsembleObject(){
			size = {150, 145};
			addSoundOutput(new SoundOutput(&ensemble, this, {size.x - 20, 5}));
			addNumberInput(new NumberInput(&ensemble.num_voices.input, this, "Number of Voices", {-5, 40}));
			addNumberInput(new NumberInput(&ensemble.frequency.input, this, "Frequency", {-5, 5}));
			addNumberInput(new NumberInput(&ensemble.frequency_spread.input, this, "Frequency Spread", {-5, 75}));
			addSoundInput(new SoundInput(&ensemble.input, this, {-5, 110}));
			addNumberOutput(new NumberOutput(&ensemble.input.frequency, this, "Frequency", {60, -5}));
			addChildWindow(new ui::Text("Ensemble", fui::getFont()), {25, 25});
		}

		private:
		musical::Ensemble ensemble;
	};
	fuiRegisterObject(EnsembleObject, "ensemble");
};