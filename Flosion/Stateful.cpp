#pragma once

#include "Stateful.h"

namespace musical {

	Stateful::~Stateful(){

	}

	State::State(State* _parent, Stateful* _owner) : parent(_parent), owner(_owner) {

	}
	State::~State(){

	}

	State* State::getParentState() const {
		return parent;
	}
	Stateful* State::getOwner() const {
		return owner;
	};

}