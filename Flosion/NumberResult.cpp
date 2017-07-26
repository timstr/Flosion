#pragma once

#include "NumberResult.h"

namespace musical {

	NumberResult::NumberResult(Stateful* owner) : input(nullptr, owner) {

	}
	double NumberResult::getValue(State* state, int chunk_pos, double default_value){
		return input.getValue(state, chunk_pos, default_value);
	}
	bool NumberResult::isSafeSource(NumberSource* source){
		return input.isSafeSource(source);
	}
	void NumberResult::setSource(NumberSource* source){
		input.setSource(source);
	}

}