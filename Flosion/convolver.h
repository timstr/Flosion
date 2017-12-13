#pragma once

#include "SoundSource.h"
#include "NumberResult.h"
#include "SoundResult.h"
#include "SoundQueue.h"
#include <algorithm>

namespace musical {

	// TODO: why is this so terribly slow?

	const unsigned int max_convolution_length = SFREQ;

	struct ConvolverState : State {
		ConvolverState(State* parent, Stateful* owner)
			: State(parent, owner), queue(max_convolution_length + CHUNK_SIZE) {

		}

		void reset() override {
			position = 0;
			length = max_convolution_length;
			queue.clear();
			for (int i = 0; i < max_convolution_length; i++){
				conv[i] = 0.0f;
			}
		}

		SoundQueue queue;
		float conv[max_convolution_length];
		unsigned int position;
		unsigned int length;
	};

	struct Convolver : SoundSourceTemplate<ConvolverState> {
		Convolver() : function(this, -1, 1), function_length(this, 0, 1), input(this) {

		}

		void renderChunk(Sample* buffer, ConvolverState* state) override {
			state->length = std::min(std::max(0u, (unsigned)(function_length.getValue(state) * SFREQ)), max_convolution_length);
			for (unsigned int i = 0; i < state->length; i++){
				state->position = i;
				state->conv[i] = function.getValue(state);
			}

			state->queue.advance(CHUNK_SIZE, input, state);
			for (size_t i = 0; i < CHUNK_SIZE; i++){
				Sample s = Sample(0, 0);
				for (size_t j = 0; j < state->length; j++){
					s += state->queue[i + j] * state->conv[j];
				}
				buffer[i] = s / (float)SFREQ;
			}
		}


		NumberResult function;
		NumberResult function_length;
		struct Input : SingleInput {
			Input(Convolver* parent) : SingleInput(parent), position(parent) {

			}

			struct Position : StateNumberSource<Convolver> {
				using StateNumberSource::StateNumberSource;

				float getValue(ConvolverState* state, State* context) const {
					return state->position / (float)state->length;
				}
			} position;
		} input;
	};
}