#pragma once

#include "FlosionUI.h"
#include "convolver.h"

namespace fui {

	struct ConvolverObject : Object {
		ConvolverObject(){
			// TODO
		}

		private:
		flo::Convolver convolver;
	};
	RegisterFactoryObject(ConvolverObject, "convolver");

}