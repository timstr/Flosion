#pragma once

#include "ObjectFactory.h"
#include "wavegen.h"

namespace fui {

	struct WaveGenObject : Object {
		WaveGenObject(){
			auto self = this->thisAs<Object>();
			add<NumberInput>(self, wavegen.frequency, "Frequency");
			add<NumberInput>(self, wavegen.wavefunction, "Wave Function");
			add<NumberOutput>(self, wavegen.phase, "Phase");
			add<SoundOutput>(self, wavegen, "Sound Output");
			write("Wave Generator", getFont());
		}

		private:
		flo::WaveGen wavegen;
	};
	RegisterFactoryObject(WaveGenObject, "wave generator");
};