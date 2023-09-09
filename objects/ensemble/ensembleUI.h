#pragma once

#include "ObjectFactory.h"
#include "ensemble.h"

namespace fui {

	struct EnsembleObject : Object {
		EnsembleObject(){
			auto self = this->thisAs<Object>();
			add<NumberInput>(self, ensemble.num_voices, "Number of Voices");
			add<NumberInput>(self, ensemble.frequency, "Frequency");
			add<NumberInput>(self, ensemble.frequency_spread, "Frequency Spread");
			add<SoundInput>(self, ensemble.input, "Sound Input");
			add<SoundOutput>(self, ensemble, "Sound Input");
			add<NumberOutput>(self, ensemble.input.frequency, "Frequency");
			write("Ensemble", getFont());
		}

		private:
		flo::Ensemble ensemble;
	};
	RegisterFactoryObject(EnsembleObject, "ensemble");
};