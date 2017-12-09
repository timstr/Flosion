#pragma once

#include "musical.h"
#include "SoundSource.h"
#include "NumberResult.h"
#include <math.h>
#include <algorithm>

namespace musical {

	struct DistortionState : State {
		using State::State;

		void reset() override {

		}
	};

	struct Distortion : SoundSourceTemplate<DistortionState> {
		Distortion() : gain(this), hardness(this), input(this) {

		}

		void renderChunk(Sample* buffer, DistortionState* state) override {
			input.getNextChunk(buffer, state);
			for (int i = 0; i < CHUNK_SIZE; i++){
				float h = hardness.getValue(state, 0.0f);
				float g = gain.getValue(state, 1.0f);
				buffer[i].l = distort(buffer[i].l, g, h);
				buffer[i].r = distort(buffer[i].r, g, h);
			}
		}

		SingleInput input;
		NumberResult gain;
		NumberResult hardness;

		private:
		
		int sign(float x) const {
			return (0.0f < x) - (x < 0.0f);
		}

		float distort(float x, float gain, float sharpness) const {
			return sign(x) * (1.0f - pow(1.0f - std::min(exp(gain) * abs(x), 1.0f), exp(sharpness)));
		}
	};
}