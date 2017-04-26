#pragma once

#include "musical.h"
#include <math.h>

namespace musical {

	struct UnaryPureFunction : PureFunction {
		UnaryPureFunction() : input(this) {

		}

		NumberInput input;
	};

	struct BinaryPureFunction : PureFunction {
		BinaryPureFunction() : input1(this), input2(this) {

		}

		NumberInput input1, input2;
	};

	struct Add : BinaryPureFunction {
		double evaluate(State* state, int chunk_pos) override {
			return (input1.getValue(state, chunk_pos, 0) + input2.getValue(state, chunk_pos, 0));
		}
	};

	struct Subtract : BinaryPureFunction {
		double evaluate(State* state, int chunk_pos) override {
			return (input1.getValue(state, chunk_pos, 0) - input2.getValue(state, chunk_pos, 0));
		}
	};

	struct Multiply : BinaryPureFunction {
		double evaluate(State* state, int chunk_pos) override {
			return (input1.getValue(state, chunk_pos, 1) * input2.getValue(state, chunk_pos, 1));
		}
	};

	struct Divide : BinaryPureFunction {
		double evaluate(State* state, int chunk_pos) override {
			return (input1.getValue(state, chunk_pos, 1) / input2.getValue(state, chunk_pos, 1));
		}
	};

	struct UnitSine : UnaryPureFunction {
		double evaluate(State* state, int chunk_pos) override {
			return sin(input.getValue(state, chunk_pos) * 2 * PI);
		}
	};

	struct SawWave : UnaryPureFunction {
		double evaluate(State* state, int chunk_pos) override {
			double val = input.getValue(state, chunk_pos, 0.5);
			return (val - floor(val)) * 2 - 1;
		}
	};

	struct SquareWave : UnaryPureFunction {
		double evaluate(State* state, int chunk_pos) override {
			double val = input.getValue(state, chunk_pos, 0.5);
			if ((val - floor(val)) < 0.5){
				return -1;
			} else {
				return 1;
			}
		}
	};
}