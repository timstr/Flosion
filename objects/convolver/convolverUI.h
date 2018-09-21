#pragma once

#include "FlosionUI.h"
#include "convolver.h"

namespace fui {

	struct ConvolverObject : Object {
		ConvolverObject(){
			// TODO
		}

		private:
		musical::Convolver convolver;
	};
	RegisterFactoryObject(ConvolverObject, "convolver");

}