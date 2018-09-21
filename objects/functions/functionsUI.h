#pragma once

#include "FlosionUI.h"
#include "functions.h"

namespace fui {

	// add
	struct AddObject : Object {
		AddObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_add.input1, "Input A");
			add<NumberInput>(self, m_add.input2, "Input B");
			add<NumberOutput>(self, m_add, "Output");
			add<ui::Text>("Add", getFont());
		}

	private:
		musical::functions::Add m_add;
	}; 
	RegisterFactoryObject(AddObject, "add", "+", "plus");

	// subtract
	struct SubtractObject : Object {
		SubtractObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_subtract.input1, "Input A");
			add<NumberInput>(self, m_subtract.input2, "Input B");
			add<NumberOutput>(self, m_subtract, "Output");
			add<ui::Text>("Subtract", getFont());
		}

	private:
		musical::functions::Subtract m_subtract;
	}; 
	RegisterFactoryObject(SubtractObject, "subtract", "-", "minus");

	// multiply
	struct MultiplyObject : Object {
		MultiplyObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_multiply.input1, "Input A");
			add<NumberInput>(self, m_multiply.input2, "Input B");
			add<NumberOutput>(self, m_multiply, "Output");
			add<ui::Text>("Multiply", getFont());
		}

	private:
		musical::functions::Multiply m_multiply;
	}; 
	RegisterFactoryObject(MultiplyObject, "multiply", "*", "x");

	// divide
	struct DivideObject : Object {
		DivideObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_divide.numerator, "Numerator");
			add<NumberInput>(self, m_divide.denominator, "Denominator");
			add<NumberOutput>(self, m_divide, "Output");
			add<ui::Text>("Divide", getFont());
		}

	private:
		musical::functions::Divide m_divide;
	}; 
	RegisterFactoryObject(DivideObject, "divide", "/");

	// unit sine
	struct UnitSineObject : Object {
		UnitSineObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_usine.input, "Input");
			add<NumberOutput>(self, m_usine, "Output");
			add<ui::Text>("usin", getFont());
		}

	private:
		musical::functions::UnitSine m_usine;
	}; 
	RegisterFactoryObject(UnitSineObject, "usin", "unit sine");

	// saw wave
	struct SawWaveObject : Object {
		SawWaveObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_saw.input, "Input");
			add<NumberOutput>(self, m_saw, "Output");
			add<ui::Text>("saw wave", getFont());
		}

	private:
		musical::functions::SawWave m_saw;
	}; 
	RegisterFactoryObject(SawWaveObject, "saw", "saw wave");

	// square wave
	struct SquareWaveObject : Object {
		SquareWaveObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_square.input, "Input");
			add<NumberOutput>(self, m_square, "Output");
			add<ui::Text>("square wave", getFont());
		}

	private:
		musical::functions::SquareWave m_square;
	}; 
	RegisterFactoryObject(SquareWaveObject, "square wave", "pulse wave");

	// gaussian bell curve
	struct GaussianObject : Object {
		GaussianObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_gaussian.height, "Height");
			add<NumberInput>(self, m_gaussian.width, "Width");
			add<NumberInput>(self, m_gaussian.center, "Center");
			add<NumberInput>(self, m_gaussian.input, "Input");
			add<NumberOutput>(self, m_gaussian, "Output");
			add<ui::Text>("Gaussian", getFont());
		}

	private:
		musical::functions::Gaussian m_gaussian;
	};
	RegisterFactoryObject(GaussianObject, "gaussian", "bell curve");

	// natural log
	struct NaturalLogObject : Object {
		NaturalLogObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_nlog.input, "Input");
			add<NumberOutput>(self, m_nlog, "Output");
			add<ui::Text>("ln", getFont());
		}

	private:
		musical::functions::NaturalLog m_nlog;
	}; 
	RegisterFactoryObject(NaturalLogObject, "ln", "logarithm", "natural logarithm");

	// exponential
	struct ExponentialObject : Object {
		ExponentialObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_exponential.input, "Input");
			add<NumberOutput>(self, m_exponential, "Output");
			add<ui::Text>("exp", getFont());
		}

	private:
		musical::functions::Exponential m_exponential;
	}; 
	RegisterFactoryObject(ExponentialObject, "exponential");

	// minimum
	struct MinimumObject : Object {
		MinimumObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_minimum.input1, "Input A");
			add<NumberInput>(self, m_minimum.input2, "Input B");
			add<NumberOutput>(self, m_minimum, "Output");
			add<ui::Text>("min", getFont());
		}

	private:
		musical::functions::Minimum m_minimum;
	}; 
	RegisterFactoryObject(MinimumObject, "minimum");

	// maximum
	struct MaximumObject : Object {
		MaximumObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_maximum.input1, "Input A");
			add<NumberInput>(self, m_maximum.input2, "Input B");
			add<NumberOutput>(self, m_maximum, "Output");
			add<ui::Text>("max", getFont());
		}

	private:
		musical::functions::Maximum m_maximum;
	}; 
	RegisterFactoryObject(MaximumObject, "maximum");

	// clamp
	struct ClampObject : Object {
		ClampObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_clamp.minimum, "Minimum");
			add<NumberInput>(self, m_clamp.maximum, "Maximum");
			add<NumberInput>(self, m_clamp.input, "Input");
			add<NumberOutput>(self, m_clamp, "Output");
			add<ui::Text>("clamp", getFont());
		}

	private:
		musical::functions::Clamp m_clamp;
	}; 
	RegisterFactoryObject(ClampObject, "clamp");

	// phase
	struct ArctangentObject : Object {
		ArctangentObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_atan.x, "X");
			add<NumberInput>(self, m_atan.y, "Y");
			add<NumberOutput>(self, m_atan, "Output");
			add<ui::Text>("max", getFont());
		}

	private:
		musical::functions::Arctangent m_atan;
	}; 
	RegisterFactoryObject(ArctangentObject, "atan", "arctangent");

	// abs
	struct AbsoluteValueObject : Object {
		AbsoluteValueObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_abs.input, "Input");
			add<NumberOutput>(self, m_abs, "Output");
			add<ui::Text>("abs", getFont());
		}

	private:
		musical::functions::AbsoluteValue m_abs;
	}; 
	RegisterFactoryObject(AbsoluteValueObject, "absolute value");

	// hypotenuse
	struct HypotenuseObject : Object {
		HypotenuseObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_hypot.x, "X");
			add<NumberInput>(self, m_hypot.y, "Y");
			add<NumberOutput>(self, m_hypot, "Output");
			add<ui::Text>("hypot", getFont());
		}

	private:
		musical::functions::Hypotenuse m_hypot;
	}; 
	RegisterFactoryObject(HypotenuseObject, "hypotenuse");

	// sine
	struct SineObject : Object {
		SineObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_sine.input, "Input");
			add<NumberOutput>(self, m_sine, "Output");
			add<ui::Text>("sin", getFont());
		}

	private:
		musical::functions::Sine m_sine;
	}; 
	RegisterFactoryObject(SineObject, "sine");

	// cosine
	struct CosineObject : Object {
		CosineObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_cosine.input, "Input");
			add<NumberOutput>(self, m_cosine, "Output");
			add<ui::Text>("cos", getFont());
		}

	private:
		musical::functions::Cosine m_cosine;
	}; 
	RegisterFactoryObject(CosineObject, "cosine");

	//tangent
	struct TangentObject : Object {
		TangentObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_tangent.input, "Input");
			add<NumberOutput>(self, m_tangent, "Output");
			add<ui::Text>("tan", getFont());
		}

	private:
		musical::functions::Tangent m_tangent;
	}; 
	RegisterFactoryObject(TangentObject, "tangent");

	struct DisplayObject : Object {
		DisplayObject() : m_input(nullptr) {
			auto self = thisAs<Object>();
			add<NumberInput>(self, m_input, "Input");
			m_text = add<ui::Text>("usin", getFont());
		}

		void render(sf::RenderWindow& rw) override {
			if (m_input.getSource() == nullptr){
				m_text->setText("-");
			} else {
				m_text->setText(std::to_string(m_input.getValue(nullptr, 0.0f)));
			}
			Object::render(rw);
		}

	private:
		musical::NumberInput m_input;
		ui::Ref<ui::Text> m_text;
	};
	RegisterFactoryObject(DisplayObject, "display", "d0"); // TODO: "d0" is a terrible name

}