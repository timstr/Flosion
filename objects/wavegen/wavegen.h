#pragma once

#include "musical.h"

namespace musical {

	struct WaveGenState : State {
		using State::State;
		void reset() override {
			phase = 0;
		}
		float phase;
	};

	struct WaveGen : SoundSourceTemplate<WaveGenState> {
		WaveGen() : phase(this, 0, 1), wavefunction(this, -1, 1), frequency(this) {

		}

		void renderChunk(Buffer& buffer, WaveGenState* state) override {
			for (int i = 0; i < CHUNK_SIZE; i++){
				buffer[i].l = wavefunction.getValue(state);
				buffer[i].r = buffer[i].l;

				state->phase += frequency.getValue(state, 250) / (float)SFREQ;
				state->phase -= floor(state->phase);
				state->tick();
			}
		}

		struct WaveGenPhase : StateNumberSource<> {
			using StateNumberSource::StateNumberSource;
			float getValue(WaveGenState* state, State* context) const override {
				return state->phase;
			}
		} phase;
		NumberResult wavefunction;
		NumberResult frequency;
	};
}