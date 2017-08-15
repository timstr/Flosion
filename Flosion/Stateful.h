#pragma once

#include <cstdint>

#include "musical.h"

namespace musical {

	struct Stateful;
	struct State;

	struct Stateful {
		virtual ~Stateful();

		virtual float getTimeSpeed(State* state_chain);

		long double getTime(State* state_chain);

		virtual bool findSource(Stateful* src) const = 0;
	};

	struct State {
		State(State* _parent, Stateful* _owner);
		virtual ~State();

		virtual void reset() = 0;

		State* getParentState() const;
		Stateful* getOwner() const;

		void tick();
		void skipTime(uint32_t t);
		void commitTime();
		long double getTime() const;
		long double getTimeAt(Stateful* stateful);
		long double getGlobalTime();

		private:
		State* const parent;
		Stateful* const owner;
		uint32_t time;
		uint32_t time_offset;
	};

}