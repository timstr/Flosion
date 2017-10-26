#pragma once

#include "FlosionUI.h"
#include "timestretch.h"

namespace fui {

	struct TimeStretchObject : ProcessingObject {
		TimeStretchObject(){
			size = {150, 150};
			addNumberInput(new NumberInput(&timestretch.speed.input, this, "Speed Ratio"), leftOf(this), insideTop(this));
			addSoundInput(new SoundInput(&timestretch.input, this), leftOf(this), middleOfY(this));
			addSoundOutput(new SoundOutput(&timestretch, this), rightOf(this), middleOfY(this));
			addChildWindow(new ui::Text("TimeStretch", fui::getFont()), middleOfX(this), middleOfY(this));
		}

		private:

		musical::TimeStretch timestretch;
	};
	fuiRegisterObject(TimeStretchObject, "timestretch", "stretch");
}