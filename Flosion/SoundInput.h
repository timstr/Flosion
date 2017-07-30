#pragma once

#include "musical.h"
#include "Stateful.h"
#include "SoundSource.h"
#include "Sample.h"
#include <map>
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

	// MultiInput

	// TODO: what on earth is ParentSoundSourceType??
	template <class StateType, class KeyType, class ParentSoundSourceType = SoundSource>
	struct MultiInput : SoundInput {
		MultiInput(ParentSoundSourceType* _parent) : SoundInput(_parent), parentsoundsource(_parent) {
			static_assert(std::is_base_of<MultiInputState<KeyType>, StateType>::value, "The StateType must derive from MultiInputState<KeyType>");
		}
		~MultiInput(){
			setSource(nullptr);
			while (keys.size() > 0){
				removeKey(*keys.begin());
			}
		}

		struct iterator {
			iterator(const MultiInput<StateType, KeyType, ParentSoundSourceType>& _multi_input, typename const std::set<KeyType>::iterator& _it, State* _parent_state)
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
			StateType& state() const {
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
			const MultiInput<StateType, KeyType, ParentSoundSourceType>& multi_input;
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
		}

		void getNextChunk(Sample* buffer, const iterator& it){
			for (int i = 0; i < CHUNK_SIZE; i++){
				buffer[i] = Sample(0, 0);
			}
			if (source){
				source->getNextChunk(buffer, &it.state(), this);
			}
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
				StateType* st = new StateType(*it, this, key);
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
				StateType* st = new StateType(parent_state, this, *it);
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

		StateType& getState(State* parent_state, KeyType key){
			auto it = state_map.find(std::make_pair(key, parent_state));
			if (it == state_map.end()){
				throw std::runtime_error("The state could not be found");
			}
			return *it;
		}

		ParentSoundSourceType* const parentsoundsource;

		private:

		std::map<std::pair<KeyType, State*>, StateType*> state_map;

		std::set<KeyType> keys;
		std::set<State*> states;
	};

}