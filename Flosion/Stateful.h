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

		void performReset();

		State* getParentState() const;
		Stateful* getOwner() const;

		void tick();
		void skipTime(uint32_t t);
		void commitTime();
		float getTime() const; // TODO: return integer sample count?
		float getTimeAt(Stateful* stateful);
		float getGlobalTime();

		protected:
		virtual void reset() = 0;

		private:
		State* const parent;
		Stateful* const owner;
		uint32_t time;
		uint32_t time_offset;
	};

}