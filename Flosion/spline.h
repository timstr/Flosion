#pragma once

#include "NumberSource.h"
#include "NumberInput.h"
#include <set>

namespace musical {

	struct Spline : PureFunction {
		Spline() : input(this){

		}
		~Spline(){
			for (Point* p : points){
				delete p;
			}
		}

		float evaluate(State* state) const override {
			return getValueAt(input.getValue(state));
		};

		float getValueAt(float pos) const {
			if (points.size() == 0){
				return (min_y + max_y) * 0.5f;
			}

			Point p(this, pos, 0.0f);

			auto it = points.upper_bound(&p);

			auto right = it;

			if (right == points.end()){
				auto left = --it;
				return (*left)->y;
			} else {
				if (right == points.begin()){
					return (*right)->y;
				} else {
					auto left = --it;
					if ((*left)->x == (*right)->x){
						return (*left)->y;
					} else {
						return interpolate((*left)->y, (*right)->y, (pos - (*left)->x) / ((*right)->x - (*left)->x), interp);
					}
				}
			}
		}

		NumberInput input;

		struct Point {
			float getX() const {
				return x;
			}
			float getY() const {
				return y;
			}

			void setX(float _x){
				x = std::min(std::max(spline->min_x, _x), spline->max_x);
			}
			void setY(float _y){
				y = std::min(std::max(spline->min_y, _y), spline->max_y);
			}

			private:
			Point(const Spline* _spline, float _x, float _y) : spline(_spline) {
				setX(_x);
				setY(_y);
			}
			const Spline* spline;
			float x;
			float y;
			friend struct Spline;
		};

		Point* addPoint(float x, float y){
			Point* point = new Point(this, x, y);
			points.insert(point);
			return point;
		}
		void removePoint(Point* point){
			for (auto it = points.begin(); it != points.end(); it++){
				if (*it == point){
					delete point;
					points.erase(it);
					return;
				}
			}
		}

		float getMinX() const {
			return min_x;
		}
		float getMaxX() const {
			return max_x;
		}
		float getMinY() const {
			return min_y;
		}
		float getMaxY() const {
			return max_y;
		}

		void setMinX(float x){
			min_x = x;
		}
		void setMaxX(float x){
			max_x = x;
		}
		void setMinY(float y){
			min_y = y;
		}
		void setMaxY(float y){
			max_y = y;
		}

		enum class Interpolation {
			None,
			Linear,
			Sinusoid
		};

		void setInterpolation(Interpolation method){
			interp = method;
		}

		private:

		Interpolation interp = Interpolation::Linear;

		float min_x = 0;
		float max_x = 1;
		float min_y = -1;
		float max_y = 1;

		static float interpolate(float l, float r, float x, Interpolation method){
			switch (method){
				case Interpolation::None:
					return l;
				case Interpolation::Linear:
					return l + (x * (r - l));
				case Interpolation::Sinusoid:
					return l + ((0.5f - 0.5f * cos(x * 3.1415927f)) * (r - l));
				default:
					return l;
			}
		}

		struct Comparator {
			bool operator()(const Point* l, const Point* r) const {
				return l->x < r->x;
			}
		};

		std::set<Point*, Comparator> points;
	};

}