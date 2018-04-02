#pragma once

#include <array>

// TODO: add facility for precomputed functions?
// i.e. for fft window, bell curve, to-amplitude function, sin, cos, log, etc

namespace musical {
	const unsigned int CHUNK_SIZE = 1 << 10;
	const unsigned int SFREQ = 44100;

	struct Sample {
		float l, r;

		Sample();
		Sample(float _l, float _r);

		Sample& operator+=(const Sample& s);
		Sample& operator-=(const Sample& s);
		Sample& operator*=(float v);
		Sample& operator/=(float v);
	};

	Sample operator+(const Sample& a, const Sample& b);
	Sample operator-(const Sample& a, const Sample& b);
	Sample operator-(const Sample& s);
	Sample operator*(const Sample& s, float v);
	Sample operator*(float v, const Sample& s);
	Sample operator/(const Sample& s, float v);

	typedef std::array<Sample, CHUNK_SIZE> Buffer;
}