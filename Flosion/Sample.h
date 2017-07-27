#pragma once

namespace musical {

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
}