#pragma once

#ifndef MUSICAL_CHUNK_SIZE
#define MUSICAL_CHUNK_SIZE 1024
#endif

#ifndef MUSICAL_SAMPLE_FREQ
#define MUSICAL_SAMPLE_FREQ 44100
#endif

#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <iterator>
#include <functional>

namespace musical {

	//----General Thoughts----------------------------------------------
	// States exist for these things:
	// - sound processors (eg. sampler progress)
	// - multi-inputs of sound processors (eg. note frequency, note progress)
	// - the output (playback object)
	//
	// Variable calculation is now to be done within a state environment which is passed
	// simply as a pointer to the current state.
	//------------------------------------------------------------------

	// TODO: add safety-checking for cycles in Number/Sound processing streams
	// TODO: find some better way to handle time-sensitive values than by passing 'chunk_pos' around
	// TODO: make thread pool system for parellelized audio processing

	struct Sample;

	const int CHUNK_SIZE = MUSICAL_CHUNK_SIZE;
	const int SFREQ = MUSICAL_SAMPLE_FREQ;

	struct State;
	struct Stateful;

	struct SoundInput;
	struct SingleInput;
	template <class K, class T, class P>
	struct MultiInput;

	struct SoundSource;
	template <class T>
	struct SoundSourceTemplate;

	struct NumberSource;
	template <typename ST>
	struct StateNumber;
	struct NumberInput;
	struct NumberResult;



	///////////////////////////

	struct Sample {
		double l, r;

		Sample(){
			l = 0;
			r = 0;
		}

		Sample(double _l, double _r){
			l = std::min(std::max(_l, -1.0), 1.0);
			r = std::min(std::max(_r, -1.0), 1.0);
		}

		Sample operator+(Sample _sample){
			return(Sample(l + _sample.l, r + _sample.r));
		}
		Sample operator+=(Sample _sample){
			*this = Sample(l + _sample.l, r + _sample.r);
			return(*this);
		}
		Sample operator-(Sample _sample){
			return(Sample(l - _sample.l, r - _sample.r));
		}
		Sample operator-=(Sample _sample){
			*this = Sample(l - _sample.l, r - _sample.r);
			return(*this);
		}
		Sample operator*(double _val){
			return(Sample(l * _val, r * _val));
		}
		Sample operator*=(double _val){
			*this = Sample(l * _val, r * _val);
			return(*this);
		}
		Sample operator/(double _val){
			return(Sample(l / _val, r / _val));
		}
		Sample operator/=(double _val){
			*this = Sample(l / _val, r / _val);
			return(*this);
		}
		Sample operator-(){
			return(Sample(-l, -r));
		}
	};
	Sample operator*(double _val, Sample _sample){
		return Sample(_sample.l * _val, _sample.r * _val);
	}

	// Base class to both SoundInput and SoundSource
	// Derived classes serve as owners of stateful information as stored
	// in a corresponding State-deriving class
	struct Stateful {
		virtual ~Stateful(){

		}

		protected:
		virtual bool findSource(Stateful* src) = 0;

		friend class NumberInput;
	};

	// Base class to sound source states and multi-input states
	// parent is either null or another state
	//-----------------------------------------------------------------------
	// a state pointer is to be passed along with fvar calculations to represent
	// the current context by the current state and all its parent states. When a
	// function in a sound source or multi-input is called, the state that belongs
	// to the sound source or multi-input must be found in the state chain and
	// referred to to return a value
	struct State {
		State(State* _parent, Stateful* _owner){
			parent = _parent;
			owner = _owner;
		}
		virtual ~State(){

		}
		virtual void reset() = 0;
		private:
		State* parent;
		Stateful* owner;

		template <typename ST>
		friend class StateNumber;
		template <class ST, class S>
		friend class MultiInputNumber;
	};

	// A sound input that has a null parent is assumed to be a sound input
	// that is not used for any musical sound processing and serves as a single
	// endpoint, as in within the SoundOutput class
	struct SoundInput : Stateful {
		SoundInput(SoundSource* _parent);
		virtual ~SoundInput();

		virtual void addState(State* parent_state) = 0;
		virtual void removeState(State* parent_state) = 0;
		virtual void resetState(State* parent_state) = 0;


		SoundSource* SoundInput::getSource(){
			return source;
		}
		virtual void setSource(SoundSource* _source) = 0;

		bool hasSource(){
			return source;
		}

		bool findSource(Stateful* src) override;

		protected:
		SoundSource* source;
		SoundSource* parent;
	};

	// Interfaces a sound source and is used to receive an input sound stream
	// chunk by chunk
	// To be used when the input is only called for once per chunk
	//-----------------------------------------------------------------------
	// TODO:
	// - pointer to sound source
	// - strict association to parent sound source for state relationships
	// - call upon source to create/remove states corresponding to states of parent
	//   sound source when a source is attached/removed respectively
	// - query all states from parent sound source when attaching input
	//   to a new sound source so new states may be created
	struct SingleInput : SoundInput {
		SingleInput(SoundSource* parent) : SoundInput(parent) {

		}
		~SingleInput(){
			setSource(nullptr);
		}

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

	template <class KeyType>
	struct MultiInputState : State {
		MultiInputState(State* _parent, Stateful* _owner, const KeyType& _key) : State(_parent, _owner), key(_key) {
			
		}

		const KeyType key;
	};

	// Interfaces a sound source and is used to receive an input sound stream
	// chunk by chunk
	// To be used when the input is called multiple times per chunk to
	// manage separate input states
	// Different input states are mapped out via a provided key type
	//-----------------------------------------------------------------------
	// TODO:
	// - keep internal store of StateType states for every key and parent state pair
	//   i.e. a map of type <State*, KeyType> -> StateType
	// - look up StateType state with given parent (sound source) state and key
	// - strict association to parent sound source for state parenthood relationships
	//   and allocation/deallocation
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

	// base processing class
	//-----------------------------------------------------------------------
	// sound source must call upon single- and multi- inputs to create new states
	// when a new state is created for the sound source itself
	struct SoundSource : Stateful {
		~SoundSource(){
			for (int i = 0; i < inputs.size(); i++){
				inputs[i]->setSource(nullptr);
			}
			inputs.clear();

			// TODO: fix this up
			// Note: When this destructor is called, the destructor for SoundSourceTemplate has already been called
			// and this object is effectively only of type SoundSource
			// thus the calls from SoundInput::setSource to SoundSource::removeState are UB?
			// maybe?
			// perhaps do this work in SoundSourceTemplate

		}

		// fills buffer with next chunk of audio data
		//-------------------------------------------------------------------
		virtual void getNextChunk(Sample* buffer, State* parent_state, SoundInput* dst) = 0;

		protected:

		void addDstInput(SoundInput* input){
			for (int i = 0; i < dsts.size(); i++){
				if (dsts[i] == input){
					throw std::runtime_error("The destination SoundInput has already been added");
				}
			}
			dsts.push_back(input);
		}

		void removeDstInput(SoundInput* input){
			for (int i = 0; i < dsts.size(); i++){
				if (dsts[i] == input){
					dsts.erase(dsts.begin() + i);
					return;
				}
			}
			throw std::runtime_error("The destination SoundInput could not be found");
		}

		virtual void addState(State* parent_state, SoundInput* dst) = 0;
		virtual void removeState(State* parent_state, SoundInput* dst) = 0;
		virtual void resetState(State* parent_state, SoundInput* dst) = 0;

		virtual void addAllStatesTo(SoundInput* input) = 0;

		virtual void removeAllStatesFrom(SoundInput* input) = 0;

		bool findSource(Stateful* src) override {
			if (src == this){
				return true;
			}

			for (SoundInput* input : inputs){
				if (input->findSource(src)){
					return true;
				}
			}

			return false;
		}

		private:

		void addSoundInput(SoundInput* input){
			for (int i = 0; i < inputs.size(); i++){
				if (inputs[i] == input){
					throw std::runtime_error("The SoundInput is already registered");
				}
			}

			inputs.push_back(input);
		}

		void removeSoundInput(SoundInput* input){
			for (int i = 0; i < inputs.size(); i++){
				if (inputs[i] == input){
					inputs.erase(inputs.begin() + i);
					return;
				}
			}
			throw std::runtime_error("The SoundInput could not be found");
		}

		std::vector<SoundInput*> inputs;
		std::vector<SoundInput*> dsts;

		friend struct SoundInput;
		friend struct SingleInput;

		template<typename K, typename S, typename P>
		friend struct MultiInput;

		template <class T>
		friend struct SoundSourceTemplate;
	};

	bool SoundInput::findSource(Stateful* src) {
		if (this == src){
			return true;
		}
		if (source){
			return source->findSource(src);
		}
		return false;
	}

	// templated base class from which all user-defined sound processing classes
	// are to derive with their own state type
	//-----------------------------------------------------------------------
	template <class StateType>
	struct SoundSourceTemplate : SoundSource {
		SoundSourceTemplate(){
			static_assert(std::is_base_of<State, StateType>::value, "The provided StateType must derive from State");
		}
		~SoundSourceTemplate(){
			while (dsts.size() > 0){
				if (dsts.front()->getSource() != this){
					throw std::runtime_error("A destination SoundInput is apparently connected to a different SoundSource");
				}
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

		std::map<std::pair<State*, SoundInput*>, StateType*> state_map;
	};

	struct SoundResult {
		SoundResult() : input(nullptr) {
			
		}
		~SoundResult(){

		}

		void getNextChunk(Sample* buffer){
			input.getNextChunk(buffer, nullptr);
		}

		void reset(){
			input.resetState(nullptr);
		}

		void setSource(SoundSource* source){
			input.setSource(source);
		}

		SingleInput input;
	};

	// Pesky forward-declarations

	SoundInput::SoundInput(SoundSource* _parent){
		source = nullptr;
		if (_parent){
			_parent->addSoundInput(this);
		}
		parent = _parent;
	}
	SoundInput::~SoundInput(){
		if (parent){
			parent->removeSoundInput(this);
		}
	}
	void SingleInput::setSource(SoundSource* _source){
		if (source){
			source->removeDstInput(this);
			if (parent){
				parent->removeAllStatesFrom(this);
			} else {
				source->removeState(nullptr, this);
			}
		}

		source = _source;

		if (source){
			if (parent){
				parent->addAllStatesTo(this);
			} else {
				source->addState(nullptr, this);
			}
			source->addDstInput(this);
		}
	}

	void SingleInput::addState(State* parent_state){
		if (source){
			source->addState(parent_state, this);
		}
	}
	void SingleInput::removeState(State* parent_state){
		if (source){
			source->removeState(parent_state, this);
		}
	}
	void SingleInput::resetState(State* parent_state){
		if (source){
			source->resetState(parent_state, this);
		}
	}
	void SingleInput::getNextChunk(Sample* buffer, State* state){
		for (int i = 0; i < CHUNK_SIZE; i++){
			buffer[i] = {0, 0};
		}
		if (source){
			source->getNextChunk(buffer, state, this);
		}
	}



	//-------------- Numerical Doings Below This Line --------------//

	// NumberSource represents the source of a numerical value
	// Extended classes of NumberSource may be constant variables or functions of
	// other NumberSources using the NumberInput class as interfaces to these
	// -------------
	// parent		-	the owner of stateful information of this NumberSource
	//					This will be either a SoundSource or SoundMultiInput
	//					If the numbersource is not inherently stateful, such as in a global
	//					variable or pure math function, the parent shall be null
	struct NumberSource {
		NumberSource(Stateful* _parent){
			parent = _parent;
		}
		virtual ~NumberSource();
		virtual double evaluate(State* state, int chunk_pos) = 0;

		private:

		void addDstInput(NumberInput* dst){
			for (int i = 0; i < dsts.size(); i++){
				if (dsts[i] == dst){
					throw std::runtime_error("The destination NumberInput is already registered");
				}
			}
			dsts.push_back(dst);
		}
		void removeDstInput(NumberInput* dst){
			for (int i = 0; i < dsts.size(); i++){
				if (dsts[i] == dst){
					dsts.erase(dsts.begin() + i);
					return;
				}
			}
			throw std::runtime_error("The destination NumberInput could not be found");
		}

		void addInput(NumberInput* input){
			for (int i = 0; i < inputs.size(); i++){
				if (inputs[i] == input){
					throw std::runtime_error("The input is already registered");
				}
			}
			inputs.push_back(input);
		}
		void removeInput(NumberInput* input){
			for (int i = 0; i < inputs.size(); i++){
				if (inputs[i] == input){
					inputs.erase(inputs.begin() + i);
					return;
				}
			}
			throw std::runtime_error("The input could not be found");
		}

		void findAllStatefulSources(std::vector<Stateful*>& sources);

		void findAllStatefulDests(std::vector<Stateful*>& dests);

		bool findStatelessDest();

		Stateful* parent;
		std::vector<NumberInput*> dsts;
		std::vector<NumberInput*> inputs;

		friend class NumberInput;
	};

	// to allow SoundProcessor classes to share their state data

	template <typename StateType>
	struct StateNumber : NumberSource {
		StateNumber(SoundSource* _owner) : NumberSource(_owner){
			owner = _owner;
		}

		double evaluate(State* state, int chunk_pos) override {
			while (state){
				if (state->owner == owner){
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
				state = state->parent;
			}
			throw std::runtime_error("State belonging to owner sound processing object was not found in state chain");
		}

		virtual double getValue(StateType* state, int chunk_pos) = 0;

		private:
		SoundSource* owner;
	};

	// to allow SoundProcessor classes to share the stateful data of their MultiInputs
	template <class StateType, class SoundInputType = SoundInput>
	struct MultiInputNumber : NumberSource {
		MultiInputNumber(SoundInputType* _owner) : NumberSource(_owner), parentmultiinput(_owner) {

		}

		double evaluate(State* state, int chunk_pos) override {
			while (state){
				if (state->owner == parentmultiinput){
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
				state = state->parent;
			}

			throw std::exception("A state belonging to the MultiInput was not found in the state chain");
		}

		virtual double getValue(StateType* state, int chunk_pos) = 0;

		SoundInputType* const parentmultiinput;
	};

	//for use inside NumberSource classes
	struct NumberInput {
		NumberInput(NumberSource* _parent, Stateful* _owner = nullptr){
			if (_parent && _owner){
				throw std::runtime_error("Time to rethink what you're doing.");
			}
			parent = _parent;
			if (parent){
				parent->addInput(this);
			}
			owner = _owner;
			source = nullptr;
		}
		~NumberInput(){
			if (parent){
				parent->removeInput(this);
			}
			if (source){
				source->removeDstInput(this);
			}
		}

		bool isSafeSource(NumberSource* src){
			if (src == nullptr){
				return true;
			}

			std::vector<Stateful*> stateful_sources;
			src->findAllStatefulSources(stateful_sources);
			if (stateful_sources.size() == 0){
				return true;
			} else {
				std::vector<Stateful*> stateful_dests;
				findAllStatefulDests(stateful_dests);
				if (stateful_dests.size() == 0){
					return !findStatelessDest();
				} else {
					for (Stateful* src : stateful_sources){
						for (Stateful* dst: stateful_dests){
							if (!src->findSource(dst)){
								return false;
							}
						}
					}
					return true;
				}
			}
		}

		void setSource(NumberSource* _source){
			if (!isSafeSource(_source)){
				throw std::runtime_error("The numbersource is not safe to connect; it violates the integrity of stateful information");
			}

			if (source){
				source->removeDstInput(this);
			}
			source = _source;
			if (source){
				source->addDstInput(this);
			}
		}

		void findAllStatefulSources(std::vector<Stateful*>& sources){
			if (source){
				source->findAllStatefulSources(sources);
			}
		}

		void findAllStatefulDests(std::vector<Stateful*>& dests){
			if (owner){
				dests.push_back(owner);
			}
			if (parent){
				parent->findAllStatefulDests(dests);
			}
		}

		bool findStatelessDest(){
			if (parent){
				return parent->findStatelessDest();
			} else if (owner){
				return false;
			} else {
				return true;
			}
		}

		// state shall be the current contextual state chain
		// any allowable values shall come from global constants which are not stateful
		// or from sound processing objects, which shall be found as parents of some state
		// in the state chain (accessed through the line of parents in the given state)
		// this state is to be passed on as-is to preserve its full context
		double getValue(State* state, unsigned int chunk_pos, double default_value = 0){
			if (source){
				return source->evaluate(state, chunk_pos);
			} else {
				return default_value;
			}
		}

		private:
		NumberSource* parent;
		Stateful* owner;
		NumberSource* source;
	};

	NumberSource::~NumberSource(){
		for (int i = 0; i < dsts.size(); i++){
			dsts[i]->setSource(nullptr);
		}
		for (int i = 0; i < inputs.size(); i++){
			inputs[i]->setSource(nullptr);
		}
	}

	void NumberSource::findAllStatefulSources(std::vector<Stateful*>& sources){
		if (parent){
			sources.push_back(parent);
		}
		for (int i = 0; i < inputs.size(); i++){
			inputs[i]->findAllStatefulSources(sources);
		}
	}

	void NumberSource::findAllStatefulDests(std::vector<Stateful*>& dests){
		for (int i = 0; i < dsts.size(); i++){
			dsts[i]->findAllStatefulDests(dests);
		}
	}

	bool NumberSource::findStatelessDest(){
		for (int i = 0; i < dsts.size(); i++){
			if (dsts[i]->findStatelessDest()){
				return true;
			}
		}
		return false;
	}

	// for use in SoundProcessors and for display of stateless calculation results
	// owner shall point to either the stateful object that uses the results of this Number object
	// in its own stateful calculations, or owner shall be null, in which case this Number object
	// is stateless (and may only have global sources)
	struct NumberResult {
		NumberResult(Stateful* owner) : input(nullptr, owner) {

		}

		double getValue(State* state, int chunk_pos, double default_value = 0.0){
			return input.getValue(state, chunk_pos, default_value);
		}

		bool isSafeSource(NumberSource* source){
			return input.isSafeSource(source);
		}

		void setSource(NumberSource* source){
			input.setSource(source);
		}

		NumberInput input;
	};

	struct PureFunction : NumberSource {
		PureFunction() : NumberSource(nullptr){

		}
	};

	template <class StateType>
	struct StatefulFunction : PureFunction {
		StatefulFunction(){
			
		}

		double evaluate(State* state, int chunk_pos) override {
			auto it = state_map.find(state);
			if (it == state_map.end()){
				return getValue(state, chunk_pos, state_map[state] = StateType());
			}
			return getValue(state, chunk_pos, state_map[state]);
		}

		virtual double getValue(State* state, int chunk_pos, StateType& own_state) = 0;

		private:
		std::map<State*, StateType> state_map;
	};

	struct Constant : PureFunction {
		Constant(double _value = 0.0){
			value = _value;
		}

		void setValue(double val){
			value = val;
		}
		double getValue(){
			return value;
		}

		double evaluate(State* state, int chunk_pos) override {
			return value;
		}

		private:
		double value;
	};

	//--------------
}