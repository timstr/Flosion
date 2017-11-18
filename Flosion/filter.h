#pragma once

#include "soundsource.h"
#include "fft.h"
#include "hannwindow.h"
#include "NumberResult.h"
#include <algorithm>

namespace musical {

	struct FilterState : State {
		using State::State;

		void reset() override {
			frequency_index = 0.0f;
			for (unsigned int i = 0; i < CHUNK_SIZE; i++){
				phase1_l[i] = 0.0f;
				phase1_r[i] = 0.0f;
				phase2_l[i] = 0.0f;
				phase2_r[i] = 0.0f;
				buffer[i] = Sample(0.0f, 0.0f);
			}
		}

		float frequency_index;

		complex phase1_l[CHUNK_SIZE];
		complex phase1_r[CHUNK_SIZE];
		complex phase2_l[CHUNK_SIZE];
		complex phase2_r[CHUNK_SIZE];
		Sample buffer[CHUNK_SIZE];
	};


	struct Filter : SoundSourceTemplate<FilterState> {
		Filter() : input(this), frequency_out(this), amplitude_in(this) {

		}
		~Filter(){

		}

		void renderChunk(Sample* buffer, FilterState* state) override {
			const unsigned int size = CHUNK_SIZE;
			const unsigned int halfsize = size / 2;

			input.getNextChunk(state->buffer, state);

			// 1
			for (unsigned int i = 0; i < halfsize; i++){
				state->phase1_l[i + halfsize] = state->buffer[i].l;
				state->phase1_r[i + halfsize] = state->buffer[i].r;
			}

			fft(state->phase1_l, size);
			fft(state->phase1_r, size);

			state->skipTime(halfsize);

			// silly testing below //////////////////////
			for (int i = 0; i < halfsize; i++){
				state->frequency_index = i;
				float amplitude = amplitude_in.getValue(state, 1.0f) * 2.0f;
				state->phase1_l[i] *= amplitude;
				state->phase1_r[i] *= amplitude;
			}
			for (int i = halfsize; i < size; i++){
				state->phase1_l[i] = 0.0f;
				state->phase1_r[i] = 0.0f;
			}
			/////////////////////////////////////////////

			ifft(state->phase1_l, size);
			ifft(state->phase1_r, size);

			for (unsigned int i = 0; i < size; i++){
				buffer[i].l = state->phase1_l[i].real() * getHannWindow(i);
				buffer[i].r = state->phase1_r[i].real() * getHannWindow(i);
			}

			// 2
			for (unsigned int i = 0; i < halfsize; i++){
				state->phase1_l[i] = state->buffer[i + halfsize].l;
				state->phase1_r[i] = state->buffer[i + halfsize].r;
			}

			// 3
			for (unsigned int i = 0; i < halfsize; i++){
				buffer[i].l += state->phase2_l[i + halfsize].real() * getHannWindow(i + halfsize);
				buffer[i].r += state->phase2_r[i + halfsize].real() * getHannWindow(i + halfsize);
			}

			// 4
			for (unsigned int i = 0; i < size; i++){
				state->phase2_l[i] = state->buffer[i].l;
				state->phase2_r[i] = state->buffer[i].r;
			}

			fft(state->phase2_l, size);
			fft(state->phase2_r, size);

			state->skipTime(halfsize);
			
			// silly testing below //////////////////////
			for (int i = 0; i < halfsize; i++){
				state->frequency_index = i;
				float amplitude = amplitude_in.getValue(state, 1.0f) * 2.0f;
				state->phase2_l[i] *= amplitude;
				state->phase2_r[i] *= amplitude;
			}
			for (int i = halfsize; i < size; i++){
				state->phase2_l[i] = 0.0f;
				state->phase2_r[i] = 0.0f;
			}
			/////////////////////////////////////////////

			ifft(state->phase2_l, size);
			ifft(state->phase2_r, size);

			for (unsigned int i = 0; i < halfsize; i++){
				buffer[i + halfsize].l += state->phase2_l[i].real() * getHannWindow(i);
				buffer[i + halfsize].r += state->phase2_r[i].real() * getHannWindow(i);
			}

			state->commitTime();
		}

		static float getFrequencyFromIndex(unsigned int index){
			return index * SFREQ / (float)CHUNK_SIZE;
		}
		static int getIndexFromFrequency(float frequency){
			return frequency * CHUNK_SIZE / (float)SFREQ;
		}

		SingleInput input;

		struct FrequencyOut : StateNumberSource<Filter> {
			using StateNumberSource::StateNumberSource;
			float getValue(FilterState* state, State* context) const {
				return getFrequencyFromIndex(state->frequency_index);
			}
		} frequency_out;

		NumberResult amplitude_in;
	};
}