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
		SoundResult() noexcept;

		void getNextChunk(SoundChunk& chunk) noexcept;

		void reset() noexcept;

		void setSource(SoundSource* source) noexcept;

		std::size_t getTime() const noexcept;

		SingleInput input;

	private:

		void addState(const State* parent_state, const Stateful* dependant) noexcept override;

		void removeState(const State* parent_state, const Stateful* dependant) noexcept override;

		void resetState(const State* parent_state, const Stateful* dependant) noexcept override;

		void addAllStatesTo(Stateful* dependency) const noexcept override;

		void removeAllStatesFrom(Stateful* dependency) const noexcept override;

		std::size_t numStates() const noexcept override;

		struct RootState : State {
			using State::State;
			void reset() noexcept override {

			}
		} state;
	};

}