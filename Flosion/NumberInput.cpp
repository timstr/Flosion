#pragma once

#include "NumberInput.h"

namespace musical {

	NumberInput::NumberInput(NumberSource* _parent, Stateful* _owner){
		if (_parent && _owner){
			throw std::runtime_error("Time to rethink what you're doing.");
		}
		parent = _parent;
		if (parent){
			parent->addInput(this);
		}
		owner = _owner;
		source = nullptr;
	}
	NumberInput::~NumberInput(){
		if (parent){
			parent->removeInput(this);
		}
		if (source){
			source->removeDstInput(this);
		}
	}
	bool NumberInput::isSafeSource(NumberSource* src){
		if (src == nullptr){
			return true;
		}

		std::vector<Stateful*> stateful_sources;
		src->findAllStatefulSources(stateful_sources);
		if (stateful_sources.size() == 0){
			return true;
		} else {
			std::vector<Stateful*> stateful_dests;
			findAllStatefulDests(stateful_dests);
			if (stateful_dests.size() == 0){
				return !findStatelessDest();
			} else {
				for (Stateful* src : stateful_sources){
					for (Stateful* dst: stateful_dests){
						if (!src->findSource(dst)){
							return false;
						}
					}
				}
				return true;
			}
		}
	}
	void NumberInput::setSource(NumberSource* _source){
		if (!isSafeSource(_source)){
			throw std::runtime_error("The numbersource is not safe to connect; it violates the integrity of stateful information");
		}

		if (source){
			source->removeDstInput(this);
		}
		source = _source;
		if (source){
			source->addDstInput(this);
		}
	}
	void NumberInput::findAllStatefulSources(std::vector<Stateful*>& sources){
		if (source){
			source->findAllStatefulSources(sources);
		}
	}
	void NumberInput::findAllStatefulDests(std::vector<Stateful*>& dests){
		if (owner){
			dests.push_back(owner);
		}
		if (parent){
			parent->findAllStatefulDests(dests);
		}
	}
	bool NumberInput::findStatelessDest(){
		if (parent){
			return parent->findStatelessDest();
		} else if (owner){
			return false;
		} else {
			return true;
		}
	}

	// state shall be the current contextual state chain
	// any allowable values shall come from global constants which are not stateful
	// or from sound processing objects, which shall be found as parents of some state
	// in the state chain (accessed through the line of parents in the given state)
	// this state is to be passed on as-is to preserve its full context
	float NumberInput::getValue(State* state, unsigned int chunk_pos, float default_value){
		if (source){
			return source->evaluate(state, chunk_pos);
		} else {
			return default_value;
		}
	}

}