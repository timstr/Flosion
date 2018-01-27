#pragma once

#include "musical.h"
#include <math.h>
#include "pi.h"

namespace musical {

	namespace functions {

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

		struct Divide : PureFunction {
			Divide() : numerator(this), denominator(this) {

			}

			float evaluate(State* state) const override {
				return (numerator.getValue(state, 1) / denominator.getValue(state, 1));
			}

			NumberInput numerator, denominator;
		};

		struct UnitSine : UnaryPureFunction {
			float evaluate(State* state) const override {
				return sin(input.getValue(state) * 2 * pi<float>);
			}
		};

		struct SawWave : UnaryPureFunction {
			float evaluate(State* state) const override {
				float val = input.getValue(state, 0.5);
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

		struct NaturalLog : UnaryPureFunction {
			float evaluate(State* state) const override {
				return log(input.getValue(state));
			}
		};

		struct Exponential : UnaryPureFunction {
			float evaluate(State* state) const override {
				return exp(input.getValue(state));
			}
		};

		struct Minimum : BinaryPureFunction {
			float evaluate(State* state) const override {
				return std::min(input1.getValue(state), input2.getValue(state));
			}
		};

		struct Maximum : BinaryPureFunction {
			float evaluate(State* state) const override {
				return std::max(input1.getValue(state), input2.getValue(state));
			}
		};

		struct Clamp : PureFunction {
			Clamp() : input(this), minimum(this), maximum(this) {
			
			}

			float evaluate(State* state) const override {
				return std::min(std::max(input.getValue(state), minimum.getValue(state)), maximum.getValue(state));
			}

			NumberInput input, minimum, maximum;
		};

		struct Phase : PureFunction {
			Phase() : x(this), y(this) {

			}

			float evaluate(State* state) const override {
				return atan2(y.getValue(state), x.getValue(state));
			}

			NumberInput x, y;
		};

		struct AbsoluteValue : UnaryPureFunction {
			float evaluate(State* state) const override {
				return abs(input.getValue(state));
			}
		};

		struct Hypotenuse : BinaryPureFunction {
			float evaluate(State* state) const override {
				return hypot(input1.getValue(state), input2.getValue(state));
			}
		};

		struct Sine : UnaryPureFunction {
			float evaluate(State* state) const override {
				return sin(input.getValue(state));
			}
		};

		struct Cosine : UnaryPureFunction {
			float evaluate(State* state) const override {
				return cos(input.getValue(state));
			}
		};

		struct Tangent : UnaryPureFunction {
			float evaluate(State* state) const override {
				return cos(input.getValue(state));
			}
		};

	}

}