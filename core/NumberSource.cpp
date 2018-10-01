#pragma once

#include "NumberSource.h"

namespace flo {

	NumberSource::NumberSource(Stateful* _parent) NOEXCEPT_IF_I_SAY_SO
		: parent(_parent) {
		
	}
	NumberSource::~NumberSource() NOEXCEPT_IF_I_SAY_SO {
		for (auto& d : dependants){
			d->setSource(nullptr);
		}
		for (auto& in : inputs){
			in->setSource(nullptr);
		}
	}
	void NumberSource::findAllStatefulSources(std::vector<const Stateful*>& sources) const NOEXCEPT_IF_I_SAY_SO {
		if (parent){
			sources.push_back(parent);
		}
		for (int i = 0; i < inputs.size(); i++){
			inputs[i]->findAllStatefulSources(sources);
		}
	}
	void NumberSource::findAllStatefulDests(std::vector<const Stateful*>& dests) const NOEXCEPT_IF_I_SAY_SO {
		for (int i = 0; i < dependants.size(); i++){
			dependants[i]->findAllStatefulDests(dests);
		}
	}
	bool NumberSource::findStatelessDest() const NOEXCEPT_IF_I_SAY_SO {
		for (int i = 0; i < dependants.size(); i++){
			if (dependants[i]->findStatelessDest()){
				return true;
			}
		}
		return false;
	}

	PureFunction::PureFunction() NOEXCEPT_IF_I_SAY_SO : NumberSource(nullptr){

	}

	Constant::Constant(float _value) NOEXCEPT_IF_I_SAY_SO {
		value = _value;
	}
	void Constant::setValue(float val) NOEXCEPT_IF_I_SAY_SO {
		value = val;
	}
	float Constant::getValue() const NOEXCEPT_IF_I_SAY_SO {
		return value;
	}
	float Constant::evaluate(const State* state) const NOEXCEPT_IF_I_SAY_SO {
		return value;
	}
}