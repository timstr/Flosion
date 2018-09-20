#pragma once

#include "SoundSource.h"

namespace musical {

	

	SoundSource::~SoundSource() {
		while (destinations.size() > 0){
			destinations.back()->setSource(nullptr);
		}
	}

}