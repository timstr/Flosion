#pragma once

#include "NumberSource.h"
#include "NumberInput.h"
#include <set>

namespace musical {

	// TODO: allow resizing with and without scaling points
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
						return interpolate((*left)->y, (*right)->y, (pos - (*left)->x) / ((*right)->x - (*left)->x), (*left)->interp_method);
					}
				}
			}
		}

		NumberInput input;

		enum Interpolation {
			None,
			Linear,
			Sinusoid,
			EaseIn,
			EaseOut,

			Count
		};

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

			void setInterpolationMethod(int interp){
				interp_method = interp;
			}
			int getInterpolationMethod() const {
				return interp_method;
			}
			void toggleInterpolationMethod(){
				interp_method = (interp_method + 1) % Spline::Interpolation::Count;
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
			unsigned int interp_method;
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
			auto it2 = points.insert(point);
			// set the interpolation method to that of the previous point, or linear by default
			if (it2.first != points.begin()){
				auto prev = std::prev(it2.first);
				point->setInterpolationMethod((*prev)->getInterpolationMethod());
			} else {
				point->setInterpolationMethod(Interpolation::Linear);
			}
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

		float getMinimum() const {
			float min = max_y;
			for (Point* p : points){
				if (p->y < min){
					min = p->y;
				}
			}
			return min;
		}
		float getMaximum() const {
			float max = min_y;
			for (Point* p : points){
				if (p->y > max){
					max = p->y;
				}
			}
			return max;
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
			updatePoints();
		}
		void setMaxX(float x){
			max_x = x;
			updatePoints();
		}
		void setMinY(float y){
			min_y = y;
			updatePoints();
		}
		void setMaxY(float y){
			max_y = y;
			updatePoints();
		}

		private:

		void updatePoints(){
			for (Point* p : points){
				p->setX(std::min(std::max(p->x, min_x), max_x));
				p->setY(std::min(std::max(p->y, min_y), max_y));
			}
		}

		float min_x = 0;
		float max_x = 1;
		float min_y = -1;
		float max_y = 1;

		static float interpolate(float l, float r, float x, int method){
			if (l == r){
				return l;
			}
			switch (method){
				case Interpolation::None:
					return l;
				case Interpolation::Linear:
					return l + (x * (r - l));
				case Interpolation::Sinusoid:
					return l + (r - l) * (0.5f - 0.5f * cos(x * 3.1415927f));
				case Interpolation::EaseIn:
					return l + (r - l) * pow(x, 4.0f);
				case Interpolation::EaseOut:
					return l + (r - l) * (1.0f - pow(1.0f - x, 4.0f));
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