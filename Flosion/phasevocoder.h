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
				prev_l[i] = 0.0f;
				prev_r[i] = 0.0f;
				next_l[i]= 0.0f;
				next_r[i]= 0.0f;
				prev_phase_diffs_l[i] = 0.0f;
				prev_phase_diffs_r[i] = 0.0f;
				next_phase_diffs_l[i] = 0.0f;
				next_phase_diffs_r[i] = 0.0f;
				prev_phase_acc_l[i] = 0.0f;
				prev_phase_acc_r[i] = 0.0f;
				next_phase_acc_l[i] = 0.0f;
				next_phase_acc_r[i] = 0.0f;
				outbuffer_l[i] = 0.0f;
				outbuffer_r[i] = 0.0f;
				carryover[i] = Sample(0, 0);
			}
		}

		// for analysis
		int phase;
		Sample inbuffer1[CHUNK_SIZE];	// for input audio data
		Sample inbuffer2[CHUNK_SIZE];	// for input audio data
		complex prev_l[CHUNK_SIZE];	// phase/amp of last window, left channel
		complex prev_r[CHUNK_SIZE];	// phase/amp of last window, right channel
		complex next_l[CHUNK_SIZE];	// phase/amp of next window, left channel
		complex next_r[CHUNK_SIZE];	// phase/amp of next window, right channel


		// for re-synthesis
		float prev_phase_diffs_l[CHUNK_SIZE];
		float prev_phase_diffs_r[CHUNK_SIZE];
		float next_phase_diffs_l[CHUNK_SIZE];
		float next_phase_diffs_r[CHUNK_SIZE];
		float prev_phase_acc_l[CHUNK_SIZE];
		float prev_phase_acc_r[CHUNK_SIZE];
		float next_phase_acc_l[CHUNK_SIZE];
		float next_phase_acc_r[CHUNK_SIZE];
		complex outbuffer_l[CHUNK_SIZE];
		complex outbuffer_r[CHUNK_SIZE];
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
				buffer[i].l += state->outbuffer_l[i].real() * getHannWindow(i);
				buffer[i].r += state->outbuffer_r[i].real() * getHannWindow(i);
				state->carryover[i] = Sample(0, 0);
			}
			
			// 1
			getNextFrame(speed, state);
			for (int i = 0; i < CHUNK_SIZE * 3 / 4; i++){
				buffer[i + CHUNK_SIZE / 4].l += state->outbuffer_l[i].real() * getHannWindow(i);
				buffer[i + CHUNK_SIZE / 4].r += state->outbuffer_r[i].real() * getHannWindow(i);
			}
			for (int i = 0; i < CHUNK_SIZE / 4; i++){
				state->carryover[i].l += state->outbuffer_l[i + CHUNK_SIZE * 3 / 4].real() * getHannWindow(i + CHUNK_SIZE * 3 / 4);
				state->carryover[i].r += state->outbuffer_r[i + CHUNK_SIZE * 3 / 4].real() * getHannWindow(i + CHUNK_SIZE * 3 / 4);
			}

			// 2
			getNextFrame(speed, state);
			for (int i = 0; i < CHUNK_SIZE / 2; i++){
				buffer[i + CHUNK_SIZE / 2].l += state->outbuffer_l[i].real() * getHannWindow(i);
				buffer[i + CHUNK_SIZE / 2].r += state->outbuffer_r[i].real() * getHannWindow(i);
			}
			for (int i = 0; i < CHUNK_SIZE / 2; i++){
				state->carryover[i].l += state->outbuffer_l[i + CHUNK_SIZE / 2].real() * getHannWindow(i + CHUNK_SIZE / 2);
				state->carryover[i].r += state->outbuffer_r[i + CHUNK_SIZE / 2].real() * getHannWindow(i + CHUNK_SIZE / 2);
			}

			// 3
			getNextFrame(speed, state);
			for (int i = 0; i < CHUNK_SIZE / 4; i++){
				buffer[i + CHUNK_SIZE * 3 / 4].l += state->outbuffer_l[i].real() * getHannWindow(i);
				buffer[i + CHUNK_SIZE * 3 / 4].r += state->outbuffer_r[i].real() * getHannWindow(i);
			}
			for (int i = 0; i < CHUNK_SIZE * 3 / 4; i++){
				state->carryover[i].l += state->outbuffer_l[i + CHUNK_SIZE / 4].real() * getHannWindow(i + CHUNK_SIZE / 4);
				state->carryover[i].r += state->outbuffer_r[i + CHUNK_SIZE / 4].real() * getHannWindow(i + CHUNK_SIZE / 4);
			}
		}

		SingleInput input;
		NumberResult timespeed;

		private:

		void swap(PhaseVocoderState* state){
			for (int i = 0; i < CHUNK_SIZE; i++){
				state->prev_l[i] = state->next_l[i];
				state->prev_r[i] = state->next_r[i];
				state->prev_phase_acc_l[i] = state->next_phase_acc_l[i];
				state->prev_phase_acc_r[i] = state->next_phase_acc_r[i];
				state->prev_phase_diffs_l[i] = state->next_phase_diffs_l[i];
				state->prev_phase_diffs_r[i] = state->next_phase_diffs_r[i];
			}


			Sample* bufferA;
			Sample* bufferB;
			if (state->phase < 4){
				bufferA = state->inbuffer1;
				bufferB = state->inbuffer2;
			} else {
				bufferA = state->inbuffer2;
				bufferB = state->inbuffer1;
			}
			if (state->phase % 4 == 1){
				input.getNextChunk(bufferB, state);
			}
			int carryover = CHUNK_SIZE * (state->phase % 4) / 4;
			for (int i = 0; i < CHUNK_SIZE - carryover; i++){
				state->next_l[i] = bufferA[i + carryover].l * getHannWindow(i);
				state->next_r[i] = bufferA[i + carryover].r * getHannWindow(i);
			}
			for (int i = 0; i < carryover; i++){
				state->next_l[i + CHUNK_SIZE - carryover] = bufferB[i].l * getHannWindow(i + CHUNK_SIZE - carryover);
				state->next_r[i + CHUNK_SIZE - carryover] = bufferB[i].r * getHannWindow(i + CHUNK_SIZE - carryover);
			}
			state->phase = (state->phase + 1) % 8;


			fft(state->next_l, CHUNK_SIZE);
			fft(state->next_r, CHUNK_SIZE);
			// cyclic shift
			for (int i = 1; i < CHUNK_SIZE; i += 2){
				state->next_l[i] *= -1.0f;
				state->next_r[i] *= -1.0f;
			}
			for (int i = 0; i < CHUNK_SIZE; i++){
				state->next_phase_diffs_l[i] = std::arg(state->next_l[i]) - std::arg(state->prev_l[i]);
				state->next_phase_diffs_r[i] = std::arg(state->next_r[i]) - std::arg(state->prev_r[i]);
			}
		}

		// write spectrum to outbuffer
		void getNextFrame(float speed, PhaseVocoderState* state){
			while (state->offset > 1.0f){
				swap(state);
				state->offset -= 1.0f;
			}

			for (int i = 0; i < CHUNK_SIZE; i++){

				state->prev_phase_acc_l[i] = fmod(state->prev_phase_acc_l[i] + state->prev_phase_diffs_l[i], 2.0f * 3.141592654f);
				state->prev_phase_acc_r[i] = fmod(state->prev_phase_acc_r[i] + state->prev_phase_diffs_r[i], 2.0f * 3.141592654f);
				state->next_phase_acc_l[i] = fmod(state->next_phase_acc_l[i] + state->next_phase_diffs_l[i], 2.0f * 3.141592654f);
				state->next_phase_acc_r[i] = fmod(state->next_phase_acc_r[i] + state->next_phase_diffs_r[i], 2.0f * 3.141592654f);

				complex prev_l = std::abs(state->prev_l[i]) * complex(cos(state->prev_phase_acc_l[i]), sin(state->prev_phase_acc_l[i]));
				complex prev_r = std::abs(state->prev_r[i]) * complex(cos(state->prev_phase_acc_r[i]), sin(state->prev_phase_acc_r[i]));
				complex next_l = std::abs(state->next_l[i]) * complex(cos(state->next_phase_acc_l[i]), sin(state->next_phase_acc_l[i]));
				complex next_r = std::abs(state->next_r[i]) * complex(cos(state->next_phase_acc_r[i]), sin(state->next_phase_acc_r[i]));

				state->outbuffer_l[i] = prev_l * (1.0f - state->offset) + next_l * state->offset;
				state->outbuffer_r[i] = prev_r * (1.0f - state->offset) + next_r * state->offset;
			}

			// cyclic shift
			for (int i = 1; i < CHUNK_SIZE; i += 2){
				state->outbuffer_l[i] *= -1.0f;
				state->outbuffer_r[i] *= -1.0f;
			}
			ifft(state->outbuffer_l, CHUNK_SIZE);
			ifft(state->outbuffer_r, CHUNK_SIZE);

			state->offset += speed;
		}
	};

}