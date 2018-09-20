#pragma once

#include "FlosionUI.h"
#include "filter.h"

namespace fui {

	struct FilterObject : ProcessingObject {
		FilterObject(){
			size = {150, 150};

			addChildWindow(new SoundInput(&filter.input, this), leftOf(this), middleOfY(this));
			addChildWindow(new SoundOutput(&filter, this), rightOf(this), middleOfY(this));

			addChildWindow(new NumberOutput(&filter.frequency_out, this, "Frequency Out"), insideLeft(this), above(this));
			addChildWindow(new NumberInput(&filter.amplitude_in, this, "Amplitude In"), leftOf(this), insideTop(this));

			addChildWindow(new ui::Text("Filter", getFont()), middleOfX(this), middleOfY(this));
		}

		private:

		musical::Filter filter;
	};
	fuiRegisterObject(FilterObject, "filter", "eq");

}