#pragma once

#include "Stateful.h"
#include "NumberInput.h"
#include <vector>

namespace musical {

	struct NumberInput;

	// NumberSource represents the source of a numerical value
	// Extended classes of NumberSource may be constant variables or functions of
	// other NumberSources using the NumberInput class as interfaces to these
	// -------------
	// parent		-	the owner of stateful information of this NumberSource
	//					This will be either a SoundSource or SoundMultiInput
	//					If the numbersource is not inherently stateful, such as in a global
	//					variable or pure math function, the parent shall be null

	struct NumberSource {
		NumberSource(Stateful* _parent);
		NumberSource(Stateful* _parent, float minimum, float maximum);
		virtual ~NumberSource();
		virtual float evaluate(State* state) const = 0;

		void setRange(float minimum, float maximum);
		bool hasRange() const;
		float getMinimum() const;
		float getMaximum() const;

		private:

		void addDstInput(NumberInput* dst);
		void removeDstInput(NumberInput* dst);

		void addInput(NumberInput* input);
		void removeInput(NumberInput* input);

		void findAllStatefulSources(std::vector<Stateful*>& sources) const;

		void findAllStatefulDests(std::vector<Stateful*>& dests) const;

		bool findStatelessDest() const;

		Stateful* parent;
		std::vector<NumberInput*> dsts;
		std::vector<NumberInput*> inputs;

		bool has_range;
		float range_minimum;
		float range_maximum;

		friend struct NumberInput;
	};

	struct PureFunction : NumberSource {
		PureFunction();
	};

	struct Constant : PureFunction {
		Constant(float _value = 0.0);

		void setValue(float val);
		float getValue() const;

		float evaluate(State* state) const override;

		private:
		float value;
	};
}