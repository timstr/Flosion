#pragma once

#include "SoundSource.h"
#include "SoundInput.h"

namespace musical {

	struct SoundResult {
		SoundResult();
		~SoundResult();

		void getNextChunk(Sample* buffer);

		void reset();

		void setSource(SoundSource* source);

		SingleInput input;
	};

}