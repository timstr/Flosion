#pragma once

#include "Stateful.h"

namespace musical {

	Stateful::~Stateful(){

	}



	State::State(State* _parent, Stateful* _owner) : parent(_parent), owner(_owner) {
		time = 0;
	}
	State::~State(){

	}
	State* State::getParentState() const {
		return parent;
	}
	Stateful* State::getOwner() const {
		return owner;
	};
	void State::tickTime(){
		time += 1;
	}
	void State::skipTime(uint32_t t){
		time += t;
	}

}