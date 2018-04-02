#pragma once

#include "FlosionUI.h"
#include "ensemble.h"

namespace fui {

	struct EnsembleObject : ProcessingObject {
		EnsembleObject(){
			size = {150, 145};
			auto numv = new NumberInput(&ensemble.num_voices.input, this, "Number of Voices");
			auto freq = new NumberInput(&ensemble.frequency.input, this, "Frequency");
			auto fspr = new NumberInput(&ensemble.frequency_spread.input, this, "Frequency Spread");
			auto soundin = new SoundInput(&ensemble.input, this);


			addChildWindow(new SoundOutput(&ensemble, this), rightOf(this), insideTop(this, 20));
			addChildWindow(numv, leftOf(this), insideTop(this));
			addChildWindow(freq, leftOf(this), below(numv));
			addChildWindow(fspr, leftOf(this), below(freq));
			addChildWindow(soundin, leftOf(this), below(fspr, 5));
			addChildWindow(new NumberOutput(&ensemble.input.frequency, this, "Frequency"), insideLeft(this), above(this));
			addChildWindow(new ui::Text("Ensemble", fui::getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::Ensemble ensemble;
	};
	fuiRegisterObject(EnsembleObject, "ensemble");
};