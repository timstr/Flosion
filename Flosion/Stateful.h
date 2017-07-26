#pragma once

namespace musical {

	struct Stateful {
		virtual ~Stateful();

		virtual bool findSource(Stateful* src) = 0;
	};

	struct State {
		State(State* _parent, Stateful* _owner);
		virtual ~State();

		virtual void reset() = 0;

		State* getParentState() const;
		Stateful* getOwner() const;

		private:
		State* const parent;
		Stateful* const owner;
	};

}