#pragma once

#include "SoundSource.h"

namespace flo {

	SoundSource::~SoundSource() {
		// NOTE: removeAllDependants is not called here but should instead
		// be called in concrete SoundSource classes to avoid nonesense
		// virtual function calls regarding removing states
	}

	void SoundSource::removeAllDependants() {
		while (destinations.size() > 0){
			destinations.back()->setSource(nullptr);
		}
	}

}