#pragma once

#include "Stateful.h"
#include "NumberInput.h"
#include <vector>

namespace flo {

	struct NumberInput;

	// TODO: cleanup API between this and NumberInput

	// NumberSource represents the source of a numerical value
	// Extended classes of NumberSource may be constant variables or functions of
	// other NumberSources using the NumberInput class as interfaces to these
	// -------------
	// parent		-	the owner of stateful information of this NumberSource
	//					This will be either a SoundSource or SoundMultiInput
	//					If the numbersource is not inherently stateful, such as in a global
	//					variable or pure math function, the parent shall be null

	struct NumberSource {
		NumberSource(Stateful* _parent) NOEXCEPT_IF_I_SAY_SO;

		virtual ~NumberSource() NOEXCEPT_IF_I_SAY_SO;

		virtual float evaluate(const State* state) const NOEXCEPT_IF_I_SAY_SO = 0;
		
	private:

		void findAllStatefulSources(std::vector<const Stateful*>& sources) const NOEXCEPT_IF_I_SAY_SO;

		void findAllStatefulDests(std::vector<const Stateful*>& dests) const NOEXCEPT_IF_I_SAY_SO;

		bool findStatelessDest() const NOEXCEPT_IF_I_SAY_SO;

		Stateful* const parent;
		std::vector<NumberInput*> dependants;
		std::vector<NumberInput*> inputs;

		friend struct NumberInput;
	};

	struct PureFunction : NumberSource {
		PureFunction() NOEXCEPT_IF_I_SAY_SO;
	};

	struct Constant : PureFunction {
		Constant(float _value = 0.0) NOEXCEPT_IF_I_SAY_SO;

		void setValue(float val) NOEXCEPT_IF_I_SAY_SO;
		float getValue() const NOEXCEPT_IF_I_SAY_SO;

		float evaluate(const State* state) const NOEXCEPT_IF_I_SAY_SO override;

		private:
		float value;
	};
}