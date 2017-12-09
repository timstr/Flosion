#pragma once

#include "FlosionUI.h"
#include "convolver.h"

namespace fui {

	struct ConvolverObject : ProcessingObject {
		ConvolverObject(){
			size = {150, 150};

			auto sin = new SoundInput(&convolver.input, this);
			auto sout = new SoundOutput(&convolver, this);

			auto pos = new NumberOutput(&convolver.input.position, this, "Position");
			auto fn = new NumberInput(&convolver.function.input, this, "Function");
			auto len = new NumberInput(&convolver.function_length.input, this, "Function Length");

			addChildWindow(pos, insideLeft(this, 5), above(this));
			addChildWindow(fn, leftOf(this), insideTop(this, 5));
			addChildWindow(len, leftOf(this), below(fn, 5));

			addChildWindow(sin, leftOf(this), below(len, 5));
			addChildWindow(sout, rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Convolver", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::Convolver convolver;
	};
	fuiRegisterObject(ConvolverObject, "convolver");

}