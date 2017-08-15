#pragma once

#include "Stateful.h"
#include "NumberInput.h"
#include "NumberSource.h"

namespace musical {

	// for use in SoundProcessors and for display of stateless calculation results
	// owner shall point to either the stateful object that uses the results of this Number object
	// in its own stateful calculations, or owner shall be null, in which case this Number object
	// is stateless (and may only have global sources)
	struct NumberResult {
		NumberResult(Stateful* owner);

		float getValue(State* state, float default_value = 0.0);

		bool isSafeSource(NumberSource* source) const;

		void setSource(NumberSource* source);

		NumberInput input;
	};

}