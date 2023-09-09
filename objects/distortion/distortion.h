#pragma once

#include "Sample.h"
#include "SoundSource.h"
#include <math.h>
#include <algorithm>
#include "SingleInput.h"

namespace flo {

	struct DistortionState : State {
		using State::State;

		void reset() noexcept override {

		}
	};

	struct Distortion : SoundSourceBase<DistortionState> {
		Distortion() :
			SoundSourceBase<DistortionState>(true),
			gain(this),
			hardness(this),
			input(this) {

		}

		void renderChunk(SoundChunk& buffer, DistortionState& state) override {
			input.getNextChunk(buffer, &state);
			for (int i = 0; i < CHUNK_SIZE; i++){
				float h = hardness.getValue(&state, 0.0f);
				float g = gain.getValue(&state, 0.0f);
				buffer[i].l = std::min(std::max(distort(buffer[i].l, g, h), -1.0f), 1.0f);
				buffer[i].r = std::min(std::max(distort(buffer[i].r, g, h), -1.0f), 1.0f);
			}
		}

		SingleInput input;
		NumberInput gain;
		NumberInput hardness;

		private:
		
		int sign(float x) const {
			return (0.0f < x) - (x < 0.0f);
		}

		float distort(float x, float gain, float sharpness) const {
			return sign(x) * (1.0f - pow(1.0f - std::min(exp(gain) * abs(x), 1.0f), exp(sharpness)));
		}
	};
}