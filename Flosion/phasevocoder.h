#pragma once

#include "SoundSource.h"
#include "NumberResult.h"
#include "fft.h"
#include "hannwindow.h"

namespace musical {

	const unsigned int max_window_size = 1 << 12;

	struct PhaseVocoderState : State {
		PhaseVocoderState(State* parent, Stateful* owner) : State(parent, owner) {
			// allocate maximum number of chunks to avoid dynamic allocation during rendering
			const unsigned int qsize = (max_window_size / CHUNK_SIZE) + 1;
			queue_in.resize(qsize);
			queue_out.resize(qsize);
			for (int i = 0; i < qsize; i++){
				queue_in[i] = new Chunk();
				queue_out[i] = new Chunk();
			}
			last_window_size = max_window_size;
		}
		~PhaseVocoderState(){
			for (int i = 0; i < queue_in.size(); i++){
				delete queue_in[i];
			}
			for (int i = 0; i < queue_out.size(); i++){
				delete queue_out[i];
			}
			queue_in.resize(0);
			queue_out.resize(0);
		}

		void reset() override {
			offset = 0.0f;
			phase = 0;
			qin_offset = 0;
			qout_offset = 0;
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
		int phase;
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

		struct Chunk {
			Chunk(){
				for (int i = 0; i < CHUNK_SIZE; i++){
					data[i] = Sample(0, 0);
				}
			}
			Sample data[CHUNK_SIZE];
		};

		std::vector<Chunk*> queue_in;
		std::vector<Chunk*> queue_out;
		unsigned int qin_offset;
		unsigned int qout_offset;

		unsigned int last_window_size;
	};

	struct PhaseVocoder : SoundSourceTemplate<PhaseVocoderState> {
		PhaseVocoder() : input(this), timespeed(this) {
			window_size = max_window_size;
		}

		void renderChunk(Sample* buffer, PhaseVocoderState* state){
			float speed = timespeed.getValue(state, 1.0f);
			checkWindowChange(window_size, state);
			fillOutBuffer(buffer, window_size, speed, state); // TODO
		}

		void setWindowSize(unsigned int size){
			if (size == 256 || size == 512 || size == 1024 || size == 2048 || size == 4096){
				window_size = size;
			}
		}
		unsigned int getWindowSize() const {
			return window_size;
		}

		SingleInput input;
		NumberResult timespeed;

		private:

		unsigned int window_size;

		void swap(PhaseVocoderState* state, unsigned int windowsize){
			for (int i = 0; i < windowsize; i++){
				state->prev_l[i] = state->next_l[i];
				state->prev_r[i] = state->next_r[i];
				state->prev_phase_acc_l[i] = state->next_phase_acc_l[i];
				state->prev_phase_acc_r[i] = state->next_phase_acc_r[i];
				state->prev_phase_diffs_l[i] = state->next_phase_diffs_l[i];
				state->prev_phase_diffs_r[i] = state->next_phase_diffs_r[i];
			}


			getNextInput(windowsize, state);
			state->phase = (state->phase + 1) % 8;


			fft(state->next_l, windowsize);
			fft(state->next_r, windowsize);
			// cyclic shift
			for (int i = 1; i < windowsize; i += 2){
				state->next_l[i] *= -1.0f;
				state->next_r[i] *= -1.0f;
			}
			for (int i = 0; i < windowsize; i++){
				state->next_phase_diffs_l[i] = std::arg(state->next_l[i]) - std::arg(state->prev_l[i]);
				state->next_phase_diffs_r[i] = std::arg(state->next_r[i]) - std::arg(state->prev_r[i]);
			}
		}

		// write spectrum to outbuffer
		void getNextFrame(float speed, PhaseVocoderState* state, unsigned int windowsize){
			while (state->offset > 1.0f){
				swap(state, windowsize);
				state->offset -= 1.0f;
			}

			for (int i = 0; i < windowsize; i++){

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
			for (int i = 1; i < windowsize; i += 2){
				state->outbuffer_l[i] *= -1.0f;
				state->outbuffer_r[i] *= -1.0f;
			}
			ifft(state->outbuffer_l, windowsize);
			ifft(state->outbuffer_r, windowsize);

			state->offset += speed;
		}

		// get next input from input queue, advance input queue
		void getNextInput(unsigned int windowsize, PhaseVocoderState* state){
			const unsigned int hopsize = windowsize / 4;
			state->qin_offset += hopsize;
			while (state->qin_offset >= CHUNK_SIZE){
				state->qin_offset -= CHUNK_SIZE;
				// get oldest chunk
				PhaseVocoderState::Chunk* ch = state->queue_in[0];
				// fill with new data
				input.getNextChunk(ch->data, state);
				// and move to newest position
				state->queue_in.erase(state->queue_in.begin());
				state->queue_in.push_back(ch);
			}

			// fill input buffers
			int chunk = 0;
			int sample = state->qin_offset;
			for (int i = 0; i < windowsize; i++){
				state->next_l[i] = state->queue_in[chunk]->data[sample].l * getHannWindow(i, windowsize);
				state->next_r[i] = state->queue_in[chunk]->data[sample].r * getHannWindow(i, windowsize);
				sample += 1;
				if (sample == CHUNK_SIZE){
					sample = 0;
					chunk += 1;
				}
			}
		}

		// add next output to output queue, advance output queue
		void addOutputBuffer(unsigned int windowsize, PhaseVocoderState* state){
			int chunk = 0;
			int sample = state->qout_offset;
			for (int i = 0; i < windowsize; i++){
				state->queue_out[chunk]->data[sample].l += state->outbuffer_l[i].real() * getHannWindow(i, windowsize);
				state->queue_out[chunk]->data[sample].r += state->outbuffer_r[i].real() * getHannWindow(i, windowsize);

				sample += 1;
				if (sample == CHUNK_SIZE){
					sample = 0;
					chunk += 1;
				}
			}
		}

		void fillOutBuffer(Sample* buffer, unsigned int windowsize, float speed, PhaseVocoderState* state){
			const unsigned int hopsize = windowsize / 4;
			
			while (state->qout_offset < CHUNK_SIZE){
				getNextFrame(speed, state, windowsize);
				addOutputBuffer(windowsize, state);
				state->qout_offset += hopsize;
			}


			// get oldest chunk
			PhaseVocoderState::Chunk* ch = state->queue_out[0];
			// copy and empty data
			for (int i = 0; i < CHUNK_SIZE; i++){
				buffer[i] = ch->data[i];
				ch->data[i] = Sample(0, 0);
			}
			// and move to newest position
			state->queue_out.erase(state->queue_out.begin());
			state->queue_out.push_back(ch);
			state->qout_offset -= CHUNK_SIZE;
		}

		void checkWindowChange(unsigned int windowsize, PhaseVocoderState* state){
			if (state->last_window_size != windowsize){
				state->clearSpectra();
				state->last_window_size = windowsize;
			}
		}
	};

}