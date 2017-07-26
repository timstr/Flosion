#pragma once

// TODO: switch from double to float

namespace musical {

	struct Sample {
		double l, r;

		Sample();
		Sample(double _l, double _r);

		Sample& operator+=(const Sample& s);
		Sample& operator-=(const Sample& s);
		Sample& operator*=(double v);
		Sample& operator/=(double v);
	};

	Sample operator+(const Sample& a, const Sample& b);
	Sample operator-(const Sample& a, const Sample& b);
	Sample operator-(const Sample& s);
	Sample operator*(const Sample& s, double v);
	Sample operator*(double v, const Sample& s);
	Sample operator/(const Sample& s, double v);
}