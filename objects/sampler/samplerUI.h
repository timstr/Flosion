#pragma once

#include "ObjectFactory.h"
#include "sampler.h"

// TODO: editor for parameter name / range
// TODO: make horizontal size depend on sampler length and vertical size depend on frequency range
// TODO: make xscale and yscale dynamic

namespace fui {

	struct SamplerObject : Object {
		SamplerObject(){
			
		}

		// TODO

	private:

		// TODO: make non-static, allow scaling of time and frequency axes
		const static float pixels_per_second;
		const static float pixels_per_note;

		static float freqFromNote(float note){
			return 16.3516f * exp2(note / 12.0f);
		}
		static float NoteFromYPos(float pos, float ysize){
			return (ysize - pos) / pixels_per_note;
		}
		static float freqFromYPos(float pos, float ysize){
			return freqFromNote(NoteFromYPos(pos, ysize));
		}
		static float noteFromFreq(float freq){
			return log2(freq / 16.3516f) * 12.0f;
		}
		static float yPosFromNote(float note, float ysize){
			return ysize - (note * pixels_per_note);
		}
		static float yPosFromFreq(float freq, float ysize){
			return yPosFromNote(noteFromFreq(freq), ysize);
		}
		static float timeFromXPos(float pos){
			return pos / pixels_per_second;
		}

		int next_param_id;
	};
	const float SamplerObject::pixels_per_second = 100.0;
	const float SamplerObject::pixels_per_note = 10.0;
	RegisterFactoryObject(SamplerObject, "sampler");
}