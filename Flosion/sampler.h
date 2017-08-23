#pragma once

#include "musical.h"

namespace musical {

	struct SamplerState : State {
		using State::State;

		void reset() override {

		}
	};

	struct Sampler : SoundSourceTemplate<SamplerState> {
		Sampler() : input(this) {

		}

		void renderChunk(Sample* buffer, SamplerState* state) override {
			double sampler_time = state->getTime();
			double delta = CHUNK_SIZE / (double)SFREQ;

			for (auto it = input.begin(state); it != input.end(state); it++){
				Note* note = it.key();
				// if the note is playing now
				if (note->start_time < (sampler_time + delta) && (note->start_time + note->length) >= sampler_time){
					int carryover = (uint32_t)(note->start_time * SFREQ) % CHUNK_SIZE;

					// if this is the note's first time playing...
					if (note->start_time >= sampler_time){
						// reset it
						input.resetState(it);
					} else {
						// add the trailing part of its buffer
						for (int i = 0; i < carryover; i++){
							buffer[i] += it.state().buffer[CHUNK_SIZE - carryover + i] * 0.1; // TODO: perform some kind of mixing/compression here instead of quieting
						}
					}
					// get the next chunk for the note
					input.getNextChunk(it.state().buffer, it);
					// write its first part to the output buffer
					for (int i = carryover; i < CHUNK_SIZE; i++){
						buffer[i] += it.state().buffer[i - carryover] * 0.1; // TODO: see above
					}
				}
			}

			state->skipTime(CHUNK_SIZE);
		}

		struct Note {
			Note(double _frequency, double _amplitude, double _start_time, double _length){
				frequency = _frequency;
				amplitude = _amplitude;
				start_time = _start_time;
				length = _length;
			}
			double frequency;
			double amplitude;
			double start_time;
			double length;
		};

		Note* addNote(double frequency, double amplitude, double start_time, double length){
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

		struct SamplerInput : MultiInput<InputState, Note*, Sampler> {
			SamplerInput(Sampler* parent) : MultiInput(parent), frequency(this), amplitude(this), notetime(this), noteprogress(this) {

			}

			struct Frequency : StateNumberSource {
				using StateNumberSource::StateNumberSource;
				float getValue(InputState* state, State* context) const override {
					return state->key->frequency;
				}
			} frequency;

			struct Amplitude : StateNumberSource {
				using StateNumberSource::StateNumberSource;
				float getValue(InputState* state, State* context) const override {
					return state->key->amplitude;
				}
			} amplitude;

			struct NoteTime : StateNumberSource {
				using StateNumberSource::StateNumberSource;
				float getValue(InputState* state, State* context) const override {
					return context->getTimeAt(parentmultiinput);
				}
			} notetime;

			struct NoteProgress : StateNumberSource {
				using StateNumberSource::StateNumberSource;
				float getValue(InputState* state, State* context) const override {
					return context->getTimeAt(parentmultiinput) / state->key->length;
				}
			} noteprogress;
		} input;

		private:
		std::vector<Note*> notes;
	};

}