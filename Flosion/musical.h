#pragma once

#ifndef MUSICAL_CHUNK_SIZE
#define MUSICAL_CHUNK_SIZE 1024
#endif

#ifndef MUSICAL_SAMPLE_FREQ
#define MUSICAL_SAMPLE_FREQ 44100
#endif

// TODO: add facility for precomputed functions
// i.e. for fft window, bell curve, to-amplitude function, sin, cos, log, etc

namespace musical {
	const unsigned int CHUNK_SIZE = MUSICAL_CHUNK_SIZE;
	const unsigned int SFREQ = MUSICAL_SAMPLE_FREQ;
}