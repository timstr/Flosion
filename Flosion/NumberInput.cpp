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
		has_range = false;
	}
	NumberInput::NumberInput(NumberSource* _parent, float minimum, float maximum, Stateful* _owner)
		: NumberInput(_parent, _owner) {
		setRange(minimum, maximum);
	}
	NumberInput::~NumberInput(){
		if (source){
			source->removeDstInput(this);
		}
		if (parent){
			parent->removeInput(this);
		}
	}
	bool NumberInput::isSafeSource(NumberSource* src) const {
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
		NumberSource* prev = source;
		source = _source;
		if (prev){
			prev->removeDstInput(this);
		}
		if (source){
			source->addDstInput(this);
		}
	}
	void NumberInput::findAllStatefulSources(std::vector<Stateful*>& sources) const {
		if (source){
			source->findAllStatefulSources(sources);
		}
	}
	void NumberInput::findAllStatefulDests(std::vector<Stateful*>& dests) const {
		if (owner){
			dests.push_back(owner);
		}
		if (parent){
			parent->findAllStatefulDests(dests);
		}
	}
	bool NumberInput::findStatelessDest() const {
		if (parent){
			return parent->findStatelessDest();
		} else if (owner){
			return false;
		} else {
			return true;
		}
	}
	void NumberInput::setRange(float minimum, float maximum){
		has_range = true;
		range_minimum = minimum;
		range_maximum = maximum;
	}
	bool NumberInput::hasRange() const {
		return has_range;
	}
	float NumberInput::getMinimum() const {
		return range_minimum;
	}
	float NumberInput::getMaximum() const {
		return range_maximum;
	}

	// state shall be the current contextual state chain
	// any allowable values shall come from global constants which are not stateful
	// or from sound processing objects, which shall be found as parents of some state
	// in the state chain (accessed through the line of parents in the given state)
	// this state is to be passed on as-is to preserve its full context
	float NumberInput::getValue(State* state, float default_value) const {
		if (source){
			return source->evaluate(state);
		} else {
			return default_value;
		}
	}

}