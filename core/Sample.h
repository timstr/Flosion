#pragma once

#include <array>
#include <stdexcept>
//#include <cassert>
#define assert(x) do { if (!static_cast<bool>(x)) { throw std::runtime_error("Assertion failure!"); } } while (false);

#define NOEXCEPT_IF_I_SAY_SO

namespace flo {

	constexpr std::size_t CHUNK_SIZE = 1 << 10;
	constexpr std::size_t SFREQ = 44100;

	struct Sample {
		float l, r;

		constexpr Sample() NOEXCEPT_IF_I_SAY_SO :
			l(0.0f), r(0.0f) {

		}
		constexpr Sample(float _l, float _r) NOEXCEPT_IF_I_SAY_SO :
			l(_l), r(_r) {
		}
	};

	constexpr Sample& operator+=(Sample& l, const Sample& r) NOEXCEPT_IF_I_SAY_SO {
		l.l += r.l;
		l.r += r.r;
		return l;
	}
	constexpr Sample& operator-=(Sample& l, const Sample& r) NOEXCEPT_IF_I_SAY_SO {
		l.l -= r.l;
		l.r -= r.r;
		return l;
	}
	constexpr Sample& operator*=(Sample& l, float r) NOEXCEPT_IF_I_SAY_SO {
		l.l *= r;
		l.r *= r;
		return l;
	}
	constexpr Sample& operator/=(Sample& l, float r) NOEXCEPT_IF_I_SAY_SO {
		l.l /= r;
		l.r /= r;
		return l;
	}

	constexpr Sample operator+(const Sample& l, const Sample& r) NOEXCEPT_IF_I_SAY_SO {
		return Sample(l.l + r.l, l.r + r.r);
	}
	constexpr Sample operator-(const Sample& l, const Sample& r) NOEXCEPT_IF_I_SAY_SO {
		return Sample(l.l - r.l, l.r - r.r);
	}
	constexpr Sample operator-(const Sample& s) NOEXCEPT_IF_I_SAY_SO {
		return Sample(-s.l, -s.r);
	}
	constexpr Sample operator*(const Sample& l, float r) NOEXCEPT_IF_I_SAY_SO {
		return Sample(l.l * r, l.r * r);
	}
	constexpr Sample operator*(float l, const Sample& r) NOEXCEPT_IF_I_SAY_SO {
		return Sample(l * r.l, l * r.r);
	}
	constexpr Sample operator/(const Sample& l, float r) NOEXCEPT_IF_I_SAY_SO {
		return Sample(l.l / r, l.r / r);
	}

	struct SoundChunk {
		constexpr SoundChunk() NOEXCEPT_IF_I_SAY_SO  {
			silence();
		}

		constexpr Sample& operator[](size_t i) NOEXCEPT_IF_I_SAY_SO {
			assert(i < size());
			return data[i];
		}

		constexpr const Sample& operator[](size_t i) const NOEXCEPT_IF_I_SAY_SO {
			assert(i < size());
			return data[i];
		}

		constexpr void silence() NOEXCEPT_IF_I_SAY_SO {
			for (Sample& s : data){
				s = Sample(0.0f, 0.0f);
			}
		}

		constexpr std::size_t size() const NOEXCEPT_IF_I_SAY_SO {
			return CHUNK_SIZE;
		}

	private:

		Sample data[CHUNK_SIZE];
	};
}