#pragma once

#include "FlosionUICore.h"
#include "functions.h"
#include "approacher.h"

namespace fui {

	struct AddObject : ProcessingObject {
		AddObject(){
			size = {75, 75};

			addNumberInput(new NumberInput(&add.input1, this, "Input A", {-5, 5}));
			addNumberInput(new NumberInput(&add.input2, this, "Input B", {-5, 40}));

			addNumberOutput(new NumberOutput(&add, this, {size.x - 25, 5}));

			addChildWindow(new ui::Text("Add", getFont()), {30, 5});
		}

		private:
		musical::Add add;
	}; 
	fuiRegisterObject(AddObject, "add", "+", "plus");

	struct SubtractObject : ProcessingObject {
		SubtractObject(){
			size = {75, 75};

			addNumberInput(new NumberInput(&subtract.input1, this, "Input A", {-5, 5}));
			addNumberInput(new NumberInput(&subtract.input2, this, "Input B", {-5, 40}));

			addNumberOutput(new NumberOutput(&subtract, this, {size.x - 25, 5}));

			addChildWindow(new ui::Text("Subtract", getFont()), {30, 5});
		}

		private:
		musical::Subtract subtract;
	}; 
	fuiRegisterObject(SubtractObject, "subtract", "-", "minus");

	struct MultiplyObject : ProcessingObject {
		MultiplyObject(){
			size = {75, 75};

			addNumberInput(new NumberInput(&multiply.input1, this, "Input A", {-5, 5}));
			addNumberInput(new NumberInput(&multiply.input2, this, "Input B", {-5, 40}));

			addNumberOutput(new NumberOutput(&multiply, this, {size.x - 25, 5}));

			addChildWindow(new ui::Text("Multiply", getFont()), {30, 5});
		}

		private:
		musical::Multiply multiply;
	}; 
	fuiRegisterObject(MultiplyObject, "multiply", "*", "x");

	struct DivideObject : ProcessingObject {
		DivideObject(){
			size = {75, 75};

			addNumberInput(new NumberInput(&divide.input1, this, "Input A", {-5, 5}));
			addNumberInput(new NumberInput(&divide.input2, this, "Input B", {-5, 40}));

			addNumberOutput(new NumberOutput(&divide, this, {size.x - 25, 5}));

			addChildWindow(new ui::Text("Divide", getFont()), {30, 5});
		}

		private:
		musical::Divide divide;
	}; 
	fuiRegisterObject(DivideObject, "divide", "/");

	struct UnitSineObject : ProcessingObject {
		UnitSineObject(){
			size = {250, 50};

			addNumberInput(new NumberInput(&usine.input, this, "Input", {-5, 10}));

			addNumberOutput(new NumberOutput(&usine, this, {225, 10}));

			addChildWindow(new ui::Text("Unit Sine", getFont()), {30, 5});
		}

		private:
		musical::UnitSine usine;
	}; 
	fuiRegisterObject(UnitSineObject, "usine", "usin", "unit sine", "unit sin");

	struct SawWaveObject : ProcessingObject {
		SawWaveObject(){
			size = {250, 50};

			addNumberInput(new NumberInput(&saw.input, this, "Input", {-5, 10}));

			addNumberOutput(new NumberOutput(&saw, this, {225, 10}));

			addChildWindow(new ui::Text("Saw Wave", getFont()), {30, 5});
		}

		private:
		musical::SawWave saw;
	}; 
	fuiRegisterObject(SawWaveObject, "saw", "sawwave", "saw wave");

	struct SquareWaveObject : ProcessingObject {
		SquareWaveObject(){
			size = {250, 50};

			addNumberInput(new NumberInput(&square.input, this, "Input", {-5, 10}));

			addNumberOutput(new NumberOutput(&square, this, {225, 10}));

			addChildWindow(new ui::Text("Square Wave", getFont()), {30, 5});
		}

		private:
		musical::SquareWave square;
	}; 
	fuiRegisterObject(SquareWaveObject, "square", "sqarewave", "square wave", "pulse", "pulse wave");

	struct ApproacherObject : ProcessingObject {
		ApproacherObject(){
			size = {250, 50};

			addNumberInput(new NumberInput(&approacher.input, this, "Input", {-5, 10}));

			addNumberOutput(new NumberOutput(&approacher, this, {225, 10}));

			addChildWindow(new ui::Text("Approacher", getFont()), {30, 5});
		}

		private:
		musical::Approacher approacher;
	}; 
	fuiRegisterObject(ApproacherObject, "approacher");



	struct DisplayObject : ProcessingObject {
		DisplayObject() : numres(nullptr) {
			size = {100, 30};
			addChildWindow(text = new ui::Text("-", getFont()), {35, 0});
			addNumberInput(new NumberInput(&numres.input, this, "Input", {0, 0}));
		}

		void render(sf::RenderWindow& rw, sf::Vector2f offset) override {
			text->setText(std::to_string(numres.getValue(nullptr, 0, 0)));
			ProcessingObject::render(rw, offset);
		}

		private:
		musical::NumberResult numres;
		ui::Text* text;
	};
	fuiRegisterObject(DisplayObject, "display", "show", "d0");

}