#pragma once

#include "musical.h"

namespace musical {

	struct WaveGenState : State {
		using State::State;
		void reset() noexcept override {
			phase = 0;
		}
		float phase;
	};

	struct WaveGen : SoundSourceTemplate<WaveGenState> {
		WaveGen() : phase(this), wavefunction(this), frequency(this) {

		}

		void renderChunk(SoundChunk& chunk, WaveGenState& state) override {
			for (int i = 0; i < CHUNK_SIZE; i++){
				chunk[i].l = wavefunction.getValue(&state);
				chunk[i].r = chunk[i].l;

				state.phase += frequency.getValue(&state, 250) / (float)SFREQ;
				state.phase -= floor(state.phase);
				state.advanceTime(1);
			}
		}

		struct WaveGenPhase : StateNumberSource<> {
			using StateNumberSource::StateNumberSource;
			float getValue(const WaveGenState& state, const State* context) const noexcept override {
				return state.phase;
			}
		} phase;
		NumberInput wavefunction;
		NumberInput frequency;
	};
}