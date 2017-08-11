#pragma once

#include "spline.h"
#include "FlosionUI.h"

namespace fui {

	struct SplineObject : ProcessingObject {
		SplineObject(){
			size = {200, 100};
			addNumberInput(new NumberInput(&spline.input, this, "Input", {-25, 35}));
			addNumberOutput(new NumberOutput(&spline, this, {195, 35}));
			calculateRenderPoints();
		}

		void render(sf::RenderWindow& rw) override {
			sf::RectangleShape rect;
			rect.setSize(size);
			rect.setFillColor(sf::Color(0xFFFF0000));
			rect.setOutlineColor(sf::Color(0xFF));
			rect.setOutlineThickness(1.0);
			rw.draw(rect);
			rw.draw(renderpoints.data(), renderpoints.size(), sf::PrimitiveType::LinesStrip);
			renderChildWindows(rw);
		}

		void onLeftClick(int clicks) override {
			sf::Vector2f mousepos = localMousePos();
			sf::Vector2f pos = toSplinePoint(mousepos);
			musical::Spline::Point* point = spline.addPoint(pos.x, pos.y);
			PointHandle* handle = new PointHandle(this, point);
			addChildWindow(handle);
			handle->pos = mousepos - handle->size * 0.5f;
			handle->startDrag();
		}

		struct PointHandle : ui::Window {
			PointHandle(SplineObject* _parent, musical::Spline::Point* _point){
				parent = _parent;
				point = _point;
				size = {10, 10};
			}

			void render(sf::RenderWindow& rw) override {
				sf::CircleShape circle(5, 15);
				circle.setFillColor(sf::Color(0xFF));
				circle.setOutlineColor(sf::Color(0xFFFFFFFF));
				circle.setOutlineThickness(1.0);
				rw.draw(circle);
			}

			void onLeftClick(int clicks) override {
				if (clicks == 1){
					startDrag();
				} else if (clicks == 2){
					parent->spline.removePoint(point);
					parent->calculateRenderPoints();
					close();
				}
			}
			void onDrag() override {
				sf::Vector2f p = parent->toSplinePoint(pos + (size * 0.5f));
				point->setX(p.x);
				point->setY(p.y);
				p.x = point->getX();
				p.y = point->getY();
				pos = parent->toScreenPoint(p) - (size * 0.5f);
				parent->calculateRenderPoints();
			}

			SplineObject* parent;
			musical::Spline::Point* point;
		};

		sf::Vector2f toScreenPoint(sf::Vector2f spline_point){
			return sf::Vector2f(
				(spline_point.x - spline.getMinX()) / (spline.getMaxX() - spline.getMinX()) * size.x,
				(1 - (spline_point.y - spline.getMinY()) / (spline.getMaxY() - spline.getMinY())) * size.y
			);
		}
		sf::Vector2f toSplinePoint(sf::Vector2f screen_point){
			return sf::Vector2f(
				(screen_point.x / (float)size.x) * (spline.getMaxX() - spline.getMinX()) + spline.getMinX(),
				(1 - (screen_point.y / (float)size.y)) * (spline.getMaxY() - spline.getMinY()) + spline.getMinY()
			);
		}

		private:

		void calculateRenderPoints(){
			int n = size.x;
			renderpoints.resize(n);
			for (int i = 0; i < n; i++){
				renderpoints[i].position.x = i;
				float mag = (spline.getValueAt(i / (float)n) - spline.getMinY()) / (spline.getMaxY() - spline.getMinY());
				renderpoints[i].position.y = size.y * (1 - mag);
				renderpoints[i].color = sf::Color(0xFFFFFFFF);
			}
		}

		musical::Spline spline;
		std::vector<sf::Vertex> renderpoints;
	};
	fuiRegisterObject(SplineObject, "spline");
};