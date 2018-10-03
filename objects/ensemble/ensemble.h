#pragma once

#include <random>
#include "Sample.h"
#include "SoundSource.h"
#include "MultiInput.h"

namespace flo {

	struct EnsembleState : State {
		using State::State;
		void reset() noexcept override {
			for (int i = 0; i < CHUNK_SIZE; i++){
				buffer[i] = {0, 0};
			}
		}
		SoundChunk buffer;
	};

	struct Ensemble : SoundSourceBase<EnsembleState> {
		Ensemble() : 
			SoundSourceBase<EnsembleState>(false),
			input(this),
			num_voices(this),
			frequency(this),
			frequency_spread(this) {

			for (int i = 0; i < max_voices; i++){
				input.addKey(i);
			}
		}

		void renderChunk(SoundChunk& chunk, EnsembleState& state) override {
			int nvoices = std::min(std::max(1, (int)num_voices.getValue(&state, 0)), max_voices);
			for (int i = 0; i < nvoices; i++){
				input.getNextChunk(state.buffer, &state, i);
				for (int s = 0; s < CHUNK_SIZE; s++){
					chunk[s] += state.buffer[s] / (float)nvoices;
				}
			}
		}

		NumberInput num_voices;
		NumberInput frequency;
		NumberInput frequency_spread;

		struct InputState : MultiInputState<int> {
			using MultiInputState::MultiInputState;
			void reset() noexcept override {
				static std::uniform_real_distribution<float> gen(0.0, 1.0);
				static std::default_random_engine eng;
				rand_value = gen(eng);
			}
			float rand_value;
			private:
		};

		struct Input : MultiInput<InputState, int, Ensemble> {
			Input(Ensemble* _parent) : MultiInput(_parent), frequency(this) {

			}

			struct Frequency : StateNumberSource {
				using StateNumberSource::StateNumberSource;
				float getValue(const InputState& state, const State* context) const noexcept override {
					float freq = parentmultiinput->parentsoundsource->frequency.getValue(&state);
					float spread = parentmultiinput->parentsoundsource->frequency_spread.getValue(&state);
					return freq * (1.0f + spread * state.rand_value);
				}
			} frequency;

		} input;

		static const int max_voices = 32;
	};

}