#pragma once

#include "Stateful.h"
#include <algorithm>

namespace flo {

	Stateful::~Stateful() {
		while (dependencies.size() > 0){
			removeDependency(dependencies.back());
		}
		while (dependants.size() > 0){
			dependants.back()->removeDependency(this);
		}
	}

	double Stateful::getTimeSpeed(const State* state_chain) const NOEXCEPT_IF_I_SAY_SO {
		return 1.0;
	}
	
	uint32_t Stateful::getTime(const State* state_chain) const NOEXCEPT_IF_I_SAY_SO {
		return state_chain->getTimeAt(this);
	}

	bool Stateful::hasDependency(const Stateful* s) const NOEXCEPT_IF_I_SAY_SO {
		if (hasImmediateDependency(s)){
			return true;
		}
		for (const auto& d : dependencies){
			if (d->hasDependency(s)){
				return true;
			}
		}
		return false;
	}

	bool Stateful::hasImmediateDependency(const Stateful* s) const NOEXCEPT_IF_I_SAY_SO {
		for (const auto& d : dependencies){
			if (d == s){
				return true;
			}
		}
		return false;
	}

	bool Stateful::hasDependant(const Stateful* s) const NOEXCEPT_IF_I_SAY_SO {
		if (hasImmediateDependant(s)){
			return true;
		}
		for (const auto& d : dependants){
			if (d->hasDependant(s)){
				return true;
			}
		}
		return false;
	}

	bool Stateful::hasImmediateDependant(const Stateful* s) const NOEXCEPT_IF_I_SAY_SO {
		for (const auto& d : dependants){
			if (d == s){
				return true;
			}
		}
		return false;
	}

	void Stateful::addDependency(Stateful* d) NOEXCEPT_IF_I_SAY_SO {
		assert(std::find(dependencies.begin(), dependencies.end(), d) == dependencies.end());
		assert(std::find(d->dependants.begin(), d->dependants.end(), this) == d->dependants.end());

		dependencies.push_back(d);
		d->dependants.push_back(this);
	}

	void Stateful::removeDependency(Stateful* d) NOEXCEPT_IF_I_SAY_SO {
		assert(std::find(dependencies.begin(), dependencies.end(), d) != dependencies.end());
		assert(std::find(d->dependants.begin(), d->dependants.end(), this) != d->dependants.end());

		dependencies.erase(std::remove(dependencies.begin(), dependencies.end(), d), dependencies.end());
		d->dependants.erase(std::remove(d->dependants.begin(), d->dependants.end(), this), d->dependants.end());
	}



	State::State(const State* _parent, const Stateful* _owner) :
		parent(_parent),
		owner(_owner),
		total_time(0),
		time_offset(0) {

	}
	
	State::~State() {

	}
	
	void State::performReset() NOEXCEPT_IF_I_SAY_SO {
		total_time = 0;
		time_offset = 0;
		reset();
	}
	
	const State* State::getParentState() const NOEXCEPT_IF_I_SAY_SO {
		return parent;
	}
	
	const Stateful* State::getOwner() const NOEXCEPT_IF_I_SAY_SO {
		return owner;
	}
	void State::advanceTime(uint32_t samples) NOEXCEPT_IF_I_SAY_SO {
		time_offset += samples;
	}
	void State::setTimeSinceSave(uint32_t samples) NOEXCEPT_IF_I_SAY_SO {
		time_offset = samples;
	}
	void State::saveTime() NOEXCEPT_IF_I_SAY_SO {
		total_time += time_offset;
		time_offset = 0;
	}
	void State::revertToSavedTime() NOEXCEPT_IF_I_SAY_SO {
		time_offset = 0;
	}

	void State::setTotalTimeTo(uint32_t samples) NOEXCEPT_IF_I_SAY_SO {
		total_time = samples;
		time_offset = 0;
	}
	
	uint32_t State::getTime() const NOEXCEPT_IF_I_SAY_SO {
		return total_time + time_offset;
	}
	
	uint32_t State::getTimeAt(const Stateful* stateful) const NOEXCEPT_IF_I_SAY_SO {
		const State* s = this;
		double offset = time_offset;
		while (s) {
			const double speed = s->owner->getTimeSpeed(s);
			offset *= (speed == 0.0) ? 0.0 : (1.0 / speed);
			if (s->owner == stateful) {
				return s->total_time + (uint32_t)offset;
			}
			s = s->parent;
		}
		return 0;
	}
	
	uint32_t State::getGlobalTime() const NOEXCEPT_IF_I_SAY_SO {
		const State* s = this;
		double offset = time_offset;
		while (true) {
			const double speed = s->owner->getTimeSpeed(s);
			offset *= (speed == 0.0) ? 0.0 : (1.0 / speed);
			if (s->parent == nullptr) {
				return s->total_time + (uint32_t)offset;
			}
			s = s->parent;
		}
		return 0;
	}

}