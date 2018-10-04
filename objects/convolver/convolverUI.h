#pragma once

#include "ObjectFactory.h"
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