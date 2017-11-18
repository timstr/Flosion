#pragma once

#include "SoundSource.h"
#include "NumberResult.h"
#include "fft.h"
#include "hannwindow.h"

namespace musical {

	// TODO: overlap and add for (hopefully) clean sound
	// TODO: separate left and right channels for stereo

	struct PhaseVocoderState : State {
		using State::State;

		void reset() override {
			offset = 1.0f;
			for (int i = 0; i < CHUNK_SIZE; i++){
				buffer[i] = Sample(0, 0);
				prev[i] = 0.0f;
				next[i]= 0.0f;
				phase_diffs[i] = 0.0f;
				phase_acc[i] = 0.0f;
				cbuffer[i] = 0.0f;
			}
		}

		// assumes buffer has been filled with next chunk of audio data
		void swap(){
			// shuffle freq spectrums over
			for (int i = 0; i < CHUNK_SIZE; i++){
				prev[i] = next[i];
				next[i] = buffer[i].l * getHannWindow(i); // TODO: non-stereo, right here
			}

			// compute next spectrum from new audio data
			fft(next, CHUNK_SIZE);

			for (int i = 0; i < CHUNK_SIZE; i++){
				phase_diffs[i] = std::arg(next[i]) - std::arg(prev[i]);
			}
		}

		void synthesize(){
			for (int i = 0; i < CHUNK_SIZE; i++){
				// linear interpolation of magnitude
				float mag = std::abs(prev[i]) * (1.0f -  offset) + std::abs(next[i]) * offset;

				// phase is accumulated
				phase_acc[i] = fmod(phase_acc[i] + phase_diffs[i], 2.0f * 3.141592654f);

				cbuffer[i] = mag * complex(cos(phase_acc[i]), sin(phase_acc[i]));
			}
			ifft(cbuffer, CHUNK_SIZE);
			for (int i = 0; i < CHUNK_SIZE; i++){
				buffer[i].l = cbuffer[i].real();
				buffer[i].r = buffer[i].l;
			}
		}

		// for analysis
		Sample buffer[CHUNK_SIZE];	// for input audio data
		complex prev[CHUNK_SIZE];	// phase/amp of last window
		complex next[CHUNK_SIZE];	// phase/amp of next window


		// for re-synthesis
		float phase_diffs[CHUNK_SIZE];
		float phase_acc[CHUNK_SIZE];
		complex cbuffer[CHUNK_SIZE];

		float offset;
	};

	struct PhaseVocoder : SoundSourceTemplate<PhaseVocoderState> {
		PhaseVocoder() : input(this), timespeed(this) {

		}

		void renderChunk(Sample* buffer, PhaseVocoderState* state){
			// get next spectrum if needed
			if (state->offset >= 1.0f){
				state->offset -= 1.0f;

				input.getNextChunk(state->buffer, state);
				state->swap();
			}

			state->synthesize();

			for (int i = 0; i < CHUNK_SIZE; i++){
				buffer[i] = state->buffer[i];
			}

			float speed = timespeed.getValue(state, 1.0f);
			state->offset += speed;
		}

		SingleInput input;
		NumberResult timespeed;
	};

}