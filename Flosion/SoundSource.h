#pragma once

#include "musical.h"
#include "Sample.h"
#include "Stateful.h"
#include "SoundInput.h"
#include <vector>
#include <unordered_map>

namespace musical {

	struct SoundInput;

	// SoundSource base

	struct SoundSource : Stateful {
		~SoundSource();

		virtual void getNextChunk(Sample* buffer, State* parent_state, SoundInput* dst) = 0;

		void addDstInput(SoundInput* input);

		void removeDstInput(SoundInput* input);

		virtual void addState(State* parent_state, SoundInput* dst) = 0;
		virtual void removeState(State* parent_state, SoundInput* dst) = 0;
		virtual void resetState(State* parent_state, SoundInput* dst) = 0;

		virtual void addAllStatesTo(SoundInput* input) = 0;

		virtual void removeAllStatesFrom(SoundInput* input) = 0;

		bool findSource(Stateful* src) const override;

		void addSoundInput(SoundInput* input);

		void removeSoundInput(SoundInput* input);

		protected:

		std::vector<SoundInput*> inputs;
		std::vector<SoundInput*> dsts;
	};

	// SoundSource template for specializing internal state type

	template <class StateType>
	struct SoundSourceTemplate : SoundSource {
		SoundSourceTemplate(){
			static_assert(std::is_base_of<State, StateType>::value, "The provided StateType must derive from State");
		}
		~SoundSourceTemplate(){
			while (dsts.size() > 0){
				dsts.front()->setSource(nullptr);
			}
		}

		// overrides base function, delegates rendering to renderChunk()
		// and passes corresponding local state
		//-------------------------------------------------------------------
		void getNextChunk(Sample* buffer, State* parent_state, SoundInput* dst) override {
			renderChunk(buffer, lookupState(parent_state, dst));
		}

		// to be overridden in user-defined classes to fill buffer with next
		// chunk of audio data and modify state appropriately
		// all statefull information should be kept in state to guarantee thread safety
		//-------------------------------------------------------------------
		virtual void renderChunk(Sample* buffer, StateType* state) = 0;

		protected:

		void resetAllStates(){
			//TODO
			// also reset all states of single/multi inputs and sources?
		}

		void addAllStatesTo(SoundInput* input) override {
			for (auto it = state_map.begin(); it != state_map.end(); it++){
				input->addState(it->second);
			}
		}

		void removeAllStatesFrom(SoundInput* input) override {
			for (auto it = state_map.begin(); it != state_map.end(); it++){
				input->removeState(it->second);
			}
		}

		// add a new state to the state map and tell all inputs to add states
		// corresponding to the new state
		void addState(State* parent_state, SoundInput* dst) override {
			if (state_map.find(std::make_pair(parent_state, dst)) != state_map.end()){
				throw std::runtime_error("The provided state is already registered");
			}
			StateType* st = new StateType(parent_state, this);
			st->reset();
			state_map[std::make_pair(parent_state, dst)] = st;
			for (int i = 0; i < inputs.size(); i++){
				inputs[i]->addState(st);
			}
		}

		// remove the state corresponding to the given state from the state map and
		// tell all inputs to do the same
		void removeState(State* parent_state, SoundInput* dst) override {
			auto it = state_map.find(std::make_pair(parent_state, dst));
			if (it == state_map.end()){
				throw std::runtime_error("The parent state could not be found in the state map");
			} else {
				for (int i = 0; i < inputs.size(); i++){
					inputs[i]->removeState(it->second);
				}
				delete it->second;
				state_map.erase(it);
			}
		}

		// reset the state in the state map corresponding to the given state and tell
		// all inputs to do the same
		void resetState(State* parent_state, SoundInput* dst) override {
			auto it = state_map.find(std::make_pair(parent_state, dst));
			if (it == state_map.end()){
				throw std::runtime_error("The parent state could not be found in the state map");
			} else {
				it->second->reset();
				for (int i = 0; i < inputs.size(); i++){
					inputs[i]->resetState(it->second);
				}
			}
		}

		private:

		// return the state corresponding to the parent state,
		// or throw exception if it doesn't exist
		StateType* lookupState(State* parent_state, SoundInput* dst){
			auto it = state_map.find(std::make_pair(parent_state, dst));
			if (it == state_map.end()){
				throw std::runtime_error("The parent state could not be found in the state map");
			} else {
				return it->second;
			}
		}

		struct Hash {
			std::size_t operator()(const std::pair<State*, SoundInput*>& x) const {
				return std::hash<State*>()(x.first) * 31 + std::hash<SoundInput*>()(x.second);
			}
		};

		std::unordered_map<std::pair<State*, SoundInput*>, StateType*, Hash> state_map;
	};

}