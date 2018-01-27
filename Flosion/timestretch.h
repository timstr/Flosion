#pragma once

#include "musical.h"

namespace musical {

	struct TimeStretchState : State {
		using State::State;
		void reset() override {
			for (int i = 0; i < CHUNK_SIZE; i++){
				buffer[i] = {0, 0};
			}
			bufferpos = CHUNK_SIZE;
			carryover = 0;
			prev = {0, 0};
			next = {0, 0};
			speed = 1.0f;
		}

		Sample advanceAndGetSample(float delta, SingleInput& input, State* statechain){
			// carryover will always be less than 1 from previous call
			float x0 = carryover;
			carryover += delta;
			float x1 = carryover;

			// x0 is initial position from beginning of 'prev' sample
			// x1 is final position from beginning of 'prev' sample
			// bufferpos is to be the sample immediately after 'prev'

			if (x1 < 1){
				// if both points are in the same sample, integrate between them and average
				Sample Fx0 = F(prev, next, x0);
				Sample Fx1 = F(prev, next, x1);

				// bufferpos cannot have advanced, nothing needs to be updated
				return (Fx1 - Fx0) / (x1 - x0);
			} else {
				// integrate the right part of the previous sample
				Sample area = F(prev, next, 1) - F(prev, next, x0);
				float length = (1 - x0);

				// integrate fully over every whole sample in the middle (if any)
				while (x1 >= 1){
					// advance bufferpos, get next and next_slope, swap prev and prev_slope
					prev = next;
					bufferpos += 1;
					if (bufferpos >= CHUNK_SIZE){
						bufferpos -= CHUNK_SIZE;
						input.getNextChunk(buffer, statechain);
					}
					next = buffer[bufferpos];

					// for every full sample, take its entire area
					// the last sample here shall be incomplete and left for outside the loop
					// hence the x1 > 2
					if (x1 >= 2){
						area += F(prev, next, 1);
						length += 1;
					}

					x1 -= 1;
				}

				// integrate the left part of the next sample
				area += F(prev, next, x1);
				length += x1;

				carryover = x1;

				return area / length;
			}
		}

		inline Sample F(Sample prev, Sample next, float x) const {
			return prev * x + (next - prev) * (x * x / 2.0f);
		}

		Buffer buffer;
		Sample prev;
		Sample next;
		unsigned int bufferpos;
		float carryover;
		float speed;
	};

	struct TimeStretch : SoundSourceTemplate<TimeStretchState> {
		TimeStretch() : input(this), speed(this) {

		}

		void renderChunk(Buffer& buffer, TimeStretchState* state) override {
			for (int i = 0; i < CHUNK_SIZE; i++){
				state->speed = speed.getValue(state, 1);
				if (state->speed <= 0.001){
					buffer[i] = {0, 0};
				} else {
					if (state->speed > 16){
						state->speed = 16;
					}
					buffer[i] = state->advanceAndGetSample(state->speed, input, state);
				}
				state->tick();
			}
		}

		float getTimeSpeed(State* s) override {
			if (s->getOwner() != this){
				throw std::runtime_error("Bad things have happened");
			}
			return ((TimeStretchState*)s)->speed;
		}

		NumberResult speed;

		SingleInput input;
	};
}