#pragma once

#include "spline.h"
#include "ObjectFactory.h"

namespace fui {

	struct SplineObject : Object {
		SplineObject(){
			setSize({200, 100}, true);
			auto self = this->thisAs<Object>();
			add<NumberInput>(self, spline.input, "Input");
			add<NumberOutput>(self, spline, "Output");
		}

		// TODO

		private:

	flo::Spline spline;
	};
	RegisterFactoryObject(SplineObject, "spline");
};