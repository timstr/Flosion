// The code here is taken from the following source under the GNU Free Documentation License:
// https://rosettacode.org/wiki/Fast_Fourier_transform

#pragma once

#include <complex>

typedef std::complex<float> complex;

void fft(complex* x, unsigned int size){
	unsigned int N = size;
	unsigned int k = N;
	float thetaT = 3.141592654 / N;
	complex phiT = complex(cos(thetaT), -sin(thetaT));

	while (k > 1){
		unsigned int n = k;
		k >>= 1;
		phiT = phiT * phiT;
		complex T = 1.0f;
		for (unsigned int l = 0; l < k; l++){
			for (unsigned int a = l; a < N; a += n){
				unsigned int b = a + k;
				complex t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * T;
			}
			T *= phiT;
		}
	}

	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++){
		unsigned int b = a;

		b = ((b & 0xAAAAAAAA) >> 1) | ((b & 0x55555555) << 1);
		b = ((b & 0xCCCCCCCC) >> 2) | ((b & 0x33333333) << 2);
		b = ((b & 0xF0F0F0F0) >> 4) | ((b & 0x0F0F0F0F) << 4);
		b = ((b & 0xFF00FF00) >> 8) | ((b & 0x00FF00FF) << 8);
		b = ((b >> 16) | (b << 16)) >> (32 - m);

		if (b > a){
			complex t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}
}

void ifft(complex* x, unsigned int size){
	for (unsigned int i = 0; i < size; i++){
		x[i] = std::conj(x[i]);
	}

	fft(x, size);

	for (unsigned int i = 0; i < size; i++){
		x[i] = std::conj(x[i]) / (float)size;
	}
}