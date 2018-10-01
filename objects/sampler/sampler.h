#pragma once

#include "spline.h"
#include "Sample.h"
#include "MultiInput.h"

namespace flo {

	// TODO: rename to Note Sequencer or similar, something with Sequence at any rate

	struct SamplerState : State {
		using State::State;

		void reset() noexcept override {
			time = 0;
		}

		uint32_t time;
	};

	struct Sampler : SoundSourceBase<SamplerState> {
		Sampler() : input(this) {
			length = SFREQ * 4;
		}

		void renderChunk(SoundChunk& buffer, SamplerState& state) override {
			unsigned int sample = 0;

			while (sample < CHUNK_SIZE){
				unsigned int len = std::min((uint32_t)CHUNK_SIZE, length - state.time);
				for (auto it = input.begin(&state), end = input.end(&state); it != end; it++){
					Note* note = it.key();
					// if the note is playing now
					// specifically, if the note both begins before the end of the chunk and ends after the beginning of the chunk
					if (note->start_time * SFREQ < (state.time + CHUNK_SIZE) && (note->start_time + note->length) * SFREQ >= state.time){
						unsigned int carryover = (uint32_t)(note->start_time * SFREQ) % CHUNK_SIZE;

						// if this is the note's first time playing...
						if (note->start_time * SFREQ >= state.time){
							// reset it
							input.resetState(it);
							it.state().performReset();
						} else {
							// add the trailing part of its buffer
							for (size_t i = sample; i < std::min(carryover, len + sample); i++){
								buffer[i] += it.state().buffer[CHUNK_SIZE - carryover + i] * 0.1f; // TODO: perform some kind of mixing/compression here instead of quieting
							}
						}
						// get the next chunk for the note
						input.getNextChunk(it.state().buffer, it);
						// write its first part to the output buffer
						for (size_t i = std::max(carryover, sample); i < std::min((uint32_t)CHUNK_SIZE, len + sample); i++){
							buffer[i] += it.state().buffer[i - carryover] * 0.1f; // TODO: see above
						}
					}
				}

				state.advanceTime(len);
				state.saveTime();

				sample += len;
				state.time += len;
				if (len < CHUNK_SIZE){
					state.time = 0;
				}
			}
		}

		// TODO: switch start time and length to integer sample counts
		struct Note {
			Note(float _frequency, float _amplitude, float _start_time, float _length){
				frequency.useConstant();
				frequency.getConstant().setValue(_frequency);
				frequency.getSpline().setMinX(0.0f);
				frequency.getSpline().setMaxX(1.0f);
				frequency.getSpline().setMinY(0.0f);
				frequency.getSpline().setMaxY(20000.0f);
				amplitude = _amplitude;
				start_time = _start_time;
				length = _length;
			}

			struct Parameter {
				Parameter(){
					mode = 0;
				}
				void useConstant(){
					mode = 0;
				}
				void useSpline(){
					mode = 1;
				}
				flo::Constant& getConstant(){
					return constant;
				}
				flo::Spline& getSpline(){
					return spline;
				}
				float getValue(float progress){
					if (mode == 0){
						return constant.getValue();
					} else {
						return spline.getValueAt(progress);
					}
				}
				private:
				flo::Constant constant;
				flo::Spline spline;
				int mode;
			};

			Parameter& addParameter(int id){
				return parameters[id];
			}
			Parameter& getParameter(int id){
				return parameters[id];
			}
			void removeParameter(int id){
				parameters.erase(id);
			}

			float amplitude;
			float start_time;
			float length;
			Parameter frequency;

			private:
			std::map<int, Parameter> parameters;
		};

		Note* addNote(float frequency, float amplitude, float start_time, float length){
			Note* note = new Note(frequency, amplitude, start_time, length);
			for (int id : parameter_ids){
				note->addParameter(id);
			}
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

		void setLength(float len){
			length = std::max(0u, (uint32_t)(len * SFREQ));
		}
		float getLength() const {
			return length / (float)SFREQ;
		}

		struct InputState : MultiInputState<Note*> {
			using MultiInputState::MultiInputState;

			void reset() noexcept override {
				for (int i = 0; i < CHUNK_SIZE; i++){
					buffer[i] = {0, 0};
				}
				time_zero = 0.0;
			}

			SoundChunk buffer;
			double time_zero;
		};

		struct SamplerInput : MultiInput<InputState, Note*, Sampler> {
			SamplerInput(Sampler* parent) : MultiInput(parent), frequency(this), amplitude(this), notetime(this), noteprogress(this) {

			}

			struct ParameterFunction : StateNumberSource {
				ParameterFunction(int _id, SamplerInput* samplerinput) : StateNumberSource(samplerinput) {
					id = _id;
				}
				float getValue(const InputState& state, const State* context) const noexcept override {
					Note* note = state.key;
					float progress = (context->getTimeAt(parentmultiinput) / (float)SFREQ) / state.key->length;
					return note->getParameter(id).getValue(progress);
				}
				int id;
			};

			ParameterFunction* addParameter(int id){
				for (auto it = paramfns.begin(); it != paramfns.end(); it++){
					if ((*it)->id == id){
						throw std::runtime_error("A parameter with the provided id already exists");
					}
				}
				ParameterFunction* pf = new ParameterFunction(id, this);
				paramfns.push_back(pf);
				return pf;
			}
			void removeParameter(int id){
				for (auto it = paramfns.begin(); it != paramfns.end(); it++){
					if ((*it)->id == id){
						delete *it;
						paramfns.erase(it);
						return;
					}
				}
				throw std::runtime_error("A parameter with the provided id could not be found");
			}

			struct Frequency : StateNumberSource {
				using StateNumberSource::StateNumberSource;
				float getValue(const InputState& state, const State* context) const noexcept override {
					Note* note = state.key;
					float progress = (context->getTimeAt(parentmultiinput) / (float)SFREQ) / state.key->length;
					return note->frequency.getValue(progress);
				}
			} frequency;

			struct Amplitude : StateNumberSource {
				using StateNumberSource::StateNumberSource;
				float getValue(const InputState& state, const State* context) const noexcept override {
					return state.key->amplitude;
				}
			} amplitude; // TODO: keep or remove?

			struct NoteTime : StateNumberSource {
				using StateNumberSource::StateNumberSource;
				float getValue(const InputState& state, const State* context) const noexcept override {
					return context->getTimeAt(parentmultiinput) / (float)SFREQ;
				}
			} notetime;

			struct NoteProgress : StateNumberSource {
				using StateNumberSource::StateNumberSource;
				float getValue(const InputState& state, const State* context) const noexcept override {
					return context->getTimeAt(parentmultiinput) / (float)SFREQ / state.key->length;
				}
			} noteprogress;

			private:

			std::vector<ParameterFunction*> paramfns;
		} input;

		SamplerInput::ParameterFunction* addParameter(int id){
			parameter_ids.insert(id);
			for (Note* note : notes){
				note->addParameter(id);
			}
			return input.addParameter(id);
		}
		void removeParameter(int id){
			parameter_ids.erase(id);
			for (Note* note : notes){
				note->removeParameter(id);
			}
			input.removeParameter(id);
		}

		private:
		std::vector<Note*> notes;
		std::set<int> parameter_ids;
		uint32_t length;
	};

}