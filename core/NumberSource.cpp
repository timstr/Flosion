#pragma once

#include "NumberSource.h"

namespace musical {

	NumberSource::NumberSource(Stateful* _parent) noexcept
		: parent(_parent) {
		
	}
	NumberSource::~NumberSource() noexcept {
		for (auto& d : dependants){
			d->setSource(nullptr);
		}
		for (auto& in : inputs){
			in->setSource(nullptr);
		}
	}
	void NumberSource::findAllStatefulSources(std::vector<const Stateful*>& sources) const noexcept {
		if (parent){
			sources.push_back(parent);
		}
		for (int i = 0; i < inputs.size(); i++){
			inputs[i]->findAllStatefulSources(sources);
		}
	}
	void NumberSource::findAllStatefulDests(std::vector<const Stateful*>& dests) const noexcept {
		for (int i = 0; i < dependants.size(); i++){
			dependants[i]->findAllStatefulDests(dests);
		}
	}
	bool NumberSource::findStatelessDest() const noexcept {
		for (int i = 0; i < dependants.size(); i++){
			if (dependants[i]->findStatelessDest()){
				return true;
			}
		}
		return false;
	}

	PureFunction::PureFunction() noexcept : NumberSource(nullptr){

	}

	Constant::Constant(float _value) noexcept {
		value = _value;
	}
	void Constant::setValue(float val) noexcept {
		value = val;
	}
	float Constant::getValue() const noexcept {
		return value;
	}
	float Constant::evaluate(const State* state) const noexcept {
		return value;
	}
}