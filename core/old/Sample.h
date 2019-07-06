#pragma once

#include <array>
#include <stdexcept>
#include <cassert>

namespace flo {

	constexpr std::size_t CHUNK_SIZE = 1 << 10;
	constexpr std::size_t SFREQ = 44100;

	struct Sample {
		float l, r;

		constexpr Sample() noexcept :
			l(0.0f), r(0.0f) {

		}
		constexpr Sample(float _l, float _r) noexcept :
			l(_l), r(_r) {
		}
	};

	constexpr Sample& operator+=(Sample& l, const Sample& r) noexcept {
		l.l += r.l;
		l.r += r.r;
		return l;
	}
	constexpr Sample& operator-=(Sample& l, const Sample& r) noexcept {
		l.l -= r.l;
		l.r -= r.r;
		return l;
	}
	constexpr Sample& operator*=(Sample& l, float r) noexcept {
		l.l *= r;
		l.r *= r;
		return l;
	}
	constexpr Sample& operator/=(Sample& l, float r) noexcept {
		l.l /= r;
		l.r /= r;
		return l;
	}

	constexpr Sample operator+(const Sample& l, const Sample& r) noexcept {
		return Sample(l.l + r.l, l.r + r.r);
	}
	constexpr Sample operator-(const Sample& l, const Sample& r) noexcept {
		return Sample(l.l - r.l, l.r - r.r);
	}
	constexpr Sample operator-(const Sample& s) noexcept {
		return Sample(-s.l, -s.r);
	}
	constexpr Sample operator*(const Sample& l, float r) noexcept {
		return Sample(l.l * r, l.r * r);
	}
	constexpr Sample operator*(float l, const Sample& r) noexcept {
		return Sample(l * r.l, l * r.r);
	}
	constexpr Sample operator/(const Sample& l, float r) noexcept {
		return Sample(l.l / r, l.r / r);
	}

	struct SoundChunk {
		constexpr SoundChunk() noexcept  {
			silence();
		}

		constexpr Sample& operator[](size_t i) noexcept {
			assert(i < size());
			return data[i];
		}

		constexpr const Sample& operator[](size_t i) const noexcept {
			assert(i < size());
			return data[i];
		}

		constexpr void silence() noexcept {
			for (Sample& s : data){
				s = Sample(0.0f, 0.0f);
			}
		}

		constexpr std::size_t size() const noexcept {
			return CHUNK_SIZE;
		}

	private:

		Sample data[CHUNK_SIZE];
	};
}