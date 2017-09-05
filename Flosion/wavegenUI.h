#pragma once

#include "FlosionUI.h"
#include "wavegen.h"

namespace fui {

	struct WaveGenObject : ProcessingObject {
		WaveGenObject(){
			size = {150, 100};
			addSoundOutput(new SoundOutput(&wavegen, this, {size.x - 20, 5}));
			addNumberInput(new NumberInput(&wavegen.frequency.input, this, "Frequency", {-5, 5}));
			addNumberInput(new NumberInput(&wavegen.wavefunction.input, this, "Wave Function", {-5, 40}));
			addNumberOutput(new NumberOutput(&wavegen.phase, this, "Phase", {60, -5}));
			addChildWindow(new ui::Text("WaveGenerator", fui::getFont()), {25, 25});
		}

		private:
		musical::WaveGen wavegen;
	};
	fuiRegisterObject(WaveGenObject, "wave generator");
};