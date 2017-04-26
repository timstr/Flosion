#pragma once

#include "FlosionUICore.h"
#include "timestretch.h"

namespace fui {

	struct TimeStretchObject : ProcessingObject {
		TimeStretchObject(){
			size = {150, 150};
			addNumberInput(new NumberInput(&timestretch.speed.input, this, "Speed Ratio", {-5, 5}));
			addSoundInput(new SoundInput(&timestretch.input, this, {-5, 40}));
			addSoundOutput(new SoundOutput(&timestretch, this, {size.x - 25, 5}));
			addChildWindow(new ui::Text("TimeStretch", fui::getFont()), {10, 10});
		}

		private:

		musical::TimeStretch timestretch;
	};
	fuiRegisterObject(TimeStretchObject, "timestretch", "stretch");
}