#pragma once

#include "musical.h"

namespace musical {

	struct SamplerState : State {
		using State::State;

		void reset() override {
			time = 0;
		}

		uint32_t time;
	};

	struct Sampler : SoundSourceTemplate<SamplerState> {
		Sampler() : input(this) {

		}

		void renderChunk(Sample* buffer, SamplerState* state) override {
			for (auto st = input.begin(state); st != input.end(state); st++){
				// if the note is playing now
				if (st.key()->start_time <= state->time + CHUNK_SIZE && st.key()->start_time + st.key()->length > state->time){
					int carryover = st.key()->start_time % CHUNK_SIZE;
					// if this is the note's first time playing...
					if (st.key()->start_time >= state->time){
						input.resetState(st);
					} else {
						int carryover = st.key()->start_time % CHUNK_SIZE;
						for (int i = 0; i < carryover; i++){
							buffer[i] += st.state().buffer[CHUNK_SIZE - carryover + i] * 0.1;
						}
					}
					// get the next chunk for the note
					input.getNextChunk(st.state().buffer, st);
					// write its first part to the output buffer
					for (int i = carryover; i < CHUNK_SIZE; i++){
						buffer[i] += st.state().buffer[i - carryover] * 0.1;
					}
				}
			}

			state->time += CHUNK_SIZE;
		}

		struct Note {
			Note(double _frequency, double _amplitude, uint32_t _start_time, uint32_t _length){
				frequency = _frequency;
				amplitude = _amplitude;
				start_time = _start_time;
				length = _length;
			}
			double frequency;
			double amplitude;
			uint32_t start_time;
			uint32_t length;
		};

		Note* addNote(double frequency, double amplitude, uint32_t start_time, uint32_t length){
			Note* note = new Note(frequency, amplitude, start_time, length);
			notes.push_back(note);
			input.addKey(note);
			return note;
		}

		void removeNote(Note* note){
			for (auto it = notes.begin(); it != notes.end(); it++){
				if (*it == note){
					notes.erase(it);
					input.removeKey(note);
					return;
				}
			}
			throw std::runtime_error("The Note could not be found");
		}

		struct InputState : MultiInputState<Note*> {
			using MultiInputState::MultiInputState;

			void reset() override {
				for (int i = 0; i < CHUNK_SIZE; i++){
					buffer[i] = {0, 0};
				}
			}

			Sample buffer[CHUNK_SIZE];
		};

		struct SamplerInput : MultiInput<InputState, Note*> {
			SamplerInput(SoundSource* parent) : MultiInput(parent), frequency(this), amplitude(this) {

			}

			struct Frequency : MultiInputNumberSource<InputState> {
				using MultiInputNumberSource::MultiInputNumberSource;
				float getValue(InputState* state, int chunk_pos) override {
					return state->key->frequency;
				}
			} frequency;

			struct Amplitude : MultiInputNumberSource<InputState> {
				using MultiInputNumberSource::MultiInputNumberSource;
				float getValue(InputState* state, int chunk_pos) override {
					return state->key->amplitude;
				}
			} amplitude;
		} input;

		private:
		std::vector<Note*> notes;
	};

}