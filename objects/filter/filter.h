#pragma once

#include "SoundSource.h"
#include "fft.h"
#include "hannwindow.h"
#include <algorithm>
#include "SingleInput.h"

namespace flo {

	struct FilterState : State {
		using State::State;

		void reset() noexcept override {
			frequency_index = 0;
			for (unsigned int i = 0; i < CHUNK_SIZE; i++){
				phase1_l[i] = 0.0f;
				phase1_r[i] = 0.0f;
				phase2_l[i] = 0.0f;
				phase2_r[i] = 0.0f;
				buffer[i] = Sample(0.0f, 0.0f);
			}
		}

		int frequency_index;

		complex phase1_l[CHUNK_SIZE];
		complex phase1_r[CHUNK_SIZE];
		complex phase2_l[CHUNK_SIZE];
		complex phase2_r[CHUNK_SIZE];
		SoundChunk buffer;
	};


	struct Filter : SoundSourceBase<FilterState> {
		Filter() : 
			SoundSourceBase<FilterState>(true),
			input(this),
			frequency_out(this),
			amplitude_in(this) {

		}
		~Filter(){

		}

		void renderChunk(SoundChunk& chunk, FilterState& state) override {
			const unsigned int size = CHUNK_SIZE;
			const unsigned int halfsize = size / 2;

			input.getNextChunk(state.buffer, &state);

			// 1
			for (unsigned int i = 0; i < halfsize; i++){
				state.phase1_l[i + halfsize] = state.buffer[i].l;
				state.phase1_r[i + halfsize] = state.buffer[i].r;
			}

			fft(state.phase1_l, size);
			fft(state.phase1_r, size);

			state.advanceTime(halfsize);

			for (int i = 0; i < halfsize; i++){
				state.frequency_index = i;
				float amplitude = amplitude_in.getValue(&state, 1.0f) * 2.0f;
				state.phase1_l[i] *= amplitude;
				state.phase1_r[i] *= amplitude;
			}
			for (int i = halfsize; i < size; i++){
				state.phase1_l[i] = 0.0f;
				state.phase1_r[i] = 0.0f;
			}

			ifft(state.phase1_l, size);
			ifft(state.phase1_r, size);

			for (unsigned int i = 0; i < size; i++){
				chunk[i].l = state.phase1_l[i].real() * getHannWindow(i, CHUNK_SIZE);
				chunk[i].r = state.phase1_r[i].real() * getHannWindow(i, CHUNK_SIZE);
			}

			// 2
			for (unsigned int i = 0; i < halfsize; i++){
				state.phase1_l[i] = state.buffer[i + halfsize].l;
				state.phase1_r[i] = state.buffer[i + halfsize].r;
			}

			// 3
			for (unsigned int i = 0; i < halfsize; i++){
				chunk[i].l += state.phase2_l[i + halfsize].real() * getHannWindow(i + halfsize, CHUNK_SIZE);
				chunk[i].r += state.phase2_r[i + halfsize].real() * getHannWindow(i + halfsize, CHUNK_SIZE);
			}

			// 4
			for (unsigned int i = 0; i < size; i++){
				state.phase2_l[i] = state.buffer[i].l;
				state.phase2_r[i] = state.buffer[i].r;
			}

			fft(state.phase2_l, size);
			fft(state.phase2_r, size);

			state.advanceTime(halfsize);
			
			for (int i = 0; i < halfsize; i++){
				state.frequency_index = i;
				float amplitude = amplitude_in.getValue(&state, 1.0f) * 2.0f;
				state.phase2_l[i] *= amplitude;
				state.phase2_r[i] *= amplitude;
			}
			for (int i = halfsize; i < size; i++){
				state.phase2_l[i] = 0.0f;
				state.phase2_r[i] = 0.0f;
			}

			ifft(state.phase2_l, size);
			ifft(state.phase2_r, size);

			for (unsigned int i = 0; i < halfsize; i++){
				chunk[i + halfsize].l += state.phase2_l[i].real() * getHannWindow(i, CHUNK_SIZE);
				chunk[i + halfsize].r += state.phase2_r[i].real() * getHannWindow(i, CHUNK_SIZE);
			}
		}

		static float getFrequencyFromIndex(unsigned int index){
			return index * SFREQ / (float)CHUNK_SIZE;
		}
		static int getIndexFromFrequency(float frequency){
			return (int)(frequency * CHUNK_SIZE / (float)SFREQ);
		}

		SingleInput input;

		struct FrequencyOut : StateNumberSource<Filter> {
			using StateNumberSource::StateNumberSource;
			float getValue(const FilterState& state, const State* context) const noexcept override {
				return getFrequencyFromIndex(state.frequency_index);
			}
		} frequency_out;

		NumberInput amplitude_in;
	};
}