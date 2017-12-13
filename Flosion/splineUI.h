#pragma once

#include "spline.h"
#include "FlosionUI.h"
#include "guihelpers.h"
#include "stringhelpers.h"

namespace fui {

	struct SplineObject : ProcessingObject {
		SplineObject(){
			size = {200, 100};
			NumberInput* input = new NumberInput(&spline.input, this, "Input", [this](NumberOutput* no){
				const musical::NumberSource* source = no->getSource();
				if (source->hasRange()){
					spline.setMinX(source->getMinimum());
					spline.setMaxX(source->getMaximum());
					updateHandles();
				}
			});
			NumberOutput* output = new NumberOutput(&spline, this, "Output", [this](NumberInput* ni){
				const musical::NumberInput* input = ni->getInput();
				if (input->hasRange()){
					spline.setMinY(input->getMinimum());
					spline.setMaxY(input->getMaximum());
					updateHandles();
				}
			});
			addChildWindow(input, leftOf(this), middleOfY(this));
			addChildWindow(output, rightOf(this), middleOfY(this));
			redraw();
		}

		void render(sf::RenderWindow& rw) override {
			sf::RectangleShape rect;
			rect.setSize(size);
			rect.setFillColor(sf::Color(0x404040FF));
			rect.setOutlineColor(sf::Color(0xFF));
			rect.setOutlineThickness(1.0);
			rw.draw(rect);
			rw.draw(renderpoints.data(), renderpoints.size(), sf::PrimitiveType::LineStrip);
			renderChildWindows(rw);
		}

		void onLeftClick(int clicks) override {
			if (keyDown(sf::Keyboard::LShift) || keyDown(sf::Keyboard::RShift)){
				auto menu = new Menu(this);
				addChildWindow(menu, middleOfX(this), middleOfY(this));
				menu->grabFocus();
			} else {
				vec2 mousepos = localMousePos();
				vec2 pos = toSplinePoint(mousepos);
				musical::Spline::Point* point = spline.addPoint(pos.x, pos.y);
				PointHandle* handle = new PointHandle(this, point);
				addChildWindow(handle);
				handle->pos = mousepos - handle->size * 0.5f;
				handle->startDrag();
			}
		}

		void updateHandles(){
			for (PointHandle* ph : pointhandles){
				ph->update();
			}
			redraw();
		}

		struct PointHandle : ui::Window {
			PointHandle(SplineObject* _parent, musical::Spline::Point* _point) : parent(_parent), point(_point) {
				parent->pointhandles.push_back(this);
				point->setOnDestroy([this]{
					parent->redraw();
					close();
				});
				size = {10, 10};
			}
			~PointHandle(){
				for (auto it = parent->pointhandles.begin(); it != parent->pointhandles.end(); it++){
					if (*it == this){
						parent->pointhandles.erase(it);
						return;
					}
				}
			}

			void render(sf::RenderWindow& rw) override {
				sf::CircleShape circle(5, 15);
				circle.setFillColor(sf::Color(0xFF));
				circle.setOutlineColor(sf::Color(0xFFFFFFFF));
				circle.setOutlineThickness(1.0);
				rw.draw(circle);
			}
			void onRightClick(int clicks) override {
				point->toggleInterpolationMethod();
				parent->redraw();
			}
			void onLeftClick(int clicks) override {
				if (clicks == 1){
					startDrag();
				} else if (clicks == 2){
					SplineObject* so = parent;
					parent->spline.removePoint(point);
					so->redraw();
				}
			}
			void onDrag() override {
				vec2 p = parent->toSplinePoint(pos + (size * 0.5f));
				point->setX(p.x);
				point->setY(p.y);
				p.x = point->getX();
				p.y = point->getY();
				pos = parent->toScreenPoint(p) - (size * 0.5f);
				parent->redraw();
			}

			void update(){
				pos = parent->toScreenPoint(vec2(point->getX(), point->getY())) - (size * 0.5f);
			}

			SplineObject* const parent;
			musical::Spline::Point* const point;
		};

		vec2 toScreenPoint(vec2 spline_point){
			return vec2(
				(spline_point.x - spline.getMinX()) / (spline.getMaxX() - spline.getMinX()) * size.x,
				(1 - (spline_point.y - spline.getMinY()) / (spline.getMaxY() - spline.getMinY())) * size.y
			);
		}
		vec2 toSplinePoint(vec2 screen_point){
			return vec2(
				(screen_point.x / (float)size.x) * (spline.getMaxX() - spline.getMinX()) + spline.getMinX(),
				(1 - (screen_point.y / (float)size.y)) * (spline.getMaxY() - spline.getMinY()) + spline.getMinY()
			);
		}

		private:

		musical::Spline spline;

		struct Menu : ui::Window {
			Menu(SplineObject* _splineobject) : splineobject(_splineobject) {
				size = splineobject->size + vec2(50, 50);

				std::function<bool(const std::string&)> validate = [](const std::string& str){
					return !std::isnan(stringToFloat(str));
				};

				auto minyfield = new ui::helpers::TextEntryHelper(
					toString(splineobject->spline.getMinY()),
					getFont(),
					[this](const std::string& str){
						float val = stringToFloat(str);
						if (!std::isnan(val)){
							splineobject->spline.setMinY(val);
							splineobject->updateHandles();
						}
					},
					validate
				);

				auto maxyfield = new ui::helpers::TextEntryHelper(
					toString(splineobject->spline.getMaxY()),
					getFont(),
					[this](const std::string& str){
						float val = stringToFloat(str);
						if (!std::isnan(val)){
							splineobject->spline.setMaxY(val);
							splineobject->updateHandles();
						}
					},
					validate
				);

				auto minxfield = new ui::helpers::TextEntryHelper(
					toString(splineobject->spline.getMinX()),
					getFont(),
					[this](const std::string& str){
						float val = stringToFloat(str);
						if (!std::isnan(val)){
							splineobject->spline.setMinX(val);
							splineobject->updateHandles();
						}
					},
					validate
				);

				auto maxxfield = new ui::helpers::TextEntryHelper(
					toString(splineobject->spline.getMaxX()),
					getFont(),
					[this](const std::string& str){
						float val = stringToFloat(str);
						if (!std::isnan(val)){
							splineobject->spline.setMaxX(val);
							splineobject->updateHandles();
						}
					},
					validate
				);
				

				addChildWindow(minyfield, 100, insideTop(this, 5.0f));
				addChildWindow(new ui::Text("Min Y:", getFont()), leftOf(minyfield, 5.0f), insideTop(this, 5.0f));

				addChildWindow(maxyfield, 100, below(minyfield, 5.0f));
				addChildWindow(new ui::Text("Max Y:", getFont()), leftOf(maxyfield, 5.0f), below(minyfield, 5.0f));

				addChildWindow(minxfield, 100, below(maxyfield, 5.0f));
				addChildWindow(new ui::Text("Min X:", getFont()), leftOf(minxfield, 5.0f), below(maxyfield, 5.0f));

				addChildWindow(maxxfield, 100, below(minxfield, 5.0f));
				addChildWindow(new ui::Text("Max X:", getFont()), leftOf(maxyfield, 5.0f), below(minxfield, 5.0f));
			}

			void onLoseFocus() override {
				close();
			}

			SplineObject* const splineobject;
		};

		void redraw(){
			int n = (int)size.x;
			renderpoints.resize(n);
			for (int i = 0; i < n; i++){
				renderpoints[i].position.x = (float)i;
				float x = (i * (spline.getMaxX() - spline.getMinX()) / (float)n) + spline.getMinX();
				float mag = (spline.getValueAt(x) - spline.getMinY()) / (spline.getMaxY() - spline.getMinY());
				renderpoints[i].position.y = size.y * (1 - mag);
				renderpoints[i].color = sf::Color(0xFFFFFFFF);
			}
		}

		std::vector<sf::Vertex> renderpoints;
		std::vector<PointHandle*> pointhandles;
	};
	fuiRegisterObject(SplineObject, "spline");
};