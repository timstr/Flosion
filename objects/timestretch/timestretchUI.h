#pragma once

#include "ObjectFactory.h"
#include "timestretch.h"

namespace fui {

	struct TimeStretchObject : Object {
		TimeStretchObject(){
			auto self = thisAs<Object>();
			add<NumberInput>(self, timestretch.speed, "Speed");
			add<SoundInput>(self, timestretch.input, "Sound Input");
			add<SoundOutput>(self, timestretch, "Sound Output");
			write("TimeStretch", getFont());
		}

		private:

		flo::TimeStretch timestretch;
	};
	RegisterFactoryObject(TimeStretchObject, "timestretch", "stretch");
}