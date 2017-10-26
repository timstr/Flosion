#pragma once

#include "FlosionUI.h"
#include "functions.h"
#include "NumberResult.h"

namespace fui {

	struct AddObject : ProcessingObject {
		AddObject(){
			size = {75, 75};

			auto in1 = new NumberInput(&add.input1, this, "Input A");
			auto in2 = new NumberInput(&add.input2, this, "Input B");

			addNumberInput(in1, leftOf(this), insideTop(this));
			addNumberInput(in2, leftOf(this), below(in1, 5));

			addNumberOutput(new NumberOutput(&add, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Add", getFont()), {30, 5});
		}

		private:
		musical::Add add;
	}; 
	fuiRegisterObject(AddObject, "add", "+", "plus");

	struct SubtractObject : ProcessingObject {
		SubtractObject(){
			size = {75, 75};

			auto in1 = new NumberInput(&subtract.input1, this, "Input A");
			auto in2 = new NumberInput(&subtract.input2, this, "Input B");

			addNumberInput(in1, leftOf(this), insideTop(this));
			addNumberInput(in2, leftOf(this), below(in1, 5));

			addNumberOutput(new NumberOutput(&subtract, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Subtract", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::Subtract subtract;
	}; 
	fuiRegisterObject(SubtractObject, "subtract", "-", "minus");

	struct MultiplyObject : ProcessingObject {
		MultiplyObject(){
			size = {75, 75};

			auto in1 = new NumberInput(&multiply.input1, this, "Input A");
			auto in2 = new NumberInput(&multiply.input2, this, "Input B");

			addNumberInput(in1, leftOf(this), insideTop(this));
			addNumberInput(in2, leftOf(this), below(in1, 5));

			addNumberOutput(new NumberOutput(&multiply, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Multiply", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::Multiply multiply;
	}; 
	fuiRegisterObject(MultiplyObject, "multiply", "*", "x");

	struct DivideObject : ProcessingObject {
		DivideObject(){
			size = {75, 75};

			auto in1 = new NumberInput(&divide.input1, this, "Input A");
			auto in2 = new NumberInput(&divide.input2, this, "Input B");

			addNumberInput(in1, leftOf(this), insideTop(this));
			addNumberInput(in2, leftOf(this), below(in1, 5));

			addNumberOutput(new NumberOutput(&divide, this, "Output"), rightOf(this), insideTop(this));

			addChildWindow(new ui::Text("Divide", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::Divide divide;
	}; 
	fuiRegisterObject(DivideObject, "divide", "/");

	struct UnitSineObject : ProcessingObject {
		UnitSineObject(){
			size = {250, 50};

			addNumberInput(new NumberInput(&usine.input, this, "Input"), leftOf(this), middleOfY(this));

			addNumberOutput(new NumberOutput(&usine, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Unit Sine", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::UnitSine usine;
	}; 
	fuiRegisterObject(UnitSineObject, "usin", "unit sine");

	struct SawWaveObject : ProcessingObject {
		SawWaveObject(){
			size = {250, 50};

			addNumberInput(new NumberInput(&saw.input, this, "Input"), leftOf(this), middleOfY(this));

			addNumberOutput(new NumberOutput(&saw, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Saw Wave", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::SawWave saw;
	}; 
	fuiRegisterObject(SawWaveObject, "saw", "saw wave");

	struct SquareWaveObject : ProcessingObject {
		SquareWaveObject(){
			size = {250, 50};

			addNumberInput(new NumberInput(&square.input, this, "Input"), leftOf(this), middleOfY(this));

			addNumberOutput(new NumberOutput(&square, this, "Output"), rightOf(this), middleOfY(this));

			addChildWindow(new ui::Text("Square Wave", getFont()), middleOfX(this), middleOfY(this));
		}

		private:
		musical::SquareWave square;
	}; 
	fuiRegisterObject(SquareWaveObject, "square wave", "pulse wave");



	struct DisplayObject : ProcessingObject {
		DisplayObject() : numres(nullptr) {
			size = {100, 30};
			addChildWindow(text = new ui::Text("-", getFont()), middleOfX(this), middleOfY(this));
			addNumberInput(new NumberInput(&numres.input, this, "Input"), leftOf(this), middleOfY(this));
		}

		void render(sf::RenderWindow& rw) override {
			text->setText(std::to_string(numres.getValue(nullptr, 0)));
			ProcessingObject::render(rw);
		}

		private:
		musical::NumberResult numres;
		ui::Text* text;
	};
	fuiRegisterObject(DisplayObject, "display", "d0");

}