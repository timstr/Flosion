#pragma once

#include "SoundSource.h"
#include "SingleInput.h"

namespace flo {

	// TODO: add a mutex that objects down the state chain acquire
	// before making changes to wiring

	// SoundResult is the global entry point into a chain of stateful
	// SoundSources. It is not intended for use inside a SoundSource,
	// but as the end point in sound processing
	struct SoundResult : Stateful {
		SoundResult() NOEXCEPT_IF_I_SAY_SO;

		void getNextChunk(SoundChunk& chunk) NOEXCEPT_IF_I_SAY_SO;

		void reset() NOEXCEPT_IF_I_SAY_SO;

		void setSource(SoundSource* source) NOEXCEPT_IF_I_SAY_SO;

		std::size_t getTime() const NOEXCEPT_IF_I_SAY_SO;

		SingleInput input;

	private:

		void addState(const State* parent_state, const Stateful* dependant) NOEXCEPT_IF_I_SAY_SO override;

		void removeState(const State* parent_state, const Stateful* dependant) NOEXCEPT_IF_I_SAY_SO override;

		void resetState(const State* parent_state, const Stateful* dependant) NOEXCEPT_IF_I_SAY_SO override;

		void addAllStatesTo(Stateful* dependency) const NOEXCEPT_IF_I_SAY_SO override;

		void removeAllStatesFrom(Stateful* dependency) const NOEXCEPT_IF_I_SAY_SO override;

		std::size_t numStates() const NOEXCEPT_IF_I_SAY_SO override;

		struct RootState : State {
			using State::State;
			void reset() NOEXCEPT_IF_I_SAY_SO override {

			}
		} state;
	};

}