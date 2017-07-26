#pragma once

#include "Sample.h"

namespace musical {

	Sample::Sample(){
		l = 0;
		r = 0;
	}

	Sample::Sample(double _l, double _r){
		l = _l;
		r = _r;
	}


	Sample& Sample::operator+=(const Sample& s){
		return *this = Sample(l + s.l, r + s.r);
	}
	Sample& Sample::operator-=(const Sample& s){
		return *this = Sample(l + s.l, r + s.r);
	}
	Sample& Sample::operator*=(double v){
		return *this = Sample(l * v, r * v);
	}
	Sample& Sample::operator/=(double v){
		return *this = Sample(l / v, r / v);
	}


	Sample operator+(const Sample& a, const Sample& b){
		return Sample(a.l + b.l, a.r + b.r);
	}
	Sample operator-(const Sample& a, const Sample& b){
		return Sample(a.l - b.l, a.r - b.r);
	}
	Sample operator-(const Sample& s){
		return Sample(-s.l, -s.r);
	}
	Sample operator*(const Sample& s, double v){
		return Sample(s.l * v, s.r * v);
	}
	Sample operator*(double v, const Sample& s){
		return Sample(s.l * v, s.r * v);
	}
	Sample operator/(const Sample& s, double v){
		return Sample(s.l / v, s.r / v);
	}
}