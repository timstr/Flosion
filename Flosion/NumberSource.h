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
		virtual ~NumberSource();
		virtual float evaluate(State* state, int chunk_pos) const = 0;

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

		friend struct NumberInput;
	};

	struct PureFunction : NumberSource {
		PureFunction();
	};

	struct Constant : PureFunction {
		Constant(float _value = 0.0);

		void setValue(float val);
		float getValue() const;

		float evaluate(State* state, int chunk_pos) const override;

		private:
		float value;
	};
}