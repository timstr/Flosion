#pragma once

#include "musical.h"
#include "StateNumberSource.h"

namespace musical {

	struct WaveGenState : State {
		using State::State;
		void reset() override {
			phase = 0;
		}
		float phase;
	};

	struct WaveGen : SoundSourceTemplate<WaveGenState> {
		WaveGen() : phase(this), wavefunction(this), frequency(this) {

		}

		void renderChunk(Sample* buffer, WaveGenState* state) override {
			for (int i = 0; i < CHUNK_SIZE; i++){
				buffer[i].l = wavefunction.getValue(state, i);
				buffer[i].r = buffer[i].l;

				state->phase += frequency.getValue(state, 250) / (double)SFREQ;
				state->phase -= floor(state->phase);
				state->tick();
			}
		}

		struct WaveGenPhase : StateNumberSource<WaveGenState> {
			using StateNumberSource::StateNumberSource;
			float getValue(WaveGenState* state) const override {
				return state->phase;
			}
		} phase;
		NumberResult wavefunction;
		NumberResult frequency;
	};
}