#pragma once

#include "Stateful.h"
#include "NumberInput.h"
#include <algorithm>
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
		NumberSource(Stateful* _parent) noexcept;

		virtual ~NumberSource() noexcept;

		virtual float evaluate(const State* state) const noexcept = 0;
		
	private:

		void findAllStatefulSources(std::vector<const Stateful*>& sources) const noexcept;

		void findAllStatefulDests(std::vector<const Stateful*>& dests) const noexcept;

		bool findStatelessDest() const noexcept;

		Stateful* const parent;
		std::vector<NumberInput*> dependants;
		std::vector<NumberInput*> inputs;

		friend struct NumberInput;
	};

	struct PureFunction : NumberSource {
		PureFunction() noexcept;
	};

	struct Constant : PureFunction {
		Constant(float _value = 0.0) noexcept;

		void setValue(float val) noexcept;
		float getValue() const noexcept;

		float evaluate(const State* state) const noexcept override;

		private:
		float value;
	};
}