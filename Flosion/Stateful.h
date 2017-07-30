#pragma once

#include <cstdint>

namespace musical {

	struct Stateful {
		virtual ~Stateful();

		virtual bool findSource(Stateful* src) const = 0;
	};

	struct State {
		State(State* _parent, Stateful* _owner);
		virtual ~State();

		virtual void reset() = 0;

		State* getParentState() const;
		Stateful* getOwner() const;

		void tickTime();
		void skipTime(uint32_t t);
		long double getTime() const;

		private:
		State* const parent;
		Stateful* const owner;
		uint32_t time;
	};

}