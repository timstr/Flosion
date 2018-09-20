#pragma once

#include "FlosionUI.h"
#include "wavegen.h"

namespace fui {

	struct WaveGenObject : ProcessingObject {
		WaveGenObject(){
			size = {150, 100};
			auto in1 = new NumberInput(&wavegen.frequency, this, "Frequency");
			auto in2 = new NumberInput(&wavegen.wavefunction, this, "Wave Function");
			addChildWindow(in1, leftOf(this), insideTop(this));
			addChildWindow(in2, leftOf(this), below(in1));
			addChildWindow(new NumberOutput(&wavegen.phase, this, "Phase"), middleOfX(this), above(this));
			addChildWindow(new SoundOutput(&wavegen, this), rightOf(this), middleOfY(this));
			addChildWindow(new ui::Text("WaveGenerator", fui::getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::WaveGen wavegen;
	};
	fuiRegisterObject(WaveGenObject, "wave generator");
};