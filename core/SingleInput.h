#pragma once

#include "SoundInput.h"
#include <unordered_set>

namespace flo {

	// TODO: rename to something with "Sound", same for MultiInput

	// SingleInput is a sound input that has one input state for every own state
	// It does not own any state of its own, and instead forwards
	// the states of its owners directly
	struct SingleInput : SoundInput {
		SingleInput(Stateful* parent) noexcept;
		~SingleInput() noexcept;

		// fills the provided buffer with audio data from the input, or silence if no
		// sound source is connected
		// state is to be the state provided to the current renderChunk function
		void getNextChunk(SoundChunk& chunk, const State* state) noexcept;

		void addState(const State* parent_state, const Stateful* dependant) noexcept override;

		void removeState(const State* parent_state, const Stateful* dependant) noexcept override;

		void resetState(const State* parent_state, const Stateful* dependant) noexcept override;

		void addAllStatesTo(Stateful* dependency) const noexcept override;

		void removeAllStatesFrom(Stateful* dependency) const noexcept override;

		std::size_t numStates() const noexcept override;

	private:

		std::unordered_set<const State*> m_statemap;
	};

} // namespace flo