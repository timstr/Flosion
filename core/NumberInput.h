#pragma once

#include "Stateful.h"
#include <vector>

namespace musical {

	struct NumberSource;

	// A NumberInput can be connected to a NumberSource and supply its value
	// to either another NumberSource, a generic Stateful object, or for global, stateless usage
	struct NumberInput {

		// create a NumberInput belonging to a NumberSource
		NumberInput(NumberSource* _owner) noexcept;

		// create a NumberInput belonging to a Stateful object
		NumberInput(Stateful* _owner) noexcept;

		// create a NumberInput belonging to the global, stateless scope
		// to be used as NumberInput(nullptr)
		NumberInput(nullptr_t) noexcept;

		~NumberInput() noexcept;

		// Returns true if a given source would be safe to connect to.
		// The source would be unsafe if it needed state information
		// that is inaccessible to the context of this NumberInput
		bool isSafeSource(const NumberSource* src) const noexcept;

		// will throw if _source is not safe (see isSafeSource())
		// will not throw if given nullptr
		void setSource(NumberSource* _source);

		NumberSource* getSource() const;

		// state shall be the current contextual state chain
		// any allowable values shall come from global constants which are not stateful
		// or from sound processing objects, which shall be found as parents of some state
		// in the state chain (accessed through the line of parents in the given state)
		// this state is to be passed on as-is to preserve its full context
		float getValue(const State* state, float default_value = 0) const noexcept;

	private:

		void findAllStatefulSources(std::vector<const Stateful*>& sources) const noexcept;

		void findAllStatefulDests(std::vector<const Stateful*>& dests) const noexcept;

		bool findStatelessDest() const noexcept;

		NumberSource* owner_numbersource;
		Stateful* owner_stateful;
		NumberSource* source;

		friend struct NumberSource;
	};

}