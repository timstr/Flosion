#pragma once

#include "NumberSource.h"

namespace musical {

	NumberSource::NumberSource(Stateful* _parent){
		parent = _parent;
		has_range = false;
	}
	NumberSource::NumberSource(Stateful* _parent, float minimum, float maximum)
		: NumberSource(_parent) {
		setRange(minimum, maximum);
	}
	NumberSource::~NumberSource(){
		for (int i = 0; i < dsts.size(); i++){
			dsts[i]->setSource(nullptr);
		}
		for (int i = 0; i < inputs.size(); i++){
			inputs[i]->setSource(nullptr);
		}
	}
	void NumberSource::setRange(float minimum, float maximum){
		has_range = true;
		range_minimum = minimum;
		range_maximum = maximum;
	}
	bool NumberSource::hasRange() const {
		return has_range;
	}
	float NumberSource::getMinimum() const {
		return range_minimum;
	}
	float NumberSource::getMaximum() const {
		return range_maximum;
	}
	void NumberSource::addDstInput(NumberInput* dst){
		for (int i = 0; i < dsts.size(); i++){
			if (dsts[i] == dst){
				return;
			}
		}
		dsts.push_back(dst);
	}
	void NumberSource::removeDstInput(NumberInput* dst){
		for (int i = 0; i < dsts.size(); i++){
			if (dsts[i] == dst){
				dsts.erase(dsts.begin() + i);
				dst->setSource(nullptr);
				return;
			}
		}
	}
	void NumberSource::addInput(NumberInput* input){
		for (int i = 0; i < inputs.size(); i++){
			if (inputs[i] == input){
				return;
			}
		}
		inputs.push_back(input);
	}
	void NumberSource::removeInput(NumberInput* input){
		for (int i = 0; i < inputs.size(); i++){
			if (inputs[i] == input){
				inputs.erase(inputs.begin() + i);
				return;
			}
		}
	}
	void NumberSource::findAllStatefulSources(std::vector<Stateful*>& sources) const {
		if (parent){
			sources.push_back(parent);
		}
		for (int i = 0; i < inputs.size(); i++){
			inputs[i]->findAllStatefulSources(sources);
		}
	}
	void NumberSource::findAllStatefulDests(std::vector<Stateful*>& dests) const {
		for (int i = 0; i < dsts.size(); i++){
			dsts[i]->findAllStatefulDests(dests);
		}
	}
	bool NumberSource::findStatelessDest() const {
		for (int i = 0; i < dsts.size(); i++){
			if (dsts[i]->findStatelessDest()){
				return true;
			}
		}
		return false;
	}

	PureFunction::PureFunction() : NumberSource(nullptr){

	}

	Constant::Constant(float _value){
		value = _value;
	}
	void Constant::setValue(float val){
		value = val;
	}
	float Constant::getValue() const {
		return value;
	}
	float Constant::evaluate(State* state) const {
		return value;
	}
}