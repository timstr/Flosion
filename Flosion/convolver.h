#pragma once

#include "SoundSource.h"
#include "NumberResult.h"
#include "SoundResult.h"
#include <list>

namespace musical {

	struct ConvolverState : State {
		using State::State;

	};

	struct Convolver : SoundSourceTemplate<ConvolverState> {
		Convolver() : function(this, -1, 1), function_length(this, 0, 1), input(this) {

		}

		void renderChunk(Sample* buffer, ConvolverState* state) override {

		}


		NumberResult function;
		NumberResult function_length;
		SingleInput input;
	};
}