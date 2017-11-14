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
		float evaluate(State* state) const override {
			return (input1.getValue(state, 0) + input2.getValue(state, 0));
		}
	};

	struct Subtract : BinaryPureFunction {
		float evaluate(State* state) const override {
			return (input1.getValue(state, 0) - input2.getValue(state, 0));
		}
	};

	struct Multiply : BinaryPureFunction {
		float evaluate(State* state) const override {
			return (input1.getValue(state, 1) * input2.getValue(state, 1));
		}
	};

	struct Divide : BinaryPureFunction {
		float evaluate(State* state) const override {
			return (input1.getValue(state, 1) / input2.getValue(state, 1));
		}
	};

	struct UnitSine : UnaryPureFunction {
		float evaluate(State* state) const override {
			return sin(input.getValue(state) * 2 * PI);
		}
	};

	struct SawWave : UnaryPureFunction {
		float evaluate(State* state) const override {
			double val = input.getValue(state, 0.5);
			return (val - floor(val)) * 2 - 1;
		}
	};

	struct SquareWave : UnaryPureFunction {
		float evaluate(State* state) const override {
			float val = input.getValue(state, 0.5);
			if ((val - floor(val)) < 0.5){
				return -1;
			} else {
				return 1;
			}
		}
	};

	struct Gaussian : PureFunction {
		Gaussian() : x(this), center(this), width(this), height(this) {

		}

		float evaluate(State* state) const override {
			float xval = x.getValue(state, 0.0f);
			float centerval = center.getValue(state, 0.0f);
			float widthval = width.getValue(state, 1.0f);
			float heightval = height.getValue(state, 1.0f);
			return exp(-pow(xval - centerval, 2.0f) / (2.0f * widthval * widthval)) * heightval;
		}

		NumberInput x;
		NumberInput center;
		NumberInput width;
		NumberInput height;
	};
}