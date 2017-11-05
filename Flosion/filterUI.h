#pragma once

#include "FlosionUI.h"
#include "filter.h"

namespace fui {

	struct FilterObject : ProcessingObject {
		FilterObject(){
			size = {100, 100};
			addChildWindow(new SoundInput(&filter.input, this), leftOf(this), middleOfY(this));
			addChildWindow(new SoundOutput(&filter, this), rightOf(this), middleOfY(this));
			addChildWindow(new ui::Text("Filter", getFont()), middleOfX(this), middleOfY(this));
		}

		private:

		musical::Filter filter;
	};
	fuiRegisterObject(FilterObject, "filter", "eq");

}