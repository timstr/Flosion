#pragma once

#include "spline.h"
#include "FlosionUI.h"

namespace fui {

	struct SplineObject : Object {
		SplineObject(){
			setSize({200, 100}, true);
			auto self = thisAs<Object>();
			add<NumberInput>(self, spline.input, "Input");
			add<NumberOutput>(self, spline, "Output");
		}

		// TODO

		private:

	flo::Spline spline;
	};
	RegisterFactoryObject(SplineObject, "spline");
};