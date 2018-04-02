#pragma once

#include "SoundSource.h"

namespace musical {

	struct SoundResult {
		SoundResult();
		~SoundResult();

		void getNextChunk(Buffer& buffer);

		void reset();

		void setSource(SoundSource* source);

		SingleInput input;
	};

}