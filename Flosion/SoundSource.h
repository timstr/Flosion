#pragma once

#include "musical.h"
#include "Sample.h"
#include "Stateful.h"
#include <vector>
#include <unordered_map>
#include <set>

namespace musical {

	struct SoundSource;

	// SoundInput base

	struct SoundInput : Stateful {
		SoundInput(SoundSource* _parent);
		virtual ~SoundInput();

		virtual void addState(State* parent_state) = 0;
		virtual void removeState(State* parent_state) = 0;
		virtual void resetState(State* parent_state) = 0;


		SoundSource* SoundInput::getSource() const;
		virtual void setSource(SoundSource* _source) = 0;
		bool hasSource() const;
		bool findSource(Stateful* src) const override;

		protected:
		SoundSource* source;
		SoundSource* parent;
	};


	// SingleInput

	struct SingleInput : SoundInput {
		SingleInput(SoundSource* parent);
		~SingleInput();

		// fills the provided buffer with audio data from the input, or silence if no
		// sound source is connected
		// buffer is assumed to be of length musical::CHUNK_SIZE
		// state is to be the state provided to the current renderChunk function
		//-------------------------------------------------------------------
		void getNextChunk(Sample* buffer, State* state);

		//if an input is connected, tell it to add a new state for the given parent state
		void addState(State* parent_state) override;

		//if an input is connected, tell it to remove the state corresponding to the given parent state
		void removeState(State* parent_state) override;

		//if an input is connected, tell it to reset the state corresponding to the given parent state
		void resetState(State* parent_state) override;

		void setSource(SoundSource* source) override;
	};

	// MultiInputState

	template <class KeyType>
	struct MultiInputState : State {
		MultiInputState(State* _parent, Stateful* _owner, const KeyType& _key) : State(_parent, _owner), key(_key) {

		}

		const KeyType key;
	};

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
			StateType* state = lookupState(parent_state, dst);
			renderChunk(buffer, state);
			state->commitTime();
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

		// NumberSource for querying state variables
		struct StateNumberSource : NumberSource {
			StateNumberSource(SoundSource* _owner) : NumberSource(_owner){
				owner = _owner;
			}

			float evaluate(State* state) const override {
				while (state){
					if (state->getOwner() == owner){
#ifdef _DEBUG
						if (StateType* s = dynamic_cast<StateType*>(state)){
							return getValue(s, chunk_pos);
						} else {
							throw std::runtime_error("The found state is not compatible");
						}
#else
						return getValue((StateType*)state);
#endif
					}
					state = state->getParentState();
				}
				throw std::runtime_error("State belonging to owner sound processing object was not found in state chain");
			}

			virtual float getValue(StateType* state) const = 0;

			private:
			SoundSource* owner;
		};

		// MultiInput struct
		template <class InputStateType, class KeyType, class SoundSourceType = SoundSourceTemplate<StateType>>
		struct MultiInput : SoundInput {
			MultiInput(SoundSourceType* _parent) : SoundInput(_parent), parentsoundsource(_parent) {
				static_assert(std::is_base_of<MultiInputState<KeyType>, InputStateType>::value, "The InputStateType must derive from MultiInputState<KeyType>");
				static_assert(std::is_base_of<SoundSourceTemplate<StateType>, SoundSourceType>::value, "The SoundSourceType must derive from SoundSourceTemplate<StateType>");
			}
			~MultiInput(){
				setSource(nullptr);
				while (keys.size() > 0){
					removeKey(*keys.begin());
				}
			}

			struct iterator {
				iterator(typename const MultiInput<InputStateType, KeyType, SoundSourceType>& _multi_input, typename const std::set<KeyType>::iterator& _it, State* _parent_state)
					: it(_it), multi_input(_multi_input), parent_state(_parent_state) {

				}

				iterator& operator++(int dummy){
					it++;
					return *this;
				}
				iterator& operator--(int dummy){
					it--;
					return *this;
				}
				bool operator==(const iterator& _it) const {
					return (this->it == _it.it);
				}
				bool operator!=(const iterator& _it) const {
					return !(this->it == _it.it);
				}

				const KeyType& key() const {
					if (it == multi_input.keys.end()){
						throw std::runtime_error("The iterator is invalid");
					}
					return *it;
				}
				InputStateType& state() const {
					if (it == multi_input.keys.end()){
						throw std::runtime_error("The iterator is invalid");
					}
					auto it2 = multi_input.state_map.find(std::make_pair(*it, parent_state));
					if (it2 == multi_input.state_map.end()){
						throw std::runtime_error("The provided state could not be found in the multi-input's state map");
					}
					return *(it2->second);
				}

				private:
				typename std::set<KeyType>::iterator it;
				const MultiInput<InputStateType, KeyType, SoundSourceType>& multi_input;
				State* parent_state;
			};

			iterator begin(State* parent_state){
				return iterator(*this, keys.begin(), parent_state);
			}

			iterator end(State* parent_state){
				return iterator(*this, keys.end(), parent_state);
			}

			// fills the provided buffer with audio data from the input, as mapped to
			// the provided key
			// buffer is silenced if no source is connected or if the key is not registered
			// buffer is assumed to be of length musical::CHUNK_SIZE
			// state is to be the state provided to the current renderChunk function
			//-------------------------------------------------------------------
			void getNextChunk(Sample* buffer, State* state, KeyType key){
				if (source){
					auto it = state_map.find({key, state});
					if (it == state_map.end()){
						throw std::runtime_error("The provided key and state combination could not be found");
					} else {
						source->getNextChunk(buffer, it->second, this);
					}
				} else {
					for (int i = 0; i < CHUNK_SIZE; i++){
						buffer[i] = Sample(0, 0);
					}
				}
				state->skipTime(CHUNK_SIZE);
				state->commitTime();
			}

			void getNextChunk(Sample* buffer, const iterator& it){
				for (int i = 0; i < CHUNK_SIZE; i++){
					buffer[i] = Sample(0, 0);
				}
				if (source){
					source->getNextChunk(buffer, &it.state(), this);
				}
				it.state().skipTime(CHUNK_SIZE);
				it.state().commitTime();
			}

			void setSource(SoundSource* _source) override {
				if (source){
					source->removeDstInput(this);
					for (auto it = state_map.begin(); it != state_map.end(); it++){
						source->removeState(it->second, this);
					}
				}

				source = _source;

				if (source){
					for (auto it = state_map.begin(); it != state_map.end(); it++){
						source->addState(it->second, this);
					}
					source->addDstInput(this);
				}
			}

			// adds a key to an input state
			//-------------------------------------------------------------------
			void addKey(KeyType key){
				if (keys.find(key) != keys.end()){
					throw std::runtime_error("The provided key is already registered.");
				}
				for (auto it = states.begin(); it != states.end(); it++){
					InputStateType* st = new InputStateType(*it, this, key);
					st->reset();
					state_map[std::make_pair(key, *it)] = st;
					if (source){
						source->addState(st, this);
					}
				}
				keys.insert(key);
			}

			// removes a key to an input state
			//-------------------------------------------------------------------
			void removeKey(KeyType key){
				for (auto it = states.begin(); it != states.end(); it++){
					auto it2 = state_map.find(std::make_pair(key, *it));
					if (it2 == state_map.end()){
						throw std::runtime_error("The provided key could not be found in the state map");
					}
					if (source){
						source->removeState(*it, this);
					}
					delete it2->second;
					state_map.erase(it2);
				}
				auto it3 = keys.find(key);
				if (it3 == keys.end()){
					throw std::runtime_error("The provided key could not be found in the key set");
				}
				keys.erase(it3);
			}

			// allocate a local state corresponding to the parent state and every key,
			// and tell the input to create new states for each if an input is connected
			void addState(State* parent_state) override {
				if (states.find(parent_state) != states.end()){
					throw std::runtime_error("The provided state is already registered");
				}
				for (auto it = keys.begin(); it != keys.end(); it++){
					InputStateType* st = new InputStateType(parent_state, this, *it);
					st->reset();
					state_map[std::make_pair(*it, parent_state)] = st;
					if (source){
						source->addState(st, this);
					}
				}
				states.insert(parent_state);
			}

			// tell the input, if it is connected, to remove those states corresponding to
			// the given parent state and every registered key
			void removeState(State* parent_state) override {
				for (auto it = keys.begin(); it != keys.end(); it++){
					auto it2 = state_map.find(std::make_pair(*it, parent_state));
					if (it2 == state_map.end()){
						throw  std::runtime_error("The provided state could not be found in the state map");
					}
					if (source){
						source->removeState(it2->second, this);
					}
					delete it2->second;
					state_map.erase(it2);
				}
				auto it3 = states.find(parent_state);
				if (it3 == states.end()){
					throw std::runtime_error("The provided state could not be found in the state set");
				}
				states.erase(it3);
			}

			// reset all states corresponding to the given parent state and all keys,
			// tell input, if it is connected, to reset all the same states
			void resetState(State* parent_state) override {
				for (auto it = keys.begin(); it != keys.end(); it++){
					auto it2 = state_map.find(std::make_pair(*it, parent_state));
					if (it2 == state_map.end()){
						throw std::runtime_error("The provided state could not be found in the state map");
					}
					if (source){
						source->resetState(it2->second, this);
					}
				}
			}

			void resetState(State* parent_state, KeyType key){
				auto it = state_map.find(std::make_pair(key, parent_state));
				if (it == state_map.end()){
					throw std::runtime_error("The provided key and state combination could not be found in the state map");
				}
				if (source){
					source->resetState(it->second);
				}
			}

			void resetState(const iterator& it){
				if (source){
					source->resetState(&it.state(), this);
				}
			}

			InputStateType& getState(State* parent_state, KeyType key){
				auto it = state_map.find(std::make_pair(key, parent_state));
				if (it == state_map.end()){
					throw std::runtime_error("The state could not be found");
				}
				return *it;
			}

			SoundSourceType* const parentsoundsource;

			// NumberSource for querying state variables
			struct StateNumberSource : NumberSource {
				StateNumberSource(MultiInput<InputStateType, KeyType, SoundSourceType>* _owner) : NumberSource(_owner), parentmultiinput(_owner) {

				}

				float evaluate(State* state) const override {
					while (state){
						if (state->getOwner() == parentmultiinput){
#ifdef _DEBUG
							if (InputStateType* s = dynamic_cast<InputStateType*>(state)){
								return getValue(s, chunk_pos);
							} else {
								throw std::exception("State found belonging to MultiInput is incorrect type");
							}
#else
							return getValue((InputStateType*)state);
#endif
						}
						state = state->getParentState();
					}

					throw std::exception("A state belonging to the MultiInput was not found in the state chain");
				}

				virtual float getValue(InputStateType* state) const = 0;

				MultiInput<InputStateType, KeyType, SoundSourceType>* const parentmultiinput;
			};

			private:

			struct Hash {
				std::size_t operator()(const std::pair<KeyType, State*>& x) const {
					return std::hash<KeyType>()(x.first) * 31 + std::hash<State*>()(x.second);
				}
			};

			std::unordered_map<std::pair<KeyType, State*>, InputStateType*, Hash> state_map;

			std::set<KeyType> keys;
			std::set<State*> states;
		};
		// end MultiInput

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