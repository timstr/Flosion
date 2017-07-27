#pragma once

#include "NumberSource.h"

namespace musical {

	NumberSource::NumberSource(Stateful* _parent){
			parent = _parent;
		}
	NumberSource::~NumberSource(){
		for (int i = 0; i < dsts.size(); i++){
			dsts[i]->setSource(nullptr);
		}
		for (int i = 0; i < inputs.size(); i++){
			inputs[i]->setSource(nullptr);
		}
	}
	void NumberSource::addDstInput(NumberInput* dst){
		for (int i = 0; i < dsts.size(); i++){
			if (dsts[i] == dst){
				throw std::runtime_error("The destination NumberInput is already registered");
			}
		}
		dsts.push_back(dst);
	}
	void NumberSource::removeDstInput(NumberInput* dst){
		for (int i = 0; i < dsts.size(); i++){
			if (dsts[i] == dst){
				dsts.erase(dsts.begin() + i);
				return;
			}
		}
		throw std::runtime_error("The destination NumberInput could not be found");
	}
	void NumberSource::addInput(NumberInput* input){
		for (int i = 0; i < inputs.size(); i++){
			if (inputs[i] == input){
				throw std::runtime_error("The input is already registered");
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
		throw std::runtime_error("The input could not be found");
	}
	void NumberSource::findAllStatefulSources(std::vector<Stateful*>& sources){
		if (parent){
			sources.push_back(parent);
		}
		for (int i = 0; i < inputs.size(); i++){
			inputs[i]->findAllStatefulSources(sources);
		}
	}
	void NumberSource::findAllStatefulDests(std::vector<Stateful*>& dests){
		for (int i = 0; i < dsts.size(); i++){
			dsts[i]->findAllStatefulDests(dests);
		}
	}
	bool NumberSource::findStatelessDest(){
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
	float Constant::getValue(){
		return value;
	}
	float Constant::evaluate(State* state, int chunk_pos){
		return value;
	}
}