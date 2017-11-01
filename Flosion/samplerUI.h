#pragma once

#include "FlosionUI.h"
#include "sampler.h"

// TODO: spline interface for note parameters
// TODO: editor for parameter name / range

namespace fui {

	struct SamplerObject : ProcessingObject {
		SamplerObject(){
			size = {400, 400};
			auto si = new SoundInput(&sampler.input, this);
			auto so = new SoundOutput(&sampler, this);
			notefrequency = new NumberOutput(&sampler.input.frequency, this, "Note Frequency");
			notetime = new NumberOutput(&sampler.input.notetime, this, "Note Time");
			noteprogress = new NumberOutput(&sampler.input.noteprogress, this, "Note Progress");

			addSoundInput(si, leftOf(this), insideTop(this, 30));
			addSoundOutput(so, rightOf(this), insideTop(this, 30));
			addNumberOutput(notefrequency, insideLeft(this, 30), above(this));
			addNumberOutput(notetime, rightOf(notefrequency, 5), above(this));
			addNumberOutput(noteprogress, rightOf(notetime, 5), above(this));
			addChildWindow(new ui::Text("Sampler", fui::getFont()), {30, 30});
			addChildWindow(addparambtn = new AddParamBtn(this), rightOf(noteprogress, 5.0), above(this));
			addChildWindow(notecontainer = new NoteContainer(this));
			next_param_id = 1;
		}

		void render(sf::RenderWindow& rw) override {
			sf::RectangleShape rect;
			rect.setSize(size);
			rect.setFillColor(sf::Color(0x80000080));
			rect.setOutlineColor(sf::Color(0xFF));
			rect.setOutlineThickness(1.0f);
			rw.draw(rect);
			renderChildWindows(rw);
		}

		void onLoseFocus() override {
			hideParameters();
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

		struct ParameterData {
			ParameterData(int _id, const std::string& _name, sf::Color _display_color, musical::NumberSource* _numbersource) : id(_id) {
				name = _name;
				display_color = _display_color;
				numbersource = _numbersource;
				min_value = 0.0f;
				max_value = 1.0f;
				visible = true;
			}
			const int id;
			std::string name;
			sf::Color display_color;
			musical::NumberSource* numbersource;
			float max_value;
			float min_value;
			mutable bool visible;
		};

		friend bool operator<(const ParameterData& l, const ParameterData& r){
			return l.id < r.id;
		}

		struct NoteWindow;

		struct NoteContainer : ui::Window {
			NoteContainer(SamplerObject* _parent){
				sampler_object = _parent;
				size = sampler_object->size;
				clipping = true;
				addChildWindow(container = new Container(this));
				addChildWindow(xscrollbtn = new XScrollBtn(this));
				addChildWindow(yscrollbtn = new YScrollBtn(this));
				addChildWindow(resizebtn = new ResizeBtn(this));
			}

			void addNote(NoteWindow* notewin){
				container->addNote(notewin);
			}
			void removeNote(NoteWindow* notewin){
				container->removeNote(notewin);
			}

			void resize(float width, float height){
				size.x = width;
				size.y = height;

				size.x = std::min(std::max(100.0f, size.x), container->size.x);
				size.y = std::min(std::max(100.0f, size.y), container->size.y);

				if (container->pos.x < size.x - container->size.x){
					container->pos.x = size.x - container->size.x;
				}
				if (container->pos.y < size.y - container->size.y){
					container->pos.y = size.y - container->size.y;
				}

				xscrollbtn->update();
				yscrollbtn->update();
				resizebtn->update();

				sampler_object->size = size;
				sampler_object->alignChildren();
			}

			void render(sf::RenderWindow& rw){
				renderChildWindows(rw);
			}

			const std::vector<NoteWindow*>& getNotes() const {
				return container->notewindows;
			}

			struct Container : ui::Window {
				Container(NoteContainer* _parent){
					parent = _parent;
					size = {800, 800};
					pos = sf::Vector2f(0.0f, parent->size.y - size.y);
				}

				void onLeftClick(int clicks) override {
					if (keyDown(sf::Keyboard::LShift) || keyDown(sf::Keyboard::RShift)){
						startDrag();
					} else {
						parent->sampler_object->hideParameters();
						sf::Vector2f mousepos = localMousePos();
						float freq = freqFromNote(ceil(NoteFromYPos(mousepos.y, size.y)));
						float time = timeFromXPos(mousepos.x);
						NoteWindow* notewin = parent->sampler_object->addNote(freq, time, 0.5f);
						addChildWindow(notewin);
						notewin->beginDragging();
					}
				}
				void onRightClick(int clicks) override {
					parent->sampler_object->startDrag();
				}

				void onDrag() override {
					pos.x = std::min(std::max(pos.x, parent->sampler_object->size.x - size.x), 0.0f);
					pos.y = std::min(std::max(pos.y, parent->sampler_object->size.y - size.y), 0.0f);
					parent->xscrollbtn->update();
					parent->yscrollbtn->update();
				}

				void addNote(NoteWindow* notewin){
					notewindows.push_back(notewin);
				}
				void removeNote(NoteWindow* notewin){
					for (auto it = notewindows.begin(); it != notewindows.end(); it++){
						if (*it == notewin){
							notewindows.erase(it);
							return;
						}
					}
				}

				void render(sf::RenderWindow& rw) override {
					sf::RectangleShape rect;
					rect.setSize(size);
					rect.setOutlineColor(sf::Color(0xFF));
					rect.setOutlineThickness(0.5f);
					rect.setSize(sf::Vector2f(size.x, pixels_per_note));
					int count = 0;
					for (float y = size.y - pixels_per_note; y >= 0; y -= pixels_per_note){
						int note = count % 12;
						if ((note < 4 && note % 2 == 1) || (note > 5 && note % 2 == 0)){
							rect.setFillColor(sf::Color(0x40));
						} else {
							rect.setFillColor(sf::Color(0xFFFFFF40));
						}
						rect.setPosition(sf::Vector2f(0.0f, y));
						rw.draw(rect);
						count++;
					}
					sf::Vertex vertices[2] = {
						sf::Vertex(sf::Vector2f(0.0f, 0.0f), sf::Color(0x80)),
						sf::Vertex(sf::Vector2f(0.0f, size.y), sf::Color(0x80)),
					};
					for (float x = pixels_per_second; x <= size.x; x += pixels_per_second){
						vertices[0].position.x = x;
						vertices[1].position.x = x;
						rw.draw(vertices, 2, sf::PrimitiveType::Lines);
					}
					vertices[0].color = sf::Color(0x40);
					vertices[1].color = sf::Color(0x40);
					for (float x = pixels_per_second * 0.5f; x <= size.x; x += pixels_per_second){
						vertices[0].position.x = x;
						vertices[1].position.x = x;
						rw.draw(vertices, 2, sf::PrimitiveType::Lines);
					}
					renderChildWindows(rw);
				}

				NoteContainer* parent;
				std::vector<NoteWindow*> notewindows;
			}* container;

			struct XScrollBtn : ui::Window {
				XScrollBtn(NoteContainer* _container)
					: container(_container) {
					size.y = 10;
					pos.y = container->size.y - size.y - 5;
					update();
				}

				void update(){
					size.x = container->size.x * container->size.x / container->container->size.x;
					float xmin = container->size.x - container->container->size.x;
					float xmax = 0;
					disabled = xmin >= xmax;
					if (disabled){
						pos.x = 0;
					} else {
						pos.x = (container->size.x - size.x) * (1 - (container->container->pos.x - xmin) / (xmax - xmin));
					}
					pos.y = container->size.y - size.y - 5;
				}

				void onLeftClick(int clicks) override {
					startDrag();
				}

				void onDrag() override {
					pos.x = std::min(std::max(pos.x, 0.0f), container->size.x - size.x);
					pos.y = container->size.y - size.y - 5;

					float xmin = container->size.x - container->container->size.x;
					float xmax = 0;

					container->container->pos.x = (1 - (pos.x / (container->size.x - size.x))) * (xmax - xmin) + xmin;
				}

				void render(sf::RenderWindow& rw) override {
					if (!disabled){
						sf::RectangleShape rect;
						rect.setSize(size);
						rect.setFillColor(sf::Color(0x80));
						rw.draw(rect);
					}
				}

				private:
				NoteContainer* const container;
			}* xscrollbtn;

			struct YScrollBtn : ui::Window {
				YScrollBtn(NoteContainer* _container)
					: container(_container) {
					size.x = 10;
					pos.x = 5;
					update();
				}

				void update(){
					pos.x = 5;
					size.y = container->size.y * container->size.y / container->container->size.y;
					float ymin = container->size.y - container->container->size.y;
					float ymax = 0;
					disabled = ymin >= ymax;
					if (disabled){
						pos.y = 0;
					} else {
						pos.y = (container->size.y - size.y) * (1 - (container->container->pos.y - ymin) / (ymax - ymin));
					}
				}

				void onLeftClick(int clicks) override {
					startDrag();
				}

				void onDrag() override {
					pos.y = std::min(std::max(pos.y, 0.0f), container->size.y - size.y);
					pos.x = 5;

					float xmin = container->size.y - container->container->size.y;
					float xmax = 0;

					container->container->pos.y = (1 - (pos.y / (container->size.y - size.y))) * (xmax - xmin) + xmin;
				}

				void render(sf::RenderWindow& rw) override {
					if (!disabled){
						sf::RectangleShape rect;
						rect.setSize(size);
						rect.setFillColor(sf::Color(0x80));
						rw.draw(rect);
					}
				}

				private:
				NoteContainer* const container;
			}* yscrollbtn;

			struct ResizeBtn : ui::Window {
				ResizeBtn(NoteContainer* _container) : container(_container) {
					size = {20, 20};
					update();
				}

				void update(){
					pos.x = container->size.x - size.x;
					pos.y = container->size.y - size.y;
				}

				void onLeftClick(int clicks) override {
					startDrag();
				}

				void onDrag() override {
					pos.x = std::min(std::max(100.0f - size.x, pos.x), container->container->size.x - size.x);
					pos.y = std::min(std::max(100.0f - size.y, pos.y), container->container->size.y - size.y);

					container->resize(pos.x + size.x, pos.y + size.y);
				}

				private:
				NoteContainer* const container;
			}* resizebtn;

			private:

			SamplerObject* sampler_object;

		}* notecontainer;

		struct NoteWindow : ui::Window {
			NoteWindow(SamplerObject* _parent, musical::Sampler::Note* _note){
				bring_to_front = true;
				sampler_object = _parent;
				container = sampler_object->notecontainer->container;
				note = _note;
				note->frequency.useSpline();
				note->frequency.getSpline().clear();
				pos = sf::Vector2f(note->start_time * pixels_per_second, 0.0f);
				size = sf::Vector2f(note->length * 100, container->size.y);
				addChildWindow(new FreqSplineBtn(this, note->frequency.getSpline().addPoint(0.0, note->frequency.getConstant().getValue())));
				addChildWindow(lengthbtn = new LengthBtn(this));
				addChildWindow(grabber = new Grabber(this));
				sampler_object->notecontainer->addNote(this);
				redraw();
			}
			~NoteWindow(){
				while (splinebuttons.size() > 0){
					splinebuttons.back()->close();
				}
				sampler_object->notecontainer->removeNote(this);
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
				sampler_object->hideParameters();
				if (clicks == 1){
					sf::Vector2f mousepos = localMousePos();
					float progress = mousepos.x / size.x;
					float freq = freqFromNote(ceil(NoteFromYPos(mousepos.y, size.y)));
					FreqSplineBtn* btn = new FreqSplineBtn(this, note->frequency.getSpline().addPoint(progress, freq));
					addChildWindow(btn);
					btn->startDrag();
				}
			}
			void onLeftClick(int clicks) override {
				sampler_object->hideParameters();
				if (keyDown(sf::Keyboard::LShift) || keyDown(sf::Keyboard::RShift)){
					container->startDrag();
				} else if (clicks == 1){
					beginDragging();
				} else if (clicks == 2){
					sampler_object->sampler.removeNote(note);
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
					sf::Color color = sf::Color(0xFF);
					vertices[2 * x].color = color;
					vertices[2 * x].position = sf::Vector2f(x, y);
					vertices[2 * x + 1].color = color;
					vertices[2 * x + 1].position = sf::Vector2f(x, y + pixels_per_note);
				}
				lengthbtn->pos = vertices[width * 2].position;
				for (FreqSplineBtn* btn : splinebuttons){
					btn->updateFromPoint();
				}
				for (ParamWindow* pw : param_windows){
					pw->update();
					redrawParameterGraph(pw);
				}
			}
			
			void render(sf::RenderWindow& rw) override {
				rw.draw(vertices.data(), vertices.size(), sf::PrimitiveType::TrianglesStrip);
				for (auto& graph : param_graphs){
					rw.draw(graph.second.data(), graph.second.size(), sf::PrimitiveType::LineStrip);
				}
				renderChildWindows(rw);
			}

			void addParameter(const ParameterData& parameter_data){
				ParamWindow* paramwin = new ParamWindow(this, note->getParameter(parameter_data.id), parameter_data);
				addChildWindow(paramwin);
				paramwin->visible = false;
				redrawParameterGraph(paramwin);
			}

			void showParameter(int id){
				for (ParamWindow* pw : param_windows){
					if (pw->paramdata.id == id){
						pw->visible = true;
						pw->update();
						pw->redrawSpline();
						pw->bringToFront();
					} else {
						pw->visible = false;
					}
				}
			}
			void hideParameters(){
				for (ParamWindow* pw : param_windows){
					pw->visible = false;
				}
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
					pos.x = std::min(std::max(0.0f, pos.x), notewin->container->size.x - notewin->pos.x);
					notewin->size.x = pos.x;
					notewin->updateNote();
					notewin->redraw();
				}

				void onFocus() override {
					notewin->sampler_object->hideParameters();
				}
				
				void render(sf::RenderWindow& rw) override {
					sf::RectangleShape rect;
					rect.setSize(size);
					rect.setFillColor(sf::Color(0x808080FF));
					rw.draw(rect);
				}
				
				private:
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
					prev_pos = pos;
					startDrag();
				}
				void onRightRelease() override {
					if (pos == prev_pos){
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
					pos.y = yPosFromNote(ceil(NoteFromYPos(pos.y, notewin->size.y)), notewin->size.y);
					pos.x = std::min(std::max(pos.x + size.x * 0.5f, 0.0f), notewin->size.x) - size.x * 0.5f;
					updatePoint();
					notewin->redraw();
				}
				
				void onFocus() override {
					notewin->sampler_object->hideParameters();
				}

				void render(sf::RenderWindow& rw) override {
					sf::CircleShape circle = sf::CircleShape(size.x / 2);
					circle.setFillColor(sf::Color(0xFF));
					circle.setOutlineColor(sf::Color(0x808080FF));
					circle.setOutlineThickness(1.0f);
					rw.draw(circle);
				}

				float old_y;

				private:
				musical::Spline::Point* point;
				NoteWindow* notewin;
				sf::Vector2f prev_pos;
			};

			struct ParamWindow : ui::Window {
				ParamWindow(NoteWindow* _notewin, musical::Sampler::Note::Parameter& _parameter, const ParameterData& _parameter_data) : parameter(_parameter), paramdata(_parameter_data) {
					notewin = _notewin;
					spline_mode = false;
					addChildWindow(dragger = new ConstDragger(this));
					parameter.useConstant();
					parameter.getConstant().setValue((paramdata.min_value + paramdata.max_value) * 0.5f);
					notewin->param_windows.push_back(this);
					update();
					dragger->pos.y = size.y * 0.5f;
				}
				~ParamWindow(){
					for (auto it = notewin->param_windows.begin(); it != notewin->param_windows.end(); it++){
						if (*it == this){
							notewin->param_windows.erase(it);
							return;
						}
					}
				}

				void switchMode(){
					spline_mode = !spline_mode;
					if (spline_mode){
						parameter.useSpline();
						redrawSpline();
						for (SplineButton* sb : spline_buttons){
							sb->visible = true;
						}
						dragger->visible = false;
					} else {
						parameter.useConstant();
						for (SplineButton* sb : spline_buttons){
							sb->visible = false;
						}
						dragger->visible = true;
					}
				}

				void update(){
					size.x = notewin->size.x;
					size.y = 50;
					pos.x = 0;

					float minfreq = notewin->note->frequency.getSpline().getMinimum();
					float maxfreq = notewin->note->frequency.getSpline().getMaximum();
					float miny = yPosFromFreq(minfreq, notewin->container->size.y);
					float maxy = yPosFromFreq(maxfreq, notewin->container->size.y);

					pos.y = ((maxy + miny) * 0.5f) - size.y * 0.5f;

					for (SplineButton* btn : spline_buttons){
						btn->update();
					}
				}

				void redrawSpline(){
					size.x = notewin->size.x;
					int width = size.x;
					vertices.resize(width * 2 + 2);
					float minY = paramdata.min_value;
					float maxY = paramdata.max_value;
					for (int i = 0; i <= width; i++){
						double y = size.y * (1 - (parameter.getSpline().getValueAt(i / (double)width) - minY) / (maxY - minY));
						vertices[2 * i].position = sf::Vector2f(i, y);
						vertices[2 * i].color = paramdata.display_color;
						vertices[2 * i + 1].position = sf::Vector2f(i, size.y);
						vertices[2 * i + 1].color = sf::Color(paramdata.display_color.r / 2, paramdata.display_color.g / 2, paramdata.display_color.b / 2, 0xFF);
					}
				}

				void onLeftClick(int clicks) override {
					if (clicks == 2){
						switchMode();
					}
				}

				void onRightClick(int clicks) override {
					if (spline_mode){
						sf::Vector2f mousepos = localMousePos();
						float time = mousepos.x / size.x;
						float maxY = paramdata.max_value;
						float minY = paramdata.min_value;
						float val = minY + (maxY - minY) * (1.0 - (mousepos.y / size.y));
						auto point = parameter.getSpline().addPoint(time, val);
						SplineButton* btn = new SplineButton(this, point);
						addChildWindow(btn, mousepos - btn->size * 0.5f);
						btn->startDrag();
					} else {
						dragger->startDrag();
					}
				}

				void render(sf::RenderWindow& rw) override {
					sf::RectangleShape rect;
					rect.setSize(size);
					rect.setOutlineColor(sf::Color(0xFF));
					rect.setOutlineThickness(1.0f);
					rect.setFillColor(sf::Color(paramdata.display_color.r * 0.25, paramdata.display_color.g * 0.25, paramdata.display_color.b * 0.25, 0xC0));
					rw.draw(rect);
					if (spline_mode){
						rw.draw(vertices.data(), vertices.size(), sf::PrimitiveType::TrianglesStrip);
					} else {
						float amt = (parameter.getConstant().getValue() - paramdata.min_value) / (paramdata.max_value - paramdata.min_value);
						/*sf::Vertex vdata[] = {
							sf::Vertex(sf::Vector2f(0.0f, size.y * (1.0f - amt)), paramdata.display_color),
							sf::Vertex(sf::Vector2f(0.0f, size.y), sf::Color(paramdata.display_color.r / 2, paramdata.display_color.g / 2, paramdata.display_color.b / 2, 0xFF)),
							sf::Vertex(sf::Vector2f(size.x, size.y * (1.0f - amt)), paramdata.display_color),
							sf::Vertex(sf::Vector2f(size.x, size.y), sf::Color(paramdata.display_color.r / 2, paramdata.display_color.g / 2, paramdata.display_color.b / 2, 0xFF)),
						};
						rw.draw(vdata, 4, sf::PrimitiveType::TriangleStrip);*/
						rect.setOutlineThickness(0.0f);
						rect.setFillColor(paramdata.display_color);
						rect.setSize(sf::Vector2f(size.x, size.y * amt));
						rect.setPosition(sf::Vector2f(0.0f, size.y * (1.0f - amt)));
						rw.draw(rect);
					}
					renderChildWindows(rw);
				}

				struct SplineButton : ui::Window {
					SplineButton(ParamWindow* _paramwindow, musical::Spline::Point* _point) : point(_point) {
						paramwindow = _paramwindow;
						paramwindow->spline_buttons.push_back(this);
						size = {10.0f, 10.0f};
						point->setOnDestroy([this]{
							paramwindow->redrawSpline();
							close();
						});
					}
					~SplineButton(){
						for (auto it = paramwindow->spline_buttons.begin(); it != paramwindow->spline_buttons.end(); it++){
							if (*it == this){
								paramwindow->spline_buttons.erase(it);
								return;
							}
						}
					}

					void update(){
						pos.x = point->getX() * paramwindow->size.x;
					}

					void render(sf::RenderWindow& rw) override {
						sf::CircleShape circle(size.x * 0.5);
						circle.setFillColor(paramwindow->paramdata.display_color);
						circle.setOutlineColor(sf::Color(0xFF));
						circle.setOutlineThickness(1.0f);
						rw.draw(circle);
					}

					void onRightClick(int clicks) override {
						startDrag();
						old_pos = pos;
					}
					void onRightRelease() override {
						if (old_pos == pos){
							point->toggleInterpolationMethod();
							paramwindow->redrawSpline();
							paramwindow->notewin->redrawParameterGraph(paramwindow);
						}
					}

					void onLeftClick(int clicks) override {
						if (clicks == 2){
							ParamWindow* pw = paramwindow;
							pw->parameter.getSpline().removePoint(point);
							pw->redrawSpline();
							pw->notewin->redrawParameterGraph(pw);
						}
					}

					void onDrag() override {
						pos.x = std::min(std::max(0.0f, pos.x + size.x * 0.5f), paramwindow->size.x) - size.x * 0.5f;
						pos.y = std::min(std::max(0.0f, pos.y + size.y * 0.5f), paramwindow->size.y) - size.y * 0.5f;

						float time = (pos.x + size.x * 0.5f) / paramwindow->size.x;
						float maxY = paramwindow->paramdata.max_value;
						float minY = paramwindow->paramdata.min_value;
						float val = minY + (maxY - minY) * (1.0 - ((pos.y + size.y * 0.5f) / paramwindow->size.y));
						point->setX(time);
						point->setY(val);
						paramwindow->redrawSpline();
						paramwindow->notewin->redrawParameterGraph(paramwindow);
					}

					private:
					ParamWindow* paramwindow;
					musical::Spline::Point* const point;
					sf::Vector2f old_pos;
				};

				struct ConstDragger : ui::Window {
					ConstDragger(ParamWindow* _paramwindow){
						paramwindow = _paramwindow;
						size = {0, 0};
					}

					void onRightClick(int clicks) override {
						startDrag();
					}
					void onDrag() override {
						pos.x = 0;
						pos.y = std::min(std::max(pos.y, 0.0f), paramwindow->size.y);
						float minv = paramwindow->paramdata.min_value;
						float maxv = paramwindow->paramdata.max_value;
						double val = minv + (maxv - minv) * (1.0f - (pos.y / paramwindow->size.y));
						paramwindow->parameter.getConstant().setValue(val);
						paramwindow->update();
						paramwindow->notewin->redrawParameterGraph(paramwindow);
					}

					private:
					ParamWindow* paramwindow;
				};

				const ParameterData& paramdata;
				musical::Sampler::Note::Parameter& parameter;

				private:
				bool spline_mode;
				std::vector<sf::Vertex> vertices;
				std::vector<SplineButton*> spline_buttons;
				ConstDragger* dragger;
				NoteWindow* notewin;
				sf::Vector2f old_pos;
			};

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
					notewin->pos.x = std::min(std::max(0.0f, pos.x), notewin->container->size.x - notewin->size.x);
					notewin->pos.y = 0.0f;
					notewin->updateNote();
					notewin->redraw();
				}
				
				void render(sf::RenderWindow& rw) override {

				}
				
				private:

				NoteWindow* notewin;
			}* grabber;

			void beginDragging(){
				for (FreqSplineBtn* btn : splinebuttons){
					btn->old_y = btn->pos.y;
				}
				grabber->pos = pos;
				grabber->startDrag();
			}

			void redrawParameterGraph(ParamWindow* pw){
				int width = size.x;
				std::vector<sf::Vertex>& vertices = param_graphs[pw];
				vertices.resize(width);
				float minv = pw->paramdata.min_value;
				float maxv = pw->paramdata.max_value;
				for (int x = 0; x < width; x++){
					float ymin = yPosFromFreq(note->frequency.getValue(x / (float)width), size.y);
					float val = (pw->parameter.getValue(x / (float)width) - minv) / (maxv - minv);
					vertices[x].position = sf::Vector2f(x, ymin + (1.0f - val) * pixels_per_note);
					vertices[x].color = pw->paramdata.display_color;
				}
			}

			SamplerObject* sampler_object;
			NoteContainer::Container* container;
			musical::Sampler::Note* note;
			std::vector<sf::Vertex> vertices;
			std::vector<FreqSplineBtn*> splinebuttons;
			std::vector<ParamWindow*> param_windows;
			std::map<ParamWindow*, std::vector<sf::Vertex>> param_graphs;
			bool spline_mode;
		};

		NoteWindow* addNote(float freq, float time, float length){
			NoteWindow* notewin = new NoteWindow(this, sampler.addNote(freq, 0.1, time, length));
			for (const ParameterData& pd : paramdata){
				notewin->addParameter(pd);
			}
			return notewin;
		}

		void addParameter(const std::string& _caption = "", sf::Color _color = sf::Color(0x00FF00FF)){
			int id = next_param_id++;
			musical::NumberSource* ns = sampler.addParameter(id);
			std::string caption = _caption == "" ? "Parameter " + std::to_string(id) : _caption;
			auto it = paramdata.insert(ParameterData(id, caption, _color, ns));
			addChildWindow(new ParamHandle(this, *(it.first)),
						   rightOf(param_handles.empty() ? (ui::Window*)noteprogress : (ui::Window*)param_handles.back(), 5.0),
						   insideTop(this, -30));
			addparambtn->setXAlign(rightOf(param_handles.back(), 5.0));
			for (NoteWindow* nw : notecontainer->getNotes()){
				nw->addParameter(*(it.first));
			}
			showParameter(id);
		}

		void showParameter(int id){
			for (NoteWindow* nw : notecontainer->getNotes()){
				nw->showParameter(id);
			}
			for (const ParameterData& pd : paramdata){
				pd.visible = pd.id == id;
			}
			for (ParamHandle* ph : param_handles){
				ph->size.y = ph->getParamId() == id ? 60 : 45;
			}
		}
		void hideParameters(){
			for (NoteWindow* nw : notecontainer->getNotes()){
				nw->hideParameters();
			}
			for (const ParameterData& pd : paramdata){
				pd.visible = false;
			}
			for (ParamHandle* ph : param_handles){
				ph->size.y = 45;
			}
		}

		// button for adding a new parameter
		struct AddParamBtn : ui::Window {
			AddParamBtn(SamplerObject* _sampler){
				sampler = _sampler;
				size = {30, 30};
			}
			void onLeftClick(int clicks) override {
				sampler->addParameter("", sf::Color(rand() & 0xFF, rand() & 0xFF, rand() & 0xFF, 0xFF));
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

		// holds a NumberOutput and label for a user-defined note parameter
		struct ParamHandle : ui::Window {
			ParamHandle(SamplerObject* _parent, const ParameterData& _param_data) : param_data(_param_data){
				parent = _parent;
				size = {30, 60};
				addChildWindow(numberoutput = new NumberOutput(param_data.numbersource, parent, param_data.name));
				parent->param_handles.push_back(this);
			}
			~ParamHandle(){
				for (auto it = parent->param_handles.begin(); it != parent->param_handles.end(); it++){
					if (*it == this){
						parent->param_handles.erase(it);
						return;
					}
				}
			}

			void onLeftClick(int clicks) override {
				param_data.visible = !param_data.visible;
				if (param_data.visible){
					parent->showParameter(param_data.id);
				} else {
					parent->hideParameters();
				}
				
			}

			void render(sf::RenderWindow& rw) override {
				sf::RectangleShape rect;
				rect.setSize(size);
				if (param_data.visible){
					rect.setFillColor(param_data.display_color);
				} else {
					rect.setFillColor(sf::Color(param_data.display_color.r * 0.5, param_data.display_color.g * 0.5, param_data.display_color.b * 0.5, 255));
				}
				rect.setOutlineColor(sf::Color(0xFF));
				rect.setOutlineThickness(1.0f);
				rw.draw(rect);
				renderChildWindows(rw);
			}

			void onHover() override {
				numberoutput->onHover();
			}

			int getParamId() const {
				return param_data.id;
			}

			private:
			SamplerObject* parent;
			const ParameterData& param_data;
			NumberOutput* numberoutput;
		};

		std::set<ParameterData> paramdata;
		NumberOutput* notefrequency;
		NumberOutput* notetime;
		NumberOutput* noteprogress;
		std::vector<ParamHandle*> param_handles;
		musical::Sampler sampler;
		int next_param_id;
	};
	const double SamplerObject::pixels_per_second = 100.0;
	const double SamplerObject::pixels_per_note = 10.0;
	fuiRegisterObject(SamplerObject, "sampler");
}