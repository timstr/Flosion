#pragma once

#include "FlosionUICore.h"
#include "wavegen.h"

namespace fui {

	struct WaveGenObject : ProcessingObject {
		WaveGenObject(){
			size = {150, 100};
			addSoundOutput(new SoundOutput(&wavegen, this, {size.x - 20, 5}));
			addNumberInput(new NumberInput(&wavegen.frequency.input, this, "Frequency", {-5, 5}));
			addNumberInput(new NumberInput(&wavegen.wavefunction.input, this, "Wave Function", {-5, 40}));
			addNumberOutput(new NumberOutput(&wavegen.phase, this, {60, -5}));
			addChildWindow(new ui::Text("frequency", fui::getFont()), {25, 5});
			addChildWindow(new ui::Text("wavefunction", fui::getFont()), {25, 40});
			addChildWindow(new ui::Text("WaveGenerator", fui::getFont()), {40, 60});
		}

		private:
		musical::WaveGen wavegen;
	};
	fuiRegisterObject(WaveGenObject, "wavegen", "wave generator");
};