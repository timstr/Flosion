#pragma once

#include "ObjectFactory.h"
#include "distortion.h"

namespace fui {

	struct DistortionObject : Object {
		DistortionObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, distortion.gain, "Gain");
			add<NumberInput>(self, distortion.hardness, "Hardness");
			add<SoundInput>(self, distortion.input, "Sound Input");
			add<SoundOutput>(self, distortion, "Sound Output");

			write("Distortion", getFont());
		}

		private:
		flo::Distortion distortion;
	};
	RegisterFactoryObject(DistortionObject, "distortion");
}