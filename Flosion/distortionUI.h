#pragma once

#include "FlosionUI.h"
#include "distortion.h"

namespace fui {

	struct DistortionObject : ProcessingObject {
		DistortionObject(){
			size = {150, 150};

			auto ampin = new NumberInput(&distortion.gain.input, this, "Gain");
			auto roundin = new NumberInput(&distortion.hardness.input, this, "Hardness");
			auto soundin = new SoundInput(&distortion.input, this);
			auto soundout = new SoundOutput(&distortion, this);

			addChildWindow(ampin, leftOf(this), insideTop(this, 5));
			addChildWindow(roundin, leftOf(this), below(ampin, 5));
			addChildWindow(soundin, leftOf(this), below(roundin, 5));
			addChildWindow(soundout, rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Distortion", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::Distortion distortion;
	};
	fuiRegisterObject(DistortionObject, "distortion");
}