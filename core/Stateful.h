#pragma once

#include <cstdint>
#include <vector>
#include "musical.h"

namespace musical {

	struct Stateful;
	struct State;

	// Stateful owns state information that is distributed among multiple State objects
	struct Stateful {
		virtual ~Stateful();

		// returns the current time speed
		// this will be 1 in most cases, but some stateful objects may slow or speed up time
		virtual double getTimeSpeed(const State* state_chain) const noexcept;

		// returns the total elapsed time (in samples)
		uint32_t getTime(const State* state_chain) const noexcept;


		// returns true if this depends on s
		bool hasDependency(const Stateful* s) const noexcept;

		// returns true if this immediately depends on s
		bool hasImmediateDependency(const Stateful* s) const noexcept;

		// returns true if s depends on this
		bool hasDependant(const Stateful* s) const noexcept;

		// returns true if s depends immediately on this
		bool hasImmediateDependant(const Stateful* s) const noexcept;


		// register a stateful object on which this depends
		void addDependency(Stateful* dependency) noexcept;

		// unregister a stateful object on which this depends
		void removeDependency(Stateful* dependency) noexcept;


		// add a new state mapped to by the immediate dependant and one of its states
		virtual void addState(const State* parent_state, const Stateful* dependant) noexcept = 0;

		// remove a state mapped to by the immediate dependant and one of its states
		virtual void removeState(const State* parent_state, const Stateful* dependant) noexcept = 0;

		// reset a state mapped to by the immediate dependant and one of its states
		virtual void resetState(const State* parent_state, const Stateful* dependant) noexcept = 0;

		// add all own states to an immediate dependency, for example when swapping sources
		// 'via' is a stateful object acting as the dependant, which usually will be 'this'
		virtual void addAllStatesTo(Stateful* dependency, const Stateful* via) const noexcept = 0;

		// remove all own states from an immediate dependency, for example when swapping sources
		virtual void removeAllStatesFrom(Stateful* dependency, const Stateful* via) const noexcept = 0;

		// total number of own states
		virtual std::size_t numStates() const noexcept = 0;

		std::vector<Stateful*> dependencies;
		std::vector<Stateful*> dependants;
	};

	// State holds all stateful information belonging to a Stateful object
	struct State {
		State(const State* _parent, const Stateful* _owner);
		virtual ~State();

		void performReset() noexcept;

		const State* getParentState() const noexcept;
		const Stateful* getOwner() const noexcept;

		// the difference between the tentative time and saved time is used along with
		// the getTimeSpeed() function of Stateful objects to give better estimates of
		// time relative to other Stateful objects when time speed varies going up the
		// state chain

		// move the tentative time forward
		void advanceTime(uint32_t samples) noexcept;

		// set the tentative time since last saving
		void setTimeSinceSave(uint32_t samples) noexcept;

		// save the tentative time to the total time
		void saveTime() noexcept;

		// go back to the last saved time
		void revertToSavedTime() noexcept;

		// set the saved time and tentative time to 'samples'
		void setTotalTimeTo(uint32_t samples) noexcept;

		// returns the object's current time, in samples, as seen by itself
		uint32_t getTime() const noexcept;

		// returns the object's current time, in samples, as seen by another Stateful object
		uint32_t getTimeAt(const Stateful* stateful) const noexcept;

		// returns the object's current time, in samples, relative to the root state
		uint32_t getGlobalTime() const noexcept;

	protected:
		// must be overridden to reset the state to its initial condition
		virtual void reset() noexcept = 0;

	private:

		// the state which this is a child of
		const State* const parent;

		// the stateful object which has this state
		const Stateful* const owner;

		// coarse elapsed time, in samples
		uint32_t total_time;

		// fine elapsed time, in samples
		uint32_t time_offset;
	};

}