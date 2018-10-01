#pragma once

#include "FlosionUI.h"
#include "filter.h"

namespace fui {

	struct FilterObject : Object {
		FilterObject(){
			auto self = thisAs<Object>();
			add<SoundInput>(self, filter.input, "Sound Input");
			add<SoundOutput>(self, filter, "SoundOutput");

			add<NumberOutput>(self, filter.frequency_out, "Frequency Out");
			add<NumberInput>(self, filter.amplitude_in, "Amplitude In");

			write("Filter", getFont());
		}

		private:

		flo::Filter filter;
	};
	RegisterFactoryObject(FilterObject, "filter", "eq");

}