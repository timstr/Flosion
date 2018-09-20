#pragma once

#include "SoundSource.h"
#include "fft.h"
#include "hannwindow.h"
#include "SoundQueue.h"
#include "SingleInput.h"

namespace musical {

	const unsigned int max_window_size = 1 << 13;

	// TODO: override time speed function
	// TODO: phase locking

	struct PhaseVocoderState : State {
		PhaseVocoderState(const State* parent, const Stateful* owner) :
			State(parent, owner),
			queue_in(max_window_size + CHUNK_SIZE),
			queue_out(max_window_size + CHUNK_SIZE),
			last_window_size(max_window_size) {

		}

		void reset() noexcept override {
			offset = 0.0f;
			frame_position = 0;
			queue_in.clear();
			queue_out.clear();
			clearSpectra();
		}

		void clearSpectra(){
			for (int i = 0; i < max_window_size; i++){
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
			}
		}

		// for analysis
		complex prev_l[max_window_size];	// phase/amp of last window, left channel
		complex prev_r[max_window_size];	// phase/amp of last window, right channel
		complex next_l[max_window_size];	// phase/amp of next window, left channel
		complex next_r[max_window_size];	// phase/amp of next window, right channel


		// for re-synthesis
		float prev_phase_diffs_l[max_window_size];
		float prev_phase_diffs_r[max_window_size];
		float next_phase_diffs_l[max_window_size];
		float next_phase_diffs_r[max_window_size];
		float prev_phase_acc_l[max_window_size];
		float prev_phase_acc_r[max_window_size];
		float next_phase_acc_l[max_window_size];
		float next_phase_acc_r[max_window_size];
		complex outbuffer_l[max_window_size];
		complex outbuffer_r[max_window_size];

		float offset;

		SoundQueue queue_in;
		SoundQueue queue_out;

		unsigned int frame_position;

		unsigned int last_window_size;
	};

	struct PhaseVocoder : SoundSourceTemplate<PhaseVocoderState> {
		PhaseVocoder() : input(this), timespeed(this) {
			window_size = max_window_size;
		}

		void renderChunk(SoundChunk& buffer, PhaseVocoderState& state) override {
			checkWindowChange(window_size, state);
			fillOutBuffer(buffer, window_size, state);
		}

		void setWindowSize(unsigned int size){
			if (size == 256 || size == 512 || size == 1024 || size == 2048 || size == 4096 || size == 8192){
				window_size = size;
			}
		}
		unsigned int getWindowSize() const {
			return window_size;
		}

		double getTimeSpeed(const State* state) const noexcept override {
			return timespeed.getValue(state);
		}

		SingleInput input;
		NumberInput timespeed;

		private:

		unsigned int window_size;

		void swap(PhaseVocoderState& state, unsigned int windowsize){
			for (size_t i = 0; i < windowsize; i++){
				state.prev_l[i] = state.next_l[i];
				state.prev_r[i] = state.next_r[i];
				state.prev_phase_acc_l[i] = state.next_phase_acc_l[i];
				state.prev_phase_acc_r[i] = state.next_phase_acc_r[i];
				state.prev_phase_diffs_l[i] = state.next_phase_diffs_l[i];
				state.prev_phase_diffs_r[i] = state.next_phase_diffs_r[i];
			}

			getNextInput(windowsize, state);

			fft(state.next_l, windowsize);
			fft(state.next_r, windowsize);
			// cyclic shift
			for (size_t i = 1; i < windowsize; i += 2){
				state.next_l[i] *= -1.0f;
				state.next_r[i] *= -1.0f;
			}
			for (size_t i = 0; i < windowsize; i++){
				state.next_phase_diffs_l[i] = std::arg(state.next_l[i]) - std::arg(state.prev_l[i]);
				state.next_phase_diffs_r[i] = std::arg(state.next_r[i]) - std::arg(state.prev_r[i]);
			}
		}

		// write spectrum to outbuffer
		void getNextFrame(PhaseVocoderState& state, unsigned int windowsize){
			while (state.offset > 1.0f){
				swap(state, windowsize);
				state.offset -= 1.0f;
			}

			for (size_t i = 0; i < windowsize; i++){

				state.prev_phase_acc_l[i] = fmod(state.prev_phase_acc_l[i] + state.prev_phase_diffs_l[i], 2.0f * 3.141592654f);
				state.prev_phase_acc_r[i] = fmod(state.prev_phase_acc_r[i] + state.prev_phase_diffs_r[i], 2.0f * 3.141592654f);
				state.next_phase_acc_l[i] = fmod(state.next_phase_acc_l[i] + state.next_phase_diffs_l[i], 2.0f * 3.141592654f);
				state.next_phase_acc_r[i] = fmod(state.next_phase_acc_r[i] + state.next_phase_diffs_r[i], 2.0f * 3.141592654f);

				complex prev_l = std::abs(state.prev_l[i]) * complex(cos(state.prev_phase_acc_l[i]), sin(state.prev_phase_acc_l[i]));
				complex prev_r = std::abs(state.prev_r[i]) * complex(cos(state.prev_phase_acc_r[i]), sin(state.prev_phase_acc_r[i]));
				complex next_l = std::abs(state.next_l[i]) * complex(cos(state.next_phase_acc_l[i]), sin(state.next_phase_acc_l[i]));
				complex next_r = std::abs(state.next_r[i]) * complex(cos(state.next_phase_acc_r[i]), sin(state.next_phase_acc_r[i]));

				float amp_prev = pow(cos(state.offset * 3.141592654f / 2.0f), 2.0f);
				float amp_next = pow(cos((1.0f - state.offset) * 3.141592654f / 2.0f), 2.0f);

				state.outbuffer_l[i] = prev_l * amp_prev + next_l * amp_next;
				state.outbuffer_r[i] = prev_r * amp_prev + next_r * amp_next;
			}

			// cyclic shift
			for (unsigned int i = 1; i < windowsize; i += 2){
				state.outbuffer_l[i] *= -1.0f;
				state.outbuffer_r[i] *= -1.0f;
			}
			ifft(state.outbuffer_l, windowsize);
			ifft(state.outbuffer_r, windowsize);

			state.offset += timespeed.getValue(&state, 1.0f);
		}

		// get next input from input queue, advance input queue
		void getNextInput(unsigned int windowsize, PhaseVocoderState& state){
			const unsigned int hopsize = windowsize / 4;

			state.queue_in.advance(hopsize, input, &state);

			for (unsigned int i = 0; i < windowsize; i++){
				float window = getHannWindow(i, windowsize);
				state.next_l[i] = state.queue_in[i].l * window;
				state.next_r[i] = state.queue_in[i].r * window;
			}
		}

		// add next output to output queue
		void addOutputBuffer(unsigned int windowsize, PhaseVocoderState& state){
			const unsigned int hopsize = windowsize / 4;
			for (unsigned int i = 0; i < windowsize; i++){
				float window = getHannWindow(i, windowsize);
				state.queue_out[i + state.frame_position].l += state.outbuffer_l[i].real() * window;
				state.queue_out[i + state.frame_position].r += state.outbuffer_r[i].real() * window;
			}
		}

		void fillOutBuffer(SoundChunk& buffer, unsigned int windowsize, PhaseVocoderState& state){

			const unsigned int hopsize = windowsize / 4;
			
			while (state.frame_position < CHUNK_SIZE){
				getNextFrame(state, windowsize);
				addOutputBuffer(windowsize, state);
				state.frame_position += hopsize;
			}
			state.frame_position -= CHUNK_SIZE;

			for (int i = 0; i < CHUNK_SIZE; i++){
				buffer[i] = state.queue_out[i];
			}

			state.queue_out.advance(CHUNK_SIZE);
		}

		void checkWindowChange(unsigned int windowsize, PhaseVocoderState& state){
			if (state.last_window_size != windowsize){
				state.clearSpectra();
				state.last_window_size = windowsize;
			}
		}
	};

}