#pragma once

#include "NumberSource.h"
#include <algorithm>
#include <functional>

// TODO: save/read waveform to/from file

namespace musical {

	struct WaveTable : PureFunction {
		WaveTable() : input(this) {
			min_y = -1;
			max_y = 1;
			size = max_size;
			for (unsigned int i = 0; i < max_size; i++){
				data[i] = 0;
			}
			interpolate = true;
		}

		float evaluate(const State* state) const noexcept override {
			return getValueAt(input.getValue(state));
		}

		inline float getValueAt(float x) const {
			x -= floor(x);
			x *= size - 1;
			if (interpolate){
				int prev = (int)floor(x);
				int next = (int)ceil(x);
				float diff = x - floor(x);
				return data[prev] * (1.0f - diff) + data[next] * diff;
			} else {
				return data[(int)floor(x)];
			}
		}

		// resamples the existing waveform to the new size
		void setSize(size_t _size){
			size_t newsize = std::min(max_size, _size);
			std::vector<float> temp;
			temp.resize(newsize);
			for (unsigned int i = 0; i < size; i++){
				temp[i] = getValueAt((float)i / (size - 1));
			}
			size = newsize;
			for (unsigned int i = 0; i < newsize; i++){
				data[i] = temp[i];
			}
		}

		size_t getSize() const {
			return size;
		}
		
		void setInterpolation(bool use){
			interpolate = use;
		}
		bool getInterpolation() const {
			return interpolate;
		}

		// fill waveform using provided [0,1] based function
		void fill(const std::function<float(float)>& fn){
			for (int i = 0; i < size; i++){
				data[i] = std::min(std::max(fn((float)i / (size - 1)), min_y), max_y);
			}
		}
		void fillLine(float x0, float x1, float y0, float y1){
			x0 = std::min(std::max(x0, 0.0f), 1.0f);
			x1 = std::min(std::max(x1, 0.0f), 1.0f);
			y0 = std::min(std::max(y0, min_y), max_y);
			y1 = std::min(std::max(y1, min_y), max_y);
			if (x1 < x0){
				float temp = x1;
				x1 = x0;
				x0 = temp;
				temp = y1;
				y1 = y0;
				y0 = temp;
			}
			int begin = (int)floor(x0 * (size - 1));
			int end = (int)ceil(x1 * (size - 1));
			if (end == begin){
				return;
			}
			for (int i = begin; i <= end; i++){
				float t = (i - begin) / (float)(end - begin);
				data[i] = y0 * (1.0f - t) + y1 * t;
			}
		}
		void setPoint(float x, float y){
			x -= floor(x);
			x *= (size - 1);
			int prev = (int)floor(x);
			int next = (int)ceil(x);
			y = std::min(std::max(y, min_y), max_y);
			data[prev] = y;
			data[next] = y;
		}

		void setRange(float miny, float maxy, bool reposition = true){
			if (reposition){
				for (int i = 0; i < size; i++){
					float progress = (data[i] - min_y) / (max_y - min_y);
					data[i] = progress * (maxy - miny) + miny;
				}
			} else {
				for (int i = 0; i < size; i++){
					data[i] = std::min(std::max(data[i], miny), maxy);
				}
			}
			min_y = miny;
			max_y = maxy;
		}
		float getMinY() const {
			return min_y;
		}
		float getMaxY() const {
			return max_y;
		}

		NumberInput input;

		private:
		bool interpolate;
		const static size_t max_size = 256;
		size_t size;
		float data[max_size];
		float min_y;
		float max_y;
	};

}