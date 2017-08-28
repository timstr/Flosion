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

			Point p(nullptr, pos, 0.0f);

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
			~Point(){
				if (onDestroy){
					onDestroy();
				}
			}

			void setOnDestroy(const std::function<void()>& _onDestroy){
				onDestroy = _onDestroy;
			}

			float getX() const {
				return x;
			}
			float getY() const {
				return y;
			}

			// TODO: this function can break the ordering of the Points set
			// fix up
			void setX(float _x){
				_x = std::min(std::max(spline->min_x, _x), spline->max_x);

				bool erased;

				do {
					// find the first point greater than this one
					erased = false;
					auto next = spline->points.upper_bound(this);
					if (next != spline->points.end()){
						// if the new X-value is past the next node
						if (_x >= (*next)->x){
							// erase the next node
							Point* p =  *next;
							spline->points.erase(next);
							delete p;
							erased = true;
						}
					}
				} while (erased);

				do {
					// find the last point less than this one
					erased = false;
					auto it = spline->points.find(this);
					if (it != spline->points.begin()){
						auto prev = std::prev(it);
						// if the X-value is before the previous node
						if (_x <= (*prev)->x){
							// erase the previous node
							Point* p = *prev;
							spline->points.erase(prev);
							delete p;
							erased = true;
						}
					}
				} while (erased);
				x = _x;
			}
			void setY(float _y){
				y = std::min(std::max(spline->min_y, _y), spline->max_y);
			}

			private:
			Point(Spline* _spline, float _x, float _y){
				spline = _spline;
				x = _x;
				y = _y;
			}
			Spline* spline;
			float x;
			float y;
			std::function<void()> onDestroy;
			friend struct Spline;
		};

		Point* addPoint(float x, float y){
			Point* point = new Point(this, x, y);
			auto it = points.find(point);
			if (it != points.end()){
				Point* p = *it;
				points.erase(it);
				delete p;
			}
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

		void clear(){
			while (points.size() > 0){
				Point* p = *points.begin();
				points.erase(points.begin());
				delete p;
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