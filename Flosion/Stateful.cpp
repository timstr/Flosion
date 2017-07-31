#pragma once

#include "Stateful.h"

namespace musical {

	Stateful::~Stateful(){

	}

	float Stateful::getTimeSpeed(State* state_chain){
		return 1.0f;
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
	void State::skipTime(uint32_t t){
		time += t;
	}
	long double State::getTime(unsigned int offset){
		return (time + offset) / (long double)SFREQ;
	}
	long double State::getGlobalTime(unsigned int offset){
		State* s = this;
		while (true){
			offset /= s->owner->getTimeSpeed(s);
			if (s->parent == nullptr){
				return s->getTime(offset);
			}
			s = s->parent;
		}
	}

}