#pragma once

#include "SoundInput.h"
#include <cassert>

namespace musical {

	template <class KeyType>
	struct MultiInputState : State {
		MultiInputState(const State* _parent, const Stateful* _owner, const KeyType& _key) noexcept :
			State(_parent, _owner),
			key(_key) {

		}

		const KeyType key;
	};

	template <class InputStateType, class KeyType, class SoundSourceType = SoundSource>
	struct MultiInput : SoundInput {
		MultiInput(SoundSourceType* _parent) noexcept :
			SoundInput(_parent),
			parentsoundsource(_parent) {

			static_assert(std::is_base_of<MultiInputState<KeyType>, InputStateType>::value, "The InputStateType must derive from MultiInputState<KeyType>");
			static_assert(std::is_base_of<SoundSource, SoundSourceType>::value, "The SoundSourceType must derive from SoundSource");
		}
		~MultiInput() {
			setSource(nullptr);
			while (keys.size() > 0) {
				removeKey(*keys.begin());
			}
		}

		struct iterator {
			using MultiInputType = MultiInput<InputStateType, KeyType, SoundSourceType>;
			using KeyIterator = typename std::set<KeyType>::iterator;

			iterator(MultiInputType& _multi_input, KeyIterator _key_it, const State* _parent_state) noexcept :
				key_it(_key_it),
				multi_input(_multi_input),
				parent_state(_parent_state) {

			}

			iterator& operator++(int dummy) noexcept {
				key_it++;
				return *this;
			}
			iterator& operator--(int dummy) noexcept {
				key_it--;
				return *this;
			}
			bool operator==(const iterator& _it) const noexcept {
				return (this->key_it == _it.key_it) && (this->parent_state == _it.parent_state);
			}
			bool operator!=(const iterator& _it) const noexcept {
				return !(this->key_it == _it.key_it) || !(this->parent_state == _it.parent_state);
			}

			const KeyType& key() const noexcept {
				assert(key_it != multi_input.keys.end());
				return *key_it;
			}
			InputStateType& state() const noexcept {
				assert(key_it != multi_input.keys.end());
				auto it = multi_input.state_map.find(std::make_pair(*key_it, parent_state));
				assert(it != multi_input.state_map.end());
				return it->second;
			}

		private:
			KeyIterator key_it;
			MultiInputType& multi_input;
			const State* parent_state;

			template <class InputStateType, class KeyType, class SoundSourceType>
			friend struct MultiInput;
		};

		iterator begin(const State* parent_state) noexcept {
			return iterator(*this, keys.begin(), parent_state);
		}

		iterator end(const State* parent_state) noexcept {
			return iterator(*this, keys.end(), parent_state);
		}

		// fills the provided chunk with audio data from the input, as mapped to
		// the provided key
		// chunk is silenced if no source is connected or if the key is not registered
		// chunk is assumed to be of length musical::CHUNK_SIZE
		// state is to be the state provided to the current renderChunk function
		void getNextChunk(SoundChunk& chunk, State* state, KeyType key) noexcept {
			if (m_source) {
				auto it = state_map.find({ key, state });
				assert(it != state_map.end());
				try {
					m_source->getNextChunk(chunk, &it->second, this);
					return;
				} catch (...) { }
			}
			chunk.silence();
		}

		void getNextChunk(SoundChunk& chunk, const iterator& it) noexcept {
			if (m_source){
				try {
					m_source->getNextChunk(chunk, &it.state(), this);
					return;
				} catch (...){ }
			}
			chunk.silence();
		}

		// add a key to an input state
		void addKey(KeyType key) noexcept {
			assert(keys.find(key) == keys.end());
			for (const auto& state : states){
				auto [it, was_inserted] = state_map.emplace(
					std::piecewise_construct,
					std::forward_as_tuple(key, state),
					std::forward_as_tuple(state, this, key)
				);

				assert(was_inserted);
				it->second.performReset();
				if (m_source) {
					m_source->addState(&it->second, this);
				}
			}
			keys.insert(key);
		}

		// remove a key to an input state
		void removeKey(KeyType key) noexcept {
			for (auto it = states.begin(); it != states.end(); it++) {
				auto it2 = state_map.find(std::make_pair(key, *it));
				assert(it2 != state_map.end());
				if (m_source) {
					m_source->removeState(&it2->second, this);
				}
				state_map.erase(it2);
			}
			auto it3 = keys.find(key);
			assert(it3 != keys.end());
			keys.erase(it3);
		}

		void addState(const State* parent_state, const Stateful* dependant) noexcept override {
			assert(dependant == parentsoundsource);
			assert(states.find(parent_state) == states.end());
			for (const auto& key : keys) {
				auto [it, was_inserted] = state_map.emplace(
					std::piecewise_construct,
					std::forward_as_tuple(key, parent_state),
					std::forward_as_tuple(parent_state, this, key)
				);
				
				assert(was_inserted);
				it->second.performReset();
				if (m_source) {
					m_source->addState(&it->second, this);
				}
			}
			states.insert(parent_state);
		}

		void removeState(const State* parent_state, const Stateful* dependant) noexcept override {
			assert(dependant == parentsoundsource);
			for (const auto& key : keys) {
				auto it = state_map.find(std::make_pair(key, parent_state));
				assert(it != state_map.end());
				if (m_source) {
					m_source->removeState(&it->second, this);
				}
				state_map.erase(it);
			}
			auto it3 = states.find(parent_state);
			assert(it3 != states.end());
			states.erase(it3);
		}

		void resetState(const State* parent_state, const Stateful* dependant) noexcept override {
			assert(dependant == parentsoundsource);
			for (const auto& key : keys){
				auto it = state_map.find(std::make_pair(key, parent_state));
				assert(it != state_map.end());
				it->second.performReset();
				if (m_source) {
					m_source->resetState(&it->second, this);
				}
			}
		}

		void resetState(State* parent_state, KeyType key) noexcept {
			auto it = state_map.find(std::make_pair(key, parent_state));
			assert(it != state_map.end());
			it->second.performReset();
			if (m_source) {
				m_source->resetState(it->second);
			}
		}

		void resetState(const iterator& it) noexcept {
			assert(it.key_it != keys.end());
			it.state().performReset();
			if (m_source) {
				m_source->resetState(&it.state(), this);
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

		InputStateType& getState(State* parent_state, KeyType key) noexcept {
			auto it = state_map.find(std::make_pair(key, parent_state));
			assert(it != state_map.end());
			return *it;
		}

		SoundSourceType* const parentsoundsource;

		// NumberSource for querying state variables
		struct StateNumberSource : NumberSource {
			StateNumberSource(MultiInput<InputStateType, KeyType, SoundSourceType>* _owner) noexcept :
				NumberSource(_owner),
				parentmultiinput(_owner) {

			}

			float evaluate(const State* state) const noexcept override {
				const State* context = state;
				while (state) {
					if (state->getOwner() == parentmultiinput) {
#ifndef NDEBUG
						const InputStateType* s = dynamic_cast<const InputStateType*>(state);
						assert(s);
						return getValue(*s, context);
#else
						return getValue(*(const InputStateType*)state, context);
#endif
					}
					state = state->getParentState();
				}

				assert(false);
				return 0.0f;
			}

			virtual float getValue(const InputStateType& state, const State* context) const noexcept = 0;

			MultiInput<InputStateType, KeyType, SoundSourceType>* const parentmultiinput;
		};

	private:

		struct Hash {
			std::size_t operator()(const std::pair<KeyType, const State*>& x) const noexcept {
				return std::hash<KeyType>()(x.first) * 31 + std::hash<const State*>()(x.second);
			}
		};

		std::unordered_map<std::pair<KeyType, const State*>, InputStateType, Hash> state_map;

		std::set<KeyType> keys;
		std::set<const State*> states;
	};

} // namespace musical