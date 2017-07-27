#pragma once

#include "Stateful.h"
#include "SoundSource.h"

namespace musical {

	// to allow SoundProcessor classes to share their state data
	template <typename StateType>
	struct StateNumberSource : NumberSource {
		StateNumberSource(SoundSource* _owner) : NumberSource(_owner){
			owner = _owner;
		}

		double evaluate(State* state, int chunk_pos) override {
			while (state){
				if (state->getOwner() == owner){
#ifdef _DEBUG
					if (StateType* s = dynamic_cast<StateType*>(state)){
						return getValue(s, chunk_pos);
					} else {
						throw std::runtime_error("The found state is not compatible");
					}
#else
					return getValue((StateType*)state, chunk_pos);
#endif
				}
				state = state->getParentState();
			}
			throw std::runtime_error("State belonging to owner sound processing object was not found in state chain");
		}

		virtual double getValue(StateType* state, int chunk_pos) = 0;

		private:
		SoundSource* owner;
	};

	// to allow SoundProcessor classes to share the stateful data of their MultiInputs
	template <class StateType, class SoundInputType = SoundInput>
	struct MultiInputNumberSource : NumberSource {
		MultiInputNumberSource(SoundInputType* _owner) : NumberSource(_owner), parentmultiinput(_owner) {

		}

		double evaluate(State* state, int chunk_pos) override {
			while (state){
				if (state->getOwner() == parentmultiinput){
#ifdef _DEBUG
					if (StateType* s = dynamic_cast<StateType*>(state)){
						return getValue(s, chunk_pos);
					} else {
						throw std::exception("State found belonging to MultiInput is incorrect type");
					}
#else
					return getValue((StateType*)state, chunk_pos);
#endif
				}
				state = state->getParentState();
			}

			throw std::exception("A state belonging to the MultiInput was not found in the state chain");
		}

		virtual double getValue(StateType* state, int chunk_pos) = 0;

		SoundInputType* const parentmultiinput;
	};

}