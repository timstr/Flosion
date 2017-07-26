#pragma once

#ifndef MUSICAL_CHUNK_SIZE
#define MUSICAL_CHUNK_SIZE 1024
#endif

#ifndef MUSICAL_SAMPLE_FREQ
#define MUSICAL_SAMPLE_FREQ 44100
#endif

#include "Sample.h"
#include "Stateful.h"
#include "SoundSource.h"
#include "SoundInput.h"
#include "SoundResult.h"
#include "NumberSource.h"
#include "NumberInput.h"
#include "NumberResult.h"
#include "StateNumberSource.h"

namespace musical {
	const int CHUNK_SIZE = MUSICAL_CHUNK_SIZE;
	const int SFREQ = MUSICAL_SAMPLE_FREQ;
}