#include "SoundSource.h"

namespace flo {

	SoundSource::SoundSource(bool _is_realtime) noexcept :
		is_realtime(_is_realtime) {

	}

	SoundSource::~SoundSource() noexcept {
		// NOTE: removeAllDependants is not called here but should instead
		// be called in concrete SoundSource classes to avoid nonesense
		// virtual function calls regarding removing states
	}

	void SoundSource::removeAllDependants() {
		while (destinations.size() > 0){
			destinations.back()->setSource(nullptr);
		}
	}

	bool SoundSource::isRealTime() const noexcept {
		return is_realtime;
	}

}