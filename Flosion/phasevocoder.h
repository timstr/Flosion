#pragma once

#include "SoundSource.h"
#include "NumberResult.h"
#include "fft.h"
#include "hannwindow.h"

namespace musical {

	// TODO: separate left and right channels for stereo

	struct PhaseVocoderState : State {
		using State::State;

		void reset() override {
			offset = 0.0f;
			phase = 0;
			for (int i = 0; i < CHUNK_SIZE; i++){
				inbuffer1[i] = Sample(0, 0);
				inbuffer2[i] = Sample(0, 0);
				prev[i] = 0.0f;
				next[i]= 0.0f;
				prev_phase_diffs[i] = 0.0f;
				next_phase_diffs[i] = 0.0f;
				prev_phase_acc[i] = 0.0f;
				next_phase_acc[i] = 0.0f;
				outbuffer[i] = 0.0f;
				carryover[i] = Sample(0, 0);
			}
		}

		// for analysis
		int phase;
		Sample inbuffer1[CHUNK_SIZE];	// for input audio data
		Sample inbuffer2[CHUNK_SIZE];	// for input audio data
		complex prev[CHUNK_SIZE];	// phase/amp of last window
		complex next[CHUNK_SIZE];	// phase/amp of next window


		// for re-synthesis
		float prev_phase_diffs[CHUNK_SIZE];
		float next_phase_diffs[CHUNK_SIZE];
		float prev_phase_acc[CHUNK_SIZE];
		float next_phase_acc[CHUNK_SIZE];
		complex outbuffer[CHUNK_SIZE];
		Sample carryover[CHUNK_SIZE];

		float offset;
	};

	struct PhaseVocoder : SoundSourceTemplate<PhaseVocoderState> {
		PhaseVocoder() : input(this), timespeed(this) {

		}

		void renderChunk(Sample* buffer, PhaseVocoderState* state){
			float speed = timespeed.getValue(state, 1.0f);

			// previous 1, 2, and 3
			for (int i = 0; i < CHUNK_SIZE; i++){
				buffer[i] = state->carryover[i];
			}

			// 0
			getNextFrame(speed, state);
			for (int i = 0; i < CHUNK_SIZE; i++){
				float val = state->outbuffer[i].real() * getHannWindow(i);
				buffer[i].l += val;
				buffer[i].r += val;
				state->carryover[i] = Sample(0, 0);
			}
			
			// 1
			getNextFrame(speed, state);
			for (int i = 0; i < CHUNK_SIZE * 3 / 4; i++){
				float val = state->outbuffer[i].real() * getHannWindow(i);
				buffer[i + CHUNK_SIZE / 4].l += val;
				buffer[i + CHUNK_SIZE / 4].r += val;
			}
			for (int i = 0; i < CHUNK_SIZE / 4; i++){
				float val = state->outbuffer[i + CHUNK_SIZE * 3 / 4].real() * getHannWindow(i + CHUNK_SIZE * 3 / 4);
				state->carryover[i].l += val;
				state->carryover[i].r += val;
			}

			// 2
			getNextFrame(speed, state);
			for (int i = 0; i < CHUNK_SIZE / 2; i++){
				float val = state->outbuffer[i].real() * getHannWindow(i);
				buffer[i + CHUNK_SIZE / 2].l += val;
				buffer[i + CHUNK_SIZE / 2].r += val;
			}
			for (int i = 0; i < CHUNK_SIZE / 2; i++){
				float val = state->outbuffer[i + CHUNK_SIZE / 2].real() * getHannWindow(i + CHUNK_SIZE / 2);
				state->carryover[i].l += val;
				state->carryover[i].r += val;
			}

			// 3
			getNextFrame(speed, state);
			for (int i = 0; i < CHUNK_SIZE / 4; i++){
				float val = state->outbuffer[i].real() * getHannWindow(i);
				buffer[i + CHUNK_SIZE * 3 / 4].l += val;
				buffer[i + CHUNK_SIZE * 3 / 4].r += val;
			}
			for (int i = 0; i < CHUNK_SIZE * 3 / 4; i++){
				float val = state->outbuffer[i + CHUNK_SIZE / 4].real() * getHannWindow(i + CHUNK_SIZE / 4);
				state->carryover[i].l += val;
				state->carryover[i].r += val;
			}
		}

		SingleInput input;
		NumberResult timespeed;

		private:

		void swap(PhaseVocoderState* state){
			for (int i = 0; i < CHUNK_SIZE; i++){
				state->prev[i] = state->next[i];
				state->prev_phase_acc[i] = state->next_phase_acc[i];
				state->prev_phase_diffs[i] = state->next_phase_diffs[i];
			}

			if (state->phase == 0){
				for (int i = 0; i < CHUNK_SIZE; i++){
					state->next[i] = state->inbuffer1[i].l * getHannWindow(i);
				}

				state->phase = 1;
			} else if (state->phase == 1){
				for (int i = 0; i < CHUNK_SIZE * 3 / 4; i++){
					state->next[i] = state->inbuffer1[i + CHUNK_SIZE / 4].l * getHannWindow(i);
				}
				input.getNextChunk(state->inbuffer2, state);
				for (int i = 0; i < CHUNK_SIZE / 4; i++){
					state->next[i + CHUNK_SIZE * 3 / 4] = state->inbuffer2[i].l * getHannWindow(i + CHUNK_SIZE * 3 / 4);
				}

				state->phase = 2;
			} else if (state->phase == 2){
				for (int i = 0; i < CHUNK_SIZE / 2; i++){
					state->next[i] = state->inbuffer1[i + CHUNK_SIZE / 2].l * getHannWindow(i);
				}
				for (int i = 0; i < CHUNK_SIZE / 2; i++){
					state->next[i + CHUNK_SIZE / 2] = state->inbuffer2[i].l * getHannWindow(i + CHUNK_SIZE / 2);
				}

				state->phase = 3;
			} else if (state->phase == 3){
				for (int i = 0; i < CHUNK_SIZE / 4; i++){
					state->next[i] = state->inbuffer1[i + CHUNK_SIZE * 3 / 4].l * getHannWindow(i);
				}
				for (int i = 0; i < CHUNK_SIZE * 3 / 4; i++){
					state->next[i + CHUNK_SIZE / 4] = state->inbuffer2[i].l * getHannWindow(i + CHUNK_SIZE / 4);
				}

				state->phase = 4;
			} else if (state->phase == 4){
				for (int i = 0; i < CHUNK_SIZE; i++){
					state->next[i] = state->inbuffer2[i].l * getHannWindow(i);
				}

				state->phase = 5;
			} else if (state->phase == 5){
				for (int i = 0; i < CHUNK_SIZE * 3 / 4; i++){
					state->next[i] = state->inbuffer2[i + CHUNK_SIZE / 4].l * getHannWindow(i);
				}
				input.getNextChunk(state->inbuffer1, state);
				for (int i = 0; i < CHUNK_SIZE / 4; i++){
					state->next[i + CHUNK_SIZE * 3 / 4] = state->inbuffer1[i].l * getHannWindow(i + CHUNK_SIZE * 3 / 4);
				}

				state->phase = 6;
			} else if (state->phase == 6){
				for (int i = 0; i < CHUNK_SIZE / 2; i++){
					state->next[i] = state->inbuffer2[i + CHUNK_SIZE / 2].l * getHannWindow(i);
				}
				for (int i = 0; i < CHUNK_SIZE / 2; i++){
					state->next[i + CHUNK_SIZE / 2] = state->inbuffer1[i].l * getHannWindow(i + CHUNK_SIZE / 2);
				}

				state->phase = 7;
			} else if (state->phase == 7){
				for (int i = 0; i < CHUNK_SIZE / 4; i++){
					state->next[i] = state->inbuffer2[i + CHUNK_SIZE * 3 / 4].l * getHannWindow(i);
				}
				for (int i = 0; i < CHUNK_SIZE * 3 / 4; i++){
					state->next[i + CHUNK_SIZE / 4] = state->inbuffer1[i].l * getHannWindow(i + CHUNK_SIZE / 4);
				}

				state->phase = 0;
			}






			fft(state->next, CHUNK_SIZE);
			// cyclic shift
			for (int i = 1; i < CHUNK_SIZE; i += 2){
				state->next[i] *= -1.0f;
			}
			for (int i = 0; i < CHUNK_SIZE; i++){
				state->next_phase_diffs[i] = std::arg(state->next[i]) - std::arg(state->prev[i]);
			}
		}

		// write spectrum to outbuffer
		void getNextFrame(float speed, PhaseVocoderState* state){
			while (state->offset > 1.0f){
				swap(state);
				state->offset -= 1.0f;
			}

			for (int i = 0; i < CHUNK_SIZE; i++){

				state->prev_phase_acc[i] = fmod(state->prev_phase_acc[i] + state->prev_phase_diffs[i], 2.0f * 3.141592654f);
				state->next_phase_acc[i] = fmod(state->next_phase_acc[i] + state->next_phase_diffs[i], 2.0f * 3.141592654f);

				complex prev = std::abs(state->prev[i]) * complex(cos(state->prev_phase_acc[i]), sin(state->prev_phase_acc[i]));
				complex next = std::abs(state->next[i]) * complex(cos(state->next_phase_acc[i]), sin(state->next_phase_acc[i]));

				state->outbuffer[i] = prev * (1.0f - state->offset) + next * state->offset;
			}

			// cyclic shift
			for (int i = 1; i < CHUNK_SIZE; i += 2){
				state->outbuffer[i] *= -1.0f;
			}
			ifft(state->outbuffer, CHUNK_SIZE);

			state->offset += speed;
		}
	};

}