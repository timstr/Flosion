#pragma once

#include "FlosionUI.h"
#include "wavegen.h"

namespace fui {

	struct WaveGenObject : Object {
		WaveGenObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, wavegen.frequency, "Frequency");
			add<NumberInput>(self, wavegen.wavefunction, "Wave Function");
			add<NumberOutput>(self, wavegen.phase, "Phase");
			add<SoundOutput>(self, wavegen, "Sound Output");
			write("Wave Generator", getFont());
		}

		private:
		musical::WaveGen wavegen;
	};
	RegisterFactoryObject(WaveGenObject, "wave generator");
};