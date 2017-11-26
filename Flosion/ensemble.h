#pragma once

#include <random>
#include "musical.h"

namespace musical {

	struct EnsembleState : State {
		using State::State;
		void reset() override {
			for (int i = 0; i < CHUNK_SIZE; i++){
				buffer[i] = {0, 0};
			}
		}
		Sample buffer[CHUNK_SIZE];
	};

	struct Ensemble : SoundSourceTemplate<EnsembleState> {
		Ensemble() : input(this), num_voices(this, 1, max_voices), frequency(this), frequency_spread(this) {
			for (int i = 0; i < max_voices; i++){
				input.addKey(i);
			}
		}

		void renderChunk(Sample* buffer, EnsembleState* state) override {
			int nvoices = std::min(std::max(1, (int)num_voices.getValue(state, 0)), max_voices);
			for (int i = 0; i < nvoices; i++){
				input.getNextChunk(state->buffer, state, i);
				for (int s = 0; s < CHUNK_SIZE; s++){
					buffer[s] += state->buffer[s] / (double)nvoices;
				}
			}
		}

		NumberResult num_voices;
		NumberResult frequency;
		NumberResult frequency_spread;

		struct InputState : MultiInputState<int> {
			using MultiInputState::MultiInputState;
			void reset() override {
				static std::uniform_real_distribution<double> gen(0.0, 1.0);
				static std::default_random_engine eng;
				rand_value = gen(eng);
			}
			double rand_value;
			private:
		};

		struct Input : MultiInput<InputState, int, Ensemble> {
			Input(Ensemble* _parent) : MultiInput(_parent), frequency(this) {

			}

			struct Frequency : StateNumberSource {
				using StateNumberSource::StateNumberSource;
				float getValue(InputState* state, State* context) const override {
					double freq = parentmultiinput->parentsoundsource->frequency.getValue(state);
					double spread = parentmultiinput->parentsoundsource->frequency_spread.getValue(state);
					return freq * (1.0 + spread * state->rand_value);
				}
			} frequency;

		} input;

		static const int max_voices = 32;
	};

}