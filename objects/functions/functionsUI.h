#pragma once

#include "FlosionUI.h"
#include "functions.h"

namespace fui {

	// add
	struct AddObject : ProcessingObject {
		AddObject(){
			size = {75, 75};

			auto in1 = new NumberInput(&add.input1, this, "Input A");
			auto in2 = new NumberInput(&add.input2, this, "Input B");

			addChildWindow(in1, leftOf(this), insideTop(this));
			addChildWindow(in2, leftOf(this), below(in1, 5));

			addChildWindow(new NumberOutput(&add, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Add", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Add add;
	}; 
	fuiRegisterObject(AddObject, "add", "+", "plus");

	// subtract
	struct SubtractObject : ProcessingObject {
		SubtractObject(){
			size = {75, 75};

			auto in1 = new NumberInput(&subtract.input1, this, "Input A");
			auto in2 = new NumberInput(&subtract.input2, this, "Input B");

			addChildWindow(in1, leftOf(this), insideTop(this));
			addChildWindow(in2, leftOf(this), below(in1, 5));

			addChildWindow(new NumberOutput(&subtract, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Subtract", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Subtract subtract;
	}; 
	fuiRegisterObject(SubtractObject, "subtract", "-", "minus");

	// multiply
	struct MultiplyObject : ProcessingObject {
		MultiplyObject(){
			size = {75, 75};

			auto in1 = new NumberInput(&multiply.input1, this, "Input A");
			auto in2 = new NumberInput(&multiply.input2, this, "Input B");

			addChildWindow(in1, leftOf(this), insideTop(this));
			addChildWindow(in2, leftOf(this), below(in1, 5));

			addChildWindow(new NumberOutput(&multiply, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Multiply", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Multiply multiply;
	}; 
	fuiRegisterObject(MultiplyObject, "multiply", "*", "x");

	// divide
	struct DivideObject : ProcessingObject {
		DivideObject(){
			size = {75, 75};

			auto in1 = new NumberInput(&divide.numerator, this, "Numerator");
			auto in2 = new NumberInput(&divide.denominator, this, "Denominator");

			addChildWindow(in1, leftOf(this), insideTop(this));
			addChildWindow(in2, leftOf(this), below(in1, 5));

			addChildWindow(new NumberOutput(&divide, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Divide", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Divide divide;
	}; 
	fuiRegisterObject(DivideObject, "divide", "/");

	// unit sine
	struct UnitSineObject : ProcessingObject {
		UnitSineObject(){
			size = {250, 50};

			addChildWindow(new NumberInput(&usine.input, this, "Input"), leftOf(this), middleOfY(this));

			addChildWindow(new NumberOutput(&usine, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Unit Sine", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::UnitSine usine;
	}; 
	fuiRegisterObject(UnitSineObject, "usin", "unit sine");

	// saw wave
	struct SawWaveObject : ProcessingObject {
		SawWaveObject(){
			size = {250, 50};

			addChildWindow(new NumberInput(&saw.input, this, "Input"), leftOf(this), middleOfY(this));

			addChildWindow(new NumberOutput(&saw, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Saw Wave", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::SawWave saw;
	}; 
	fuiRegisterObject(SawWaveObject, "saw", "saw wave");

	// square wave
	struct SquareWaveObject : ProcessingObject {
		SquareWaveObject(){
			size = {250, 50};

			addChildWindow(new NumberInput(&square.input, this, "Input"), leftOf(this), middleOfY(this));

			addChildWindow(new NumberOutput(&square, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Square Wave", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::SquareWave square;
	}; 
	fuiRegisterObject(SquareWaveObject, "square wave", "pulse wave");

	// gaussian bell curve
	struct GaussianObject : ProcessingObject {
		GaussianObject(){
			size = {100, 150};
			addChildWindow(new NumberInput(&gaussian.x, this, "X"), leftOf(this), insideTop(this));

			auto heightinput = new NumberInput(&gaussian.height, this, "Height");
			auto widthinput = new NumberInput(&gaussian.width, this, "Width");
			auto centerinput = new NumberInput(&gaussian.center, this, "Center");
			
			addChildWindow(heightinput, leftOf(this), insideBottom(this));
			addChildWindow(widthinput, leftOf(this), above(heightinput, 5.0f));
			addChildWindow(centerinput, leftOf(this), above(widthinput, 5.0f));

			addChildWindow(new NumberOutput(&gaussian, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Gaussian", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Gaussian gaussian;
	};
	fuiRegisterObject(GaussianObject, "gaussian", "bell curve");

	// natural log
	struct NaturalLogObject : ProcessingObject {
		NaturalLogObject(){
			size = {50, 30};

			addChildWindow(new NumberInput(&nlog.input, this, "Input"), leftOf(this), middleOfY(this));

			addChildWindow(new NumberOutput(&nlog, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Log", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::NaturalLog nlog;
	}; 
	fuiRegisterObject(NaturalLogObject, "ln", "logarithm", "natural logarithm");

	// exponential
	struct ExponentialObject : ProcessingObject {
		ExponentialObject(){
			size = {50, 30};

			addChildWindow(new NumberInput(&exponential.input, this, "Input"), leftOf(this), middleOfY(this));

			addChildWindow(new NumberOutput(&exponential, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Exp", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Exponential exponential;
	}; 
	fuiRegisterObject(ExponentialObject, "exponential");

	// minimum
	struct MinimumObject : ProcessingObject {
		MinimumObject(){
			size = {50, 50};

			auto in1 = new NumberInput(&minimum.input1, this, "Input A");
			auto in2 = new NumberInput(&minimum.input2, this, "Input B");

			addChildWindow(in1, leftOf(this), insideTop(this));
			addChildWindow(in2, leftOf(this), below(in1, 5));

			addChildWindow(new NumberOutput(&minimum, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Min", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Minimum minimum;
	}; 
	fuiRegisterObject(MinimumObject, "minimum");

	// maximum
	struct MaximumObject : ProcessingObject {
		MaximumObject(){
			size = {50, 50};

			auto in1 = new NumberInput(&maximum.input1, this, "Input A");
			auto in2 = new NumberInput(&maximum.input2, this, "Input B");

			addChildWindow(in1, leftOf(this), insideTop(this));
			addChildWindow(in2, leftOf(this), below(in1, 5));

			addChildWindow(new NumberOutput(&maximum, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Max", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Maximum maximum;
	}; 
	fuiRegisterObject(MaximumObject, "maximum");

	// clamp
	struct ClampObject : ProcessingObject {
		ClampObject(){
			size = {75, 50};

			auto in1 = new NumberInput(&clamp.input, this, "Input");
			auto in2 = new NumberInput(&clamp.minimum, this, "Minimum");
			auto in3 = new NumberInput(&clamp.maximum, this, "Maximum");

			addChildWindow(in1, leftOf(this), insideTop(this));
			addChildWindow(in2, leftOf(this), below(in1, 5));
			addChildWindow(in3, leftOf(this), below(in2, 5));

			addChildWindow(new NumberOutput(&clamp, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Clamp", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Clamp clamp;
	}; 
	fuiRegisterObject(ClampObject, "clamp");

	// phase
	struct PhaseObject : ProcessingObject {
		PhaseObject(){
			size = {50, 50};

			auto in1 = new NumberInput(&phase.x, this, "X");
			auto in2 = new NumberInput(&phase.y, this, "Y");

			addChildWindow(in1, leftOf(this), insideTop(this));
			addChildWindow(in2, leftOf(this), below(in1, 5));

			addChildWindow(new NumberOutput(&phase, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Phase", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Phase phase;
	}; 
	fuiRegisterObject(PhaseObject, "phase");

	// abs
	struct AbsoluteValueObject : ProcessingObject {
		AbsoluteValueObject(){
			size = {75, 30};

			addChildWindow(new NumberInput(&absval.input, this, "Input"), leftOf(this), middleOfY(this));

			addChildWindow(new NumberOutput(&absval, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Abs", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::AbsoluteValue absval;
	}; 
	fuiRegisterObject(AbsoluteValueObject, "absolute value");

	// hypotenuse
	struct HypotenuseObject : ProcessingObject {
		HypotenuseObject(){
			size = {50, 50};

			auto in1 = new NumberInput(&hypotenuse.input1, this, "Input A");
			auto in2 = new NumberInput(&hypotenuse.input2, this, "Input B");

			addChildWindow(in1, leftOf(this), insideTop(this));
			addChildWindow(in2, leftOf(this), below(in1, 5));

			addChildWindow(new NumberOutput(&hypotenuse, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Hypot", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Hypotenuse hypotenuse;
	}; 
	fuiRegisterObject(HypotenuseObject, "hypotenuse");

	// sine
	struct SineObject : ProcessingObject {
		SineObject(){
			size = {50, 30};

			addChildWindow(new NumberInput(&sine.input, this, "Input"), leftOf(this), middleOfY(this));

			addChildWindow(new NumberOutput(&sine, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Sin", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Sine sine;
	}; 
	fuiRegisterObject(SineObject, "sine");

	// cosine
	struct CosineObject : ProcessingObject {
		CosineObject(){
			size = {50, 30};

			addChildWindow(new NumberInput(&cosine.input, this, "Input"), leftOf(this), middleOfY(this));

			addChildWindow(new NumberOutput(&cosine, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Cos", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Cosine cosine;
	}; 
	fuiRegisterObject(CosineObject, "cosine");

	//tangent
	struct TangentObject : ProcessingObject {
		TangentObject(){
			size = {50, 30};

			addChildWindow(new NumberInput(&tangent.input, this, "Input"), leftOf(this), middleOfY(this));

			addChildWindow(new NumberOutput(&tangent, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Tan", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::functions::Tangent tangent;
	}; 
	fuiRegisterObject(TangentObject, "tangent");

	struct DisplayObject : ProcessingObject {
		DisplayObject() : numres(nullptr) {
			size = {100, 30};
			addChildWindow(text = new ui::Text("-", getFont()), middleOfX(this), middleOfY(this));
			addChildWindow(new NumberInput(&numres, this, "Input"), leftOf(this), middleOfY(this));
		}

		void render(sf::RenderWindow& rw) override {
			text->setText(toString(numres.getValue(nullptr, 0)));
			ProcessingObject::render(rw);
		}

		private:
		musical::NumberInput numres;
		ui::Text* text;
	};
	fuiRegisterObject(DisplayObject, "display", "d0");

}