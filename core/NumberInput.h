#pragma once

#include "NumberSource.h"
#include <vector>

namespace musical {

	struct NumberSource;

	//for use inside NumberSource classes
	struct NumberInput {
		NumberInput(NumberSource* _parent, Stateful* _owner = nullptr);
		NumberInput(NumberSource* _parent, float minimum, float maximum, Stateful* _owner = nullptr);
		~NumberInput();

		bool isSafeSource(NumberSource* src) const;

		void setSource(NumberSource* _source);

		void findAllStatefulSources(std::vector<Stateful*>& sources) const;

		void findAllStatefulDests(std::vector<Stateful*>& dests) const;

		bool findStatelessDest() const;

		void setRange(float minimum, float maximum);
		bool hasRange() const;
		float getMinimum() const;
		float getMaximum() const;

		// state shall be the current contextual state chain
		// any allowable values shall come from global constants which are not stateful
		// or from sound processing objects, which shall be found as parents of some state
		// in the state chain (accessed through the line of parents in the given state)
		// this state is to be passed on as-is to preserve its full context
		float getValue(State* state, float default_value = 0) const;

		private:
		NumberSource* parent;
		Stateful* owner;
		NumberSource* source;

		bool has_range;
		float range_minimum;
		float range_maximum;
	};

}