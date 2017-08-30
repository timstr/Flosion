#pragma once

#include "FlosionUI.h"
#include "sampler.h"

namespace fui {

	struct SamplerObject : ProcessingObject {
		SamplerObject(){
			size = {800, 400};
			addSoundInput(new SoundInput(&sampler.input, this, {-5, 5}));
			addSoundOutput(new SoundOutput(&sampler, this, {size.x - 25, 5}));
			addNumberOutput(new NumberOutput(&sampler.input.frequency, this, "Note Frequency", {30, -5}));
			addNumberOutput(new NumberOutput(&sampler.input.notetime, this, "Note Time", {65, -5}));
			addNumberOutput(new NumberOutput(&sampler.input.noteprogress, this, "Note Progress", {100, -5}));
			addChildWindow(new ui::Text("Sampler", fui::getFont()), {30, 30});
			addChildWindow(addparambtn = new AddParamBtn(this));
			addparambtn->pos = {135, -5};
		}

		void onLeftClick(int clicks) override {
			sf::Vector2f mousepos = localMousePos();
			float freq = freqFromNote(floor(NoteFromYPos(mousepos.y, size.y)));
			float time = timeFromXPos(mousepos.x);
			NoteWindow* notewin = new NoteWindow(this, sampler.addNote(freq, 0.1, time, 1.0));
			addChildWindow(notewin);
			notewin->beginDragging();
		}

		void addParameter(){
			// TODO
		}

		void render(sf::RenderWindow& rw) override {
			sf::RectangleShape rect;
			rect.setSize(size);
			rect.setFillColor(sf::Color(0x80000080));
			rect.setOutlineColor(sf::Color(0xFF));
			rect.setOutlineThickness(1.0f);
			rw.draw(rect);
			rect.setOutlineThickness(0.0f);
			rect.setFillColor(sf::Color(0x40));
			rect.setSize(sf::Vector2f(size.x, pixels_per_note));
			for (float y = size.y - 2 * pixels_per_note; y >= 0; y -= 2 * pixels_per_note){
				rect.setPosition(sf::Vector2f(0.0f, y));
				rw.draw(rect);
			}
			renderChildWindows(rw);
		}

		const static double pixels_per_second;
		const static double pixels_per_note;

		private:

		static double freqFromNote(float note){
			return 16.3516 * exp2(note / 12.0);
		}
		static double NoteFromYPos(float pos, float ysize){
			return (ysize - pos) / pixels_per_note;
		}
		static double freqFromYPos(float pos, float ysize){
			return freqFromNote(NoteFromYPos(pos, ysize));
		}
		static double noteFromFreq(float freq){
			return log2(freq / 16.3516) * 12.0;
		}
		static double yPosFromNote(float note, float ysize){
			return ysize - (note * pixels_per_note);
		}
		static double yPosFromFreq(float freq, float ysize){
			return yPosFromNote(noteFromFreq(freq), ysize);
		}
		static double timeFromXPos(float pos){
			return pos / pixels_per_second;
		}

		struct NoteWindow : ui::Window {
			NoteWindow(SamplerObject* _parent, musical::Sampler::Note* _note){
				parent = _parent;
				note = _note;
				note->frequency.useSpline();
				note->frequency.getSpline().clear();
				pos = sf::Vector2f(note->start_time * pixels_per_second, 0.0f);
				size = sf::Vector2f(note->length * 100, parent->size.y);
				addChildWindow(new FreqSplineBtn(this, note->frequency.getSpline().addPoint(0.0, note->frequency.getConstant().getValue())));
				addChildWindow(lengthbtn = new LengthBtn(this));
				addChildWindow(grabber = new Grabber(this));
				redraw();
			}
			~NoteWindow(){
				while (splinebuttons.size() > 0){
					splinebuttons.back()->close();
				}
			}
			bool hit(sf::Vector2f vec) override {
				if (Window::hit(vec)){
					float progress = vec.x / size.x;
					double y = yPosFromFreq(note->frequency.getValue(progress), size.y);
					return (vec.y >= y && vec.y < y + pixels_per_note);
				}
				return false;
			}
			void onRightClick(int clicks) override {
				if (clicks == 1){
					sf::Vector2f mousepos = localMousePos();
					float progress = mousepos.x / size.x;
					float freq = freqFromNote(floor(NoteFromYPos(mousepos.y, size.y)));
					FreqSplineBtn* btn = new FreqSplineBtn(this, note->frequency.getSpline().addPoint(progress, freq));
					addChildWindow(btn);
					btn->startDrag();
				}
			}
			void onLeftClick(int clicks) override {
				if (clicks == 1){
					beginDragging();
				} else if (clicks == 2){
					parent->sampler.removeNote(note);
					close();
				}
			}
			void updateNote(){
				note->start_time = timeFromXPos(pos.x);
				note->length = size.x / 100.0;
			}
			void redraw(){
				int width = note->length * pixels_per_second;
				musical::Spline& spline = note->frequency.getSpline();
				size.x = width;
				vertices.resize(width * 2 + 2);
				for (int x = 0; x < width + 1; x++){
					double y = yPosFromFreq(spline.getValueAt(x / (double)width), size.y);
					vertices[2 * x].color = sf::Color(0xFFFFFF80);
					vertices[2 * x].position = sf::Vector2f(x, y);
					vertices[2 * x + 1].color = sf::Color(0xFFFFFF80);
					vertices[2 * x + 1].position = sf::Vector2f(x, y + pixels_per_note);
				}
				lengthbtn->pos = vertices[width * 2].position;
				for (FreqSplineBtn* btn : splinebuttons){
					btn->updateFromPoint();
				}
			}
			void render(sf::RenderWindow& rw) override {
				rw.draw(vertices.data(), vertices.size(), sf::PrimitiveType::TrianglesStrip);
				renderChildWindows(rw);
			}

			struct LengthBtn : ui::Window {
				LengthBtn(NoteWindow* _notewin){
					notewin = _notewin;
					size = sf::Vector2f(pixels_per_note, pixels_per_note);
					pos = {notewin->size.x, 0};
				};
				void onLeftClick(int clicks) override {
					startDrag();
				}
				void onDrag() override {
					pos.y = notewin->vertices[notewin->vertices.size() - 2].position.y;
					pos.x = std::min(std::max(0.0f, pos.x), notewin->parent->size.x - notewin->pos.x);
					notewin->size.x = pos.x;
					notewin->updateNote();
					notewin->redraw();
				}
				void render(sf::RenderWindow& rw) override {
					sf::RectangleShape rect;
					rect.setSize(size);
					rect.setFillColor(sf::Color(0x808080FF));
					rw.draw(rect);
				}
				NoteWindow* notewin;
			}* lengthbtn;

			struct FreqSplineBtn : ui::Window {
				FreqSplineBtn(NoteWindow* _notewin, musical::Spline::Point* _point){
					notewin = _notewin;
					point = _point;
					point->setOnDestroy([this]{
						notewin->redraw();
						close();
					});
					size = sf::Vector2f(pixels_per_note, pixels_per_note);
					updateFromPoint();
					notewin->splinebuttons.push_back(this);
				}
				~FreqSplineBtn(){
					for (auto it = notewin->splinebuttons.begin(); it != notewin->splinebuttons.end(); it++){
						if (*it == this){
							notewin->splinebuttons.erase(it);
							return;
						}
					}
					throw std::runtime_error("The spline button could not be found");
				}
				void updatePoint(){
					point->setX((pos.x + size.x * 0.5f) / notewin->size.x);
					point->setY(freqFromYPos(pos.y, notewin->size.y));
				}
				void updateFromPoint(){
					pos.x = point->getX() * notewin->size.x - size.x * 0.5f;
					pos.y = yPosFromFreq(point->getY(), notewin->size.y);
				}
				void onRightClick(int clicks) override {
					if (clicks == 1){
						startDrag();
					} else if (clicks == 2){
						point->toggleInterpolationMethod();
						notewin->redraw();
					}
				}
				void onLeftClick(int clicks) override {
					if (clicks == 1){
						notewin->beginDragging();
					} else if (clicks == 2){
						NoteWindow* nw = notewin;
						notewin->note->frequency.getSpline().removePoint(point);
						nw->redraw();
					}
				}
				void onDrag() override {
					pos.y = yPosFromNote(floor(NoteFromYPos(pos.y, notewin->size.y)), notewin->size.y);
					pos.x = std::min(std::max(pos.x + size.x * 0.5f, 0.0f), notewin->size.x) - size.x * 0.5f;
					updatePoint();
					notewin->redraw();
				}
				void render(sf::RenderWindow& rw) override {
					sf::CircleShape circle = sf::CircleShape(size.x / 2);
					circle.setFillColor(sf::Color(0xFF));
					circle.setOutlineColor(sf::Color(0x808080FF));
					circle.setOutlineThickness(1.0f);
					rw.draw(circle);
				}
				musical::Spline::Point* point;
				NoteWindow* notewin;
				float old_y;
			};
			std::vector<FreqSplineBtn*> splinebuttons;


			struct Grabber : ui::Window {
				Grabber(NoteWindow* _notewin){
					notewin = _notewin;
					size = {0, 0};
					pos = {0, 0};
				}
				void onDrag() override {
					pos.y = floorf(pos.y / pixels_per_note) * pixels_per_note;
					for (FreqSplineBtn* btn : notewin->splinebuttons){
						btn->pos.y = btn->old_y + pos.y;
						btn->updatePoint();
					}
					notewin->pos.x = std::min(std::max(0.0f, pos.x), notewin->parent->size.x - notewin->size.x);
					notewin->pos.y = 0.0f;
					notewin->updateNote();
					notewin->redraw();
				}
				NoteWindow* notewin;
				void render(sf::RenderWindow& rw) override {

				}
			}* grabber;
			void beginDragging(){
				for (FreqSplineBtn* btn : splinebuttons){
					btn->old_y = btn->pos.y;
				}
				grabber->pos = pos;
				grabber->startDrag();
			}

			SamplerObject* parent;
			musical::Sampler::Note* note;
			std::vector<sf::Vertex> vertices;
			bool spline_mode;
		};

		struct AddParamBtn : ui::Window {
			AddParamBtn(SamplerObject* _sampler){
				sampler = _sampler;
				size = {30, 30};
			}
			void onLeftClick(int clicks) override {
				//TODO
			}
			void render(sf::RenderWindow& rw) override {
				sf::RectangleShape rect;
				rect.setSize(size);
				rect.setFillColor(sf::Color(0x0080FF80));
				rect.setOutlineColor(sf::Color(0xFF));
				rect.setOutlineThickness(1.0f);
				rw.draw(rect);
				sf::Vertex vertices[] = {
					sf::Vertex({5, 15}, sf::Color(0xFF)),
					sf::Vertex({25, 15}, sf::Color(0xFF)),
					sf::Vertex({15, 5}, sf::Color(0xFF)),
					sf::Vertex({15, 25}, sf::Color(0xFF))
				};
				rw.draw(vertices, 4, sf::PrimitiveType::Lines);
			}
			SamplerObject* sampler;
		}* addparambtn;

		musical::Sampler sampler;
	};
	const double SamplerObject::pixels_per_second = 100.0;
	const double SamplerObject::pixels_per_note = 10.0;
	fuiRegisterObject(SamplerObject, "sampler");
}