#pragma once

#include "NumberSource.h"
#include <vector>

namespace musical {

	struct NumberSource;

	//for use inside NumberSource classes
	struct NumberInput {
		NumberInput(NumberSource* _parent, Stateful* _owner = nullptr);
		~NumberInput();

		bool isSafeSource(NumberSource* src);

		void setSource(NumberSource* _source);

		void findAllStatefulSources(std::vector<Stateful*>& sources);

		void findAllStatefulDests(std::vector<Stateful*>& dests);

		bool findStatelessDest();

		// state shall be the current contextual state chain
		// any allowable values shall come from global constants which are not stateful
		// or from sound processing objects, which shall be found as parents of some state
		// in the state chain (accessed through the line of parents in the given state)
		// this state is to be passed on as-is to preserve its full context
		float getValue(State* state, unsigned int chunk_pos, float default_value = 0);

		private:
		NumberSource* parent;
		Stateful* owner;
		NumberSource* source;
	};

}