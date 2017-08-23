#pragma once

#include "Stateful.h"

namespace musical {

	Stateful::~Stateful(){

	}

	float Stateful::getTimeSpeed(State* state_chain){
		return 1.0f;
	}
	long double Stateful::getTime(State* state_chain){
		return state_chain->getTimeAt(this);
	}


	State::State(State* _parent, Stateful* _owner) : parent(_parent), owner(_owner) {
		time = 0;
	}
	State::~State(){

	}
	void State::performReset(){
		time = 0;
		time_offset = 0;
		reset();
	}
	State* State::getParentState() const {
		return parent;
	}
	Stateful* State::getOwner() const {
		return owner;
	};
	void State::tick(){
		time_offset += 1;
	}
	void State::skipTime(uint32_t t){
		time_offset += t;
	}
	void State::commitTime(){
		time += time_offset;
		time_offset = 0;
	}
	long double State::getTime() const{
		return (time + time_offset) / (long double)SFREQ;
	}
	long double State::getTimeAt(Stateful* stateful){
		State* s = this;
		double offset = time_offset;
		while (s){
			offset /= s->owner->getTimeSpeed(s);
			if (s->owner == stateful){
				return (s->time + offset) / (long double)SFREQ;
			}
			s = s->parent;
		}
	}
	long double State::getGlobalTime(){
		State* s = this;
		double offset = time_offset;
		while (true){
			offset /= s->owner->getTimeSpeed(s);
			if (s->parent == nullptr){
				return (s->time + offset) / (long double)SFREQ;
			}
			s = s->parent;
		}
	}

}