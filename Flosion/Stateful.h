#pragma once

#include <cstdint>

#include "musical.h"

namespace musical {

	struct Stateful;
	struct State;

	struct Stateful {
		virtual ~Stateful();

		virtual float getTimeSpeed(State* state_chain);

		virtual bool findSource(Stateful* src) const = 0;
	};

	struct State {
		State(State* _parent, Stateful* _owner);
		virtual ~State();

		virtual void reset() = 0;

		State* getParentState() const;
		Stateful* getOwner() const;

		void skipTime(uint32_t t);
		long double getTime(unsigned int offset);
		long double getGlobalTime(unsigned int offset);

		private:
		State* const parent;
		Stateful* const owner;
		uint32_t time;
	};

}