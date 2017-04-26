#pragma once

#include "musical.h"

namespace musical {


	struct Approacher : StatefulFunction<double> {
		Approacher() : input(this) {

		}

		double getValue(State* state, int chunk_pos, double& own_state) override {
			own_state += (input.getValue(state, chunk_pos) - own_state) * 0.01;
			return own_state;
		}

		NumberInput input;
	};

}