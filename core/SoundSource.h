#pragma once

#include "musical.h"
#include "SoundInput.h"
#include "Stateful.h"
#include <vector>
#include <unordered_map>
#include <set>
#include <cassert>

namespace musical {

	struct SoundSource : Stateful {
		~SoundSource() noexcept;

		virtual void getNextChunk(SoundChunk& chunk, const State* dependant_state, const Stateful* dst) = 0;

	protected:

		friend struct SoundInput;

		std::vector<SoundInput*> destinations;
	};

	// SoundSource template for specializing internal state type

	template <class StateType>
	struct SoundSourceTemplate : SoundSource {
		SoundSourceTemplate() noexcept {
			static_assert(std::is_base_of<State, StateType>::value, "The provided StateType must derive from State");
		}

		// overrides base function, delegates rendering to renderChunk()
		// and passes corresponding local state
		//-------------------------------------------------------------------
		void getNextChunk(SoundChunk& chunk, const State* parent_state, const Stateful* dst) override {
			StateType& state = lookupState(parent_state, dst);
			const auto time = state.getTime();
			renderChunk(chunk, state);
			state.setTotalTimeTo(time + CHUNK_SIZE);
		}

		// to be overridden in user-defined classes to fill buffer with next
		// chunk of audio data and modify state appropriately
		// all statefull information should be kept in state to guarantee thread safety
		//-------------------------------------------------------------------
		virtual void renderChunk(SoundChunk& chunk, StateType& state) = 0;

		// add a new state to the state map and tell all inputs to add states
		// corresponding to the new state
		void addState(const State* parent_state, const Stateful* dependant) noexcept override {
			auto [it, was_inserted] = state_map.emplace(
				std::piecewise_construct,
				std::forward_as_tuple(parent_state, dependant),
				std::forward_as_tuple(parent_state, dependant)
			);
			assert(was_inserted);
			it->second.performReset();
			for (const auto& d : dependencies){
				d->addState(&it->second, this);
			}
		}

		// remove the state corresponding to the given state from the state map and
		// tell all inputs to do the same
		void removeState(const State* parent_state, const Stateful* dependant) noexcept override {
			auto it = state_map.find(std::make_pair(parent_state, dependant));
			assert(it != state_map.end());
			for (const auto& d : dependencies){
				d->removeState(&it->second, this);
			}
			state_map.erase(it);
		}

		// reset the state in the state map corresponding to the given state and tell
		// all inputs to do the same
		void resetState(const State* parent_state, const Stateful* dependant) noexcept override {
			auto it = state_map.find(std::make_pair(parent_state, dependant));
			assert(it != state_map.end());
			it->second.performReset();
			for (const auto& d : dependencies){
				d->resetState(&it->second, this);
			}
		}

		void addAllStatesTo(Stateful* dependency, const Stateful* via) const noexcept override {
			for (const auto& it : state_map){
				dependency->addState(&it.second, via);
			}
		}

		void removeAllStatesFrom(Stateful* dependency, const Stateful* via) const noexcept override {
			for (const auto& it : state_map){
				dependency->removeState(&it.second, via);
			}
		}

		std::size_t numStates() const noexcept override {
			return state_map.size();
		}

		// NumberSource for querying state variables
		template <typename SoundSourceType = SoundSource>
		struct StateNumberSource : NumberSource {
			StateNumberSource(SoundSourceType* _parentsoundsource)
				: NumberSource(_parentsoundsource), parentsoundsource(_parentsoundsource) {

			}

			float evaluate(const State* state) const noexcept override {
				const State* context = state;
				while (state) {
					if (state->getOwner() == parentsoundsource) {
#ifndef NDEBUG
						const StateType* s = dynamic_cast<const StateType*>(state);
						assert(s);
						return getValue(*s, context);
#else
						return getValue(static_cast<const StateType&>(*state), context);
#endif
					}
					state = state->getParentState();
				}
				assert(false);
				return 0.0f;
			}

			virtual float getValue(const StateType& state, const State* context) const noexcept = 0;

		protected:
			SoundSourceType* const parentsoundsource;
		};

	private:

		// return the state corresponding to the parent state,
		// or throw exception if it doesn't exist
		StateType& lookupState(const State* parent_state, const Stateful* dependant) {
			auto it = state_map.find(std::make_pair(parent_state, dependant));
			if (it == state_map.end()) {
				throw std::runtime_error("The parent state could not be found in the state map");
			} else {
				return it->second;
			}
		}

		struct Hash {
			std::size_t operator()(const std::pair<const State*, const Stateful*>& x) const noexcept {
				return std::hash<const State*>()(x.first) * 31 + std::hash<const Stateful*>()(x.second);
			}
		};

		std::unordered_map<std::pair<const State*, const Stateful*>, StateType, Hash> state_map;
	};

}