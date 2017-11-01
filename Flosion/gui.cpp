#pragma once

#include "gui.h"
#include <SFML\Graphics.hpp>
#include <map>
#include <set>
#include <functional>


const double PI = 3.14159265358979323846264338327950288;

namespace ui {

	// Transition
	Transition::Transition(Window* _target, double _duration, const std::function<void(double)>& _transitionFn, const std::function<void()>& _onComplete) : target(_target) {
		duration = _duration;
		transitionFn = _transitionFn;
		onComplete = _onComplete;
		timestamp = getProgramTime();
		completed = false;
	}
	void Transition::apply(){
		double progress = (getProgramTime() - timestamp) / duration;
		if (progress > 1){
			transitionFn(1.0);
			if (onComplete){
				onComplete();
			}
			completed = true;
		} else {
			transitionFn(progress);
		}
	}
	bool Transition::complete() const {
		return completed;
	}

	// Context
	void Context::init(sf::Vector2f size, std::string title, double _render_delay){
		width = size.x;
		height = size.y;
		render_delay = _render_delay;
		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;
		Context::getRenderWindow().create(sf::VideoMode(size.x, size.y), title, sf::Style::Default, settings);
		resetView();
		Context::current_window = root();
		Context::clock.restart();
	}
	void Context::addTransition(const Transition& transition){
		transitions.push_back(transition);
	}
	void Context::applyTransitions(){
		for (int i = 0; i < transitions.size();){
			if (transitions[i].complete()){
				transitions.erase(transitions.begin() + i);
			} else {
				transitions[i].apply();
				i++;
			}
		}
	}
	void Context::clearTransitions(Window* target){
		for (auto it = transitions.begin(); it != transitions.end();){
			if (it->target == target){
				transitions.erase(it);
			} else {
				it++;
			}
		}
	}
	void Context::focusTo(Window* window){
		if (Context::current_window == window){
			return;
		}

		if (Context::dragging_window){
			Context::dragging_window = nullptr;
		}
		if (Context::getTextEntry()){
			Context::setTextEntry(nullptr);
		}

		std::vector<Window*> current_path;
		std::vector<Window*> new_path;

		Window* twindow = Context::current_window;
		Context::current_window = window;

		while (twindow != nullptr){
			current_path.push_back(twindow);
			twindow = twindow->parent;
		}

		twindow = window;
		while (twindow != nullptr){
			new_path.push_back(twindow);
			twindow = twindow->parent;
		}

		while ((current_path.size() > 0) && (new_path.size() > 0) && (current_path.back() == new_path.back())){
			current_path.pop_back();
			new_path.pop_back();
		}

		while (current_path.size() > 0){
			current_path.front()->onLoseFocus();
			current_path.erase(current_path.begin());
		}

		while (new_path.size() > 0){
			if (new_path.back()->parent && new_path.back()->bring_to_front){
				new_path.back()->bringToFront();
			}
			new_path.back()->onFocus();
			new_path.pop_back();
		}
	}
	sf::Vector2f Context::getMousePosition(){
		return (sf::Vector2f)sf::Mouse::getPosition(renderwindow);
	}
	void Context::handleMouseDown(sf::Mouse::Button button, sf::Vector2f pos){
		Window* hitwin = root()->findWindowAt(pos);

		if (hitwin == nullptr){
			return;
		}

		if (clock.getElapsedTime().asMilliseconds() - click_timestamp <= doubleclicktime){
			// if the mouse has been clicked recently:
			if (click_button == button){
				// if the mouse was clicked with the same button:
				if (hitwin == click_window){
					// if the click hit the same window again:

					// double click that window
					if (button == sf::Mouse::Left){
						hitwin->onLeftClick(2);
					} else if (button == sf::Mouse::Right){
						hitwin->onRightClick(2);
					}

					// don't let it be double clicked again until after it gets single clicked again
					// achieved by faking an old timestamp
					click_timestamp = clock.getElapsedTime().asMilliseconds() - doubleclicktime;
				} else {
					// if the click hit a different window:

					Context::focusTo(hitwin);
					// single-click the first window, then this one
					if (button == sf::Mouse::Left){
						hitwin->onLeftClick(1);
					} else if (button == sf::Mouse::Right){
						hitwin->onRightClick(1);
					}
					click_timestamp = clock.getElapsedTime().asMilliseconds();
				}
			} else {
				// if the mouse was clicked with a different button:

				Context::focusTo(hitwin);

				// single-click the current window
				if (button == sf::Mouse::Left){
					hitwin->onLeftClick(1);
				} else if (button == sf::Mouse::Right){
					hitwin->onRightClick(1);
				}

				click_timestamp = clock.getElapsedTime().asMilliseconds();
			}
		} else {
			// if the mouse hasn't been clicked recently:

			Context::focusTo(hitwin);
			if (button == sf::Mouse::Left){
				hitwin->onLeftClick(1);
			} else if (button == sf::Mouse::Right){
				hitwin->onRightClick(1);
			}

			click_timestamp = clock.getElapsedTime().asMilliseconds();
		}

		click_button = button;
		click_window = hitwin;
	}
	void Context::addKeyboardCommand(sf::Keyboard::Key trigger_key, void (*handler)()){
		auto pair = std::pair<sf::Keyboard::Key, std::vector<sf::Keyboard::Key>>(trigger_key, {});
		auto it = commands.find(pair);
		if (it == commands.end()){
			commands[pair] = handler;
		} else {
			throw;
		}
	}
	void Context::addKeyboardCommand(sf::Keyboard::Key trigger_key, std::vector<sf::Keyboard::Key> required_keys, void (*handler)()){
		auto pair = std::pair<sf::Keyboard::Key, std::vector<sf::Keyboard::Key>>(trigger_key, required_keys);
		auto it = commands.find(pair);
		if (it == commands.end()){
			commands[pair] = handler;
		} else {
			throw;
		}
	}
	void Context::setQuitHandler(bool (*handler)()){
		quit_handler = handler;
	}
	void Context::handleKeyPress(sf::Keyboard::Key key){
		auto it = commands.begin();
		int max = 0;
		auto current_it = commands.end();
		while (it != commands.end()){
			if (it->first.first == key){
				bool match = true;
				for (int i = 0; i < it->first.second.size() && match; i++){
					match = sf::Keyboard::isKeyPressed(it->first.second[i]);
				}
				if (match && it->first.second.size() >= max){
					max = it->first.second.size();
					current_it = it;
				}
			}
			it++;
		}

		if (current_it != commands.end()){
			current_it->second();
		} else if (current_window){
			current_window->onKeyDown(key);
		}
	}
	void Context::handleMouseUp(sf::Mouse::Button button, sf::Vector2f pos){
		if (Context::dragging_window){
			if (button == sf::Mouse::Left){
				Context::dragging_window->onLeftRelease();
			} else if (button == sf::Mouse::Right){
				Context::dragging_window->onRightRelease();
			}

			Window* hover_window = root()->findWindowAt(pos);
			if (hover_window){
				hover_window->onDropDragWindow(Context::dragging_window);
			}
			Context::dragging_window = nullptr;
		} else if (Context::current_window){
			if (button == sf::Mouse::Left){
				Context::current_window->onLeftRelease();
			} else if (button == sf::Mouse::Right){
				Context::current_window->onRightRelease();
			}
		}
	}
	void Context::handleDrag(){
		if (dragging_window){
			dragging_window->pos = (sf::Vector2f)sf::Mouse::getPosition(Context::getRenderWindow()) - drag_offset;
			dragging_window->onDrag();
		}
	}
	void Context::handleHover(){
		Window* hover_window = root()->findWindowAt((sf::Vector2f)sf::Mouse::getPosition(Context::getRenderWindow()));
		if (hover_window){
			if (dragging_window){
				hover_window->onHoverWithDrag(dragging_window);
			} else {
				hover_window->onHover();
			}
		}
	}
	void Context::handleQuit(bool force){
		if (quit_handler && !force){
			if (quit_handler()){
				quit = true;
			}
		} else {
			quit = true;
		}
	}
	bool Context::hasQuit(){
		return quit;
	}
	long double Context::getProgramTime(){
		return clock.getElapsedTime().asMilliseconds() / 1000.0;
	}
	sf::RenderWindow& Context::getRenderWindow(){
		return renderwindow;
	}
	double Context::getRenderDelay(){
		return render_delay;
	}
	void Context::translateView(sf::Vector2f offset){
		view_offset.x-= offset.x;
		view_offset.y -= offset.y;
	}
	sf::Vector2f Context::getViewOffset(){
		return view_offset;
	}
	void Context::resetView(){
		sf::Vector2f size = getScreenSize();
		clip_rect = sf::FloatRect(0, 0, size.x, size.y);
		view_offset = sf::Vector2f(0, 0);
		updateView();
	}
	const sf::FloatRect& Context::getClipRect(){
		return clip_rect;
	}
	void Context::setClipRect(const sf::FloatRect& rect){
		clip_rect = rect;
	}
	void Context::intersectClipRect(const sf::FloatRect& rect){
		float left = std::max(clip_rect.left, rect.left);
		float top = std::max(clip_rect.top, rect.top);
		float right = std::min(clip_rect.left + clip_rect.width, rect.left + rect.width);
		float bottom = std::min(clip_rect.top + clip_rect.height, rect.top + rect.height);
		clip_rect = sf::FloatRect(left, top, right - left, bottom - top);
	}
	void Context::resize(int w, int h){
		width = w;
		height = h;
	}
	Window* Context::getDraggingWindow(){
		return dragging_window;
	}
	void Context::setDraggingWindow(Window* window, sf::Vector2f offset){
		dragging_window = window;
		drag_offset = offset;
	}
	Window* Context::getCurrentWindow(){
		return current_window;
	}
	TextEntry* Context::getTextEntry(){
		return text_entry;
	}
	void Context::setTextEntry(TextEntry* textentry){
		text_entry = textentry;
	}
	void Context::updateView(){
		sf::Vector2f size = getScreenSize();
		sf::View view;
		sf::FloatRect rect = getClipRect();
		sf::Vector2f offset = getViewOffset() + sf::Vector2f(rect.left, rect.top);
		view.setSize(rect.width, rect.height);
		view.setCenter(offset.x + rect.width * 0.5f, offset.y + rect.height * 0.5f);
		sf::FloatRect vp = sf::FloatRect(
			rect.left / size.x,
			rect.top / size.y,
			rect.width / size.x,
			rect.height / size.y
		);
		view.setViewport(vp);
		renderwindow.setView(view);
	}

	bool Context::quit = false;
	double Context::render_delay = 0.025;
	sf::Vector2f Context::drag_offset = sf::Vector2f();
	sf::RenderWindow Context::renderwindow;
	Window* Context::dragging_window = nullptr;
	Window* Context::current_window = nullptr;
	TextEntry* Context::text_entry = nullptr;
	std::vector<Transition> Context::transitions;
	sf::Clock Context::clock;
	std::map<std::pair<sf::Keyboard::Key, std::vector<sf::Keyboard::Key>>, void (*)()> Context::commands;
	bool (*Context::quit_handler)() = nullptr;
	const int Context::doubleclicktime = 250;
	uint32_t Context::click_timestamp;
	sf::Mouse::Button Context::click_button;
	Window* Context::click_window;
	sf::FloatRect Context::clip_rect;
	sf::Vector2f Context::view_offset;
	int Context::width;
	int Context::height;

	// Window
	Window::~Window(){
		if (parent){
			parent->releaseChildWindow(this);
			if (Context::getCurrentWindow() == this){
				Context::focusTo(parent);
			}
		}
		Context::clearTransitions(this);
		if (Context::getDraggingWindow() == this){
			Context::setDraggingWindow(nullptr);
		}
		clear();
	}
	void Window::close(){
		delete this;
	}
	bool Window::hit(sf::Vector2f testpos){
		return ((testpos.x >= 0.0f) && (testpos.x < size.x) && (testpos.y >= 0.0f) && (testpos.y < size.y));
	}
	sf::Vector2f Window::localMousePos(){
		sf::Vector2f pos = (sf::Vector2f)sf::Mouse::getPosition(Context::getRenderWindow());
		Window *window = this;
		while (window){
			pos -= window->pos;
			window = window->parent;
		}
		return(pos);
	}
	sf::Vector2f Window::absPos(){
		sf::Vector2f pos = sf::Vector2f();
		Window *window = this;
		while (window != nullptr){
			pos += window->pos;
			window = window->parent;
		}
		return(pos);
	}
	void Window::onLeftClick(int clicks){

	}
	void Window::onLeftRelease(){

	}
	void Window::onRightClick(int clicks){

	}
	void Window::onRightRelease(){

	}
	void Window::onScroll(double delta_x, double delta_y){

	}
	void Window::startDrag(){
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right)){
			if (Context::getDraggingWindow() != this){
				Context::focusTo(this);
				Context::setDraggingWindow(this, (sf::Vector2f)sf::Mouse::getPosition(Context::getRenderWindow()) - pos);
			}
		}
	}
	void Window::onDrag(){

	}
	void Window::onHover(){

	}
	void Window::onHoverWithDrag(Window *drag_window){

	}
	void Window::onDropDragWindow(Window *window){

	}
	void Window::onFocus(){

	}
	bool Window::isFocused(){
		return Context::getCurrentWindow() == this;
	}
	void Window::onLoseFocus(){

	}
	void Window::focusToNextWindow(){
		if (parent && isFocused()){
			Window* next = nullptr;
			for (Window* window : parent->childwindows){
				if (window->pos.y > pos.y){
					if (!next || window->pos.y < next->pos.y){
						next = window;
					}
				} else if (window->pos.y == pos.y){
					if (window->pos.x > pos.x){
						if (!next || window->pos.x < next->pos.x){
							next = window;
						}
					}
				}
			}
			if (next == nullptr){
				next = parent->childwindows.front();
				for (Window* window : parent->childwindows){
					if (window->pos.y < next->pos.y){
						next = window;
					} else if (window->pos.y == next->pos.y){
						if (window->pos.x < next->pos.x){
							next = window;
						}
					}
				}
			}
			Context::focusTo(next);
		}
	}
	void Window::focusToPreviousWindow(){
		if (parent && isFocused()){
			Window* prev = nullptr;
			for (Window* window : parent->childwindows){
				if (window->pos.y < pos.y){
					if (!prev || window->pos.y > prev->pos.y){
						prev = window;
					}
				} else if (window->pos.y == pos.y){
					if (window->pos.x < pos.x){
						if (!prev || window->pos.x > prev->pos.x){
							prev = window;
						}
					}
				}
			}
			if (prev == nullptr){
				prev = parent->childwindows.front();
				for (Window* window : parent->childwindows){
					if (window->pos.y > prev->pos.y){
						prev = window;
					} else if (window->pos.y == prev->pos.y){
						if (window->pos.x > prev->pos.x){
							prev = window;
						}
					}
				}
			}
			Context::focusTo(prev);
		}
	}
	void Window::grabFocus(){
		Context::focusTo(this);
	}
	void Window::onKeyDown(sf::Keyboard::Key key){

	}
	void Window::onKeyUp(sf::Keyboard::Key key){

	}
	bool Window::keyDown(sf::Keyboard::Key key){
		if (Context::getCurrentWindow() == this){
			return(sf::Keyboard::isKeyPressed(key));
		} else {
			return(false);
		}
	}
	Window::Alignment::Alignment(Type _type, Window* _relative_to, float _margin){
		type = _type;
		relative_to = _relative_to;
		margin = _margin;
	}
	Window::XAlignment::XAlignment(Type type, Window* relative_to, float margin) : Alignment(type, relative_to, margin) {
		
	}
	Window::YAlignment::YAlignment(Type type, Window* relative_to, float margin) : Alignment(type, relative_to, margin) {

	}
	void Window::setXAlign(XAlignment xalignment){
		xalign = xalignment;
		if (parent){
			parent->children_aligned = false;
		}
	}
	void Window::setYAlign(YAlignment yalignment){
		yalign = yalignment;
		if (parent){
			parent->children_aligned = false;
		}
	}
	void Window::align(){
		if (xalign.type != Alignment::None){
			if (xalign.relative_to){
				float relx = xalign.relative_to == parent ? 0.0f : xalign.relative_to->pos.x;
				switch (xalign.type){
					case Alignment::Before:
						pos.x = relx - size.x - xalign.margin;
						break;
					case Alignment::After:
						pos.x = relx + xalign.relative_to->size.x + xalign.margin;
						break;
					case Alignment::Center:
						pos.x = relx + xalign.relative_to->size.x * 0.5f - size.x * 0.5f;
						break;
					case Alignment::InsideMin:
						pos.x = relx + xalign.margin;
						break;
					case Alignment::InsideMax:
						pos.x = relx + xalign.relative_to->size.x - size.x - xalign.margin;
						break;
				}
			}
		}

		if (yalign.type != Alignment::None){
			if (yalign.relative_to){
				float rely = yalign.relative_to == parent ? 0.0f : yalign.relative_to->pos.y;
				switch (yalign.type){
					case Alignment::Before:
						pos.y = rely - size.y - yalign.margin;
						break;
					case Alignment::After:
						pos.y = rely + yalign.relative_to->size.y + yalign.margin;
						break;
					case Alignment::Center:
						pos.y = rely + yalign.relative_to->size.y * 0.5f - size.y * 0.5f;
						break;
					case Alignment::InsideMin:
						pos.y = rely + yalign.margin;
						break;
					case Alignment::InsideMax:
						pos.y = rely + yalign.relative_to->size.y - size.y - yalign.margin;
						break;
				}
			}
		}
	}
	void Window::alignChildren(){
		std::set<Window*> remaining;
		std::vector<Window*> ordered;

		for (Window* w : childwindows){
			remaining.insert(w);
		}

		while (remaining.size() > 0){
			bool found = false;
			for (auto it = remaining.begin(); !found && it != remaining.end(); it++){
				Window* xwin = (*it)->xalign.relative_to;
				Window* ywin = (*it)->yalign.relative_to;
				if ((!xwin || (remaining.find(xwin) == remaining.end())) && (!ywin || (remaining.find(ywin) == remaining.end()))){
					ordered.push_back(*it);
					remaining.erase(it);
					found = true;
					break;
				}
			}
			if (!found){
				throw std::runtime_error("The relative alignments are cyclical");
			}
		}

		for (Window* w : ordered){
			w->align();
		}
	}
	
	Window::XAlignment Window::noAlignX(){
		return XAlignment(Alignment::None);
	}
	Window::XAlignment Window::leftOf(Window* window, float margin){
		return XAlignment(Alignment::Before, window, margin);
	}
	Window::XAlignment Window::rightOf(Window* window, float margin){
		return XAlignment(Alignment::After, window, margin);
	}
	Window::XAlignment Window::middleOfX(Window* window){
		return XAlignment(Alignment::Center, window, 0.0f);
	}
	Window::XAlignment Window::insideLeft(Window* window, float margin){
		return XAlignment(Alignment::InsideMin, window, margin);
	}
	Window::XAlignment Window::insideRight(Window* window, float margin){
		return XAlignment(Alignment::InsideMax, window, margin);
	}
	Window::YAlignment Window::noAlignY(){
		return YAlignment(Alignment::None);
	}
	Window::YAlignment Window::above(Window* window, float margin){
		return YAlignment(Alignment::Before, window, margin);
	}
	Window::YAlignment Window::below(Window* window, float margin){
		return YAlignment(Alignment::After, window, margin);
	}
	Window::YAlignment Window::middleOfY(Window* window){
		return YAlignment(Alignment::Center, window, 0.0f);
	}
	Window::YAlignment Window::insideTop(Window* window, float margin){
		return YAlignment(Alignment::InsideMin, window, margin);
	}
	Window::YAlignment Window::insideBottom(Window* window, float margin){
		return YAlignment(Alignment::InsideMax, window, margin);
	}
	void Window::addChildWindow(Window *window){
		if (window->parent != nullptr){
			throw;
		}
		window->parent = this;
		childwindows.insert(childwindows.begin(), window);
	}
	void Window::addChildWindow(Window *window, sf::Vector2f pos){
		window->pos = pos;
		addChildWindow(window);
	}
	void Window::addChildWindow(Window *window, XAlignment xalignment){
		addChildWindow(window);
		window->setXAlign(xalignment);
	}
	void Window::addChildWindow(Window *window, YAlignment yalignment){
		addChildWindow(window);
		window->setYAlign(yalignment);
	}
	void Window::addChildWindow(Window *window, XAlignment xalignment, YAlignment yalignment){
		addChildWindow(window);
		window->setXAlign(xalignment);
		window->setYAlign(yalignment);
	}
	void Window::releaseChildWindow(Window* window){
		for (int i = 0; i < childwindows.size(); i++){
			if (childwindows[i] == window){
				childwindows[i]->parent = nullptr;
				childwindows.erase(childwindows.begin() + i);
				return;
			}
		}
		throw;
	}
	void Window::bringToFront(){
		if (!parent){
			return;
		}
		for (int i = 0; i < parent->childwindows.size(); i++){
			if (parent->childwindows[i] == this){
				parent->childwindows.erase(parent->childwindows.begin() + i);
				parent->childwindows.insert(parent->childwindows.begin(), this);
				return;
			}
		}
		throw;
	}
	void Window::clear(){
		while (childwindows.size() > 0){
			childwindows[0]->close();
		}
	}
	Window* Window::findWindowAt(sf::Vector2f _pos){
		if (!visible || disabled){
			return nullptr;
		}

		if (clipping && ((_pos.x < 0.0f) || (_pos.x > size.x) || (_pos.y < 0.0) || (_pos.y > size.y))){
			return false;
		}

		Window* window = nullptr;
		for (int i = 0; i < childwindows.size() && window == nullptr; i++){
			window = childwindows[i]->findWindowAt(_pos - childwindows[i]->pos);
			if (window && (window == Context::getDraggingWindow())){
				window = nullptr;
			}
		}
		if (window){
			return window;
		}

		if (this->hit(_pos)){
			return this;
		}

		return nullptr;
	}
	void Window::render(sf::RenderWindow& renderwindow){
		sf::RectangleShape rectshape;
		rectshape.setSize(size);
		rectshape.setFillColor(sf::Color((((uint32_t)std::hash<Window*>{}(this)) & 0xFFFFFF00) | 0x80));
		rectshape.setOutlineColor(sf::Color(0xFF));
		rectshape.setOutlineThickness(1);
		renderwindow.draw(rectshape);
		renderChildWindows(renderwindow);
	}
	void Window::renderChildWindows(sf::RenderWindow& renderwindow){
		if (!children_aligned){
			alignChildren();
			children_aligned = true;
		}
		for (int i = childwindows.size() - 1; i >= 0; i -= 1){
			if (childwindows[i]->visible){
				if (childwindows[i]->clipping){

					sf::FloatRect rect = Context::getClipRect();
					sf::Vector2f pos = Context::getViewOffset();
					Context::intersectClipRect(sf::FloatRect(-pos, childwindows[i]->size));
					Context::translateView(childwindows[i]->pos);
					Context::updateView();
					childwindows[i]->render(renderwindow);
					Context::translateView(-childwindows[i]->pos);
					Context::setClipRect(rect);
					Context::updateView();
				} else {
					Context::translateView(childwindows[i]->pos);
					Context::updateView();
					childwindows[i]->render(renderwindow);
					Context::translateView(-childwindows[i]->pos);
					Context::updateView();
				}
			}
		}
	}
	void Window::startTransition(double duration, const std::function<void(double)> transitionFn, const std::function<void()>& onComplete){
		Context::addTransition(Transition(this, duration, transitionFn, onComplete));
	}
	const std::vector<Window*>& Window::getChildWindows() const {
		return childwindows;
	}
	Window* Window::getParent() const {
		return parent;
	}


	// Text
	Text::Text(const std::string& _text, const sf::Font& _font, sf::Color color, int charsize){
		text = sf::Text("", _font, charsize);
		text.setFillColor(color);
		setText(_text);
		disabled = true;
	}
	void Text::setText(const std::string& _text){
		text.setString(_text);
		updateSize();
	}
	const std::string& Text::getText(){
		return text.getString();
	}
	void Text::render(sf::RenderWindow& renderwin){
		renderwin.draw(text);
	}
	void Text::updateSize(){
		if (text.getString().isEmpty()){
			size.x = 0;
			size.y = (float)text.getCharacterSize();
		} else {
			size.x = text.getLocalBounds().width;
			size.y = text.getLocalBounds().height;
		}
	}


	// TextEntry
	TextEntry::TextEntry(const sf::Font& font, int charsize){
		text = sf::Text("", font, charsize);
		updateSize();
	}
	TextEntry::TextEntry(const std::string& str, const sf::Font& font, int charsize, sf::Color _text_color, sf::Color _bg_color){
		text = sf::Text(str, font, charsize);
		updateSize();
		setTextColor(_text_color);
		setBackGroundColor(_bg_color);
	}
	void TextEntry::beginTyping(){
		grabFocus();
		Context::setTextEntry(this);
	}
	void TextEntry::moveTo(sf::Vector2f pos){
		for (int i = 0; i < text.getString().getSize(); i++){
			sf::Vector2f charpos = text.findCharacterPos(i);
			if (pos.x < charpos.x){
				cursor_index = i - 1;
				updateSize();
				return;
			}
		}
		cursor_index = text.getString().getSize();
		updateSize();
	}
	void TextEntry::setText(const std::string& str){
		text.setString(str);
	}
	std::string TextEntry::getText() const {
		return text.getString().toAnsiString();
	}
	void TextEntry::clearText(){
		text.setString("");
		cursor_index = 0;
		updateSize();
	}
	void TextEntry::setTextColor(sf::Color color){
		text.setFillColor(color);
	}
	sf::Color TextEntry::getTextColor() const {
		return text.getFillColor();
	}
	void TextEntry::setBackGroundColor(sf::Color color){
		background_color = color;
	}
	sf::Color TextEntry::getBackGroundColor() const {
		return background_color;
	}
	void TextEntry::onReturn(const std::string& entered_text){
		
	}
	void TextEntry::onType(const std::string& full_text){
		
	}
	void TextEntry::render(sf::RenderWindow& renderwindow){
		sf::RectangleShape rect(size);
		rect.setFillColor(background_color);
		renderwindow.draw(rect);
		renderwindow.draw(text);
		if (Context::getTextEntry() == this){
			sf::RectangleShape rect2(sf::Vector2f(cursor_width, text.getCharacterSize()));
			rect2.setFillColor(sf::Color(
				text.getColor().r,
				text.getColor().g,
				text.getColor().b,
				128 * (0.5 + 0.5 * sin(getProgramTime() * PI * 2))));
			rect2.setPosition(sf::Vector2f(cursor_pos, 0));
			renderwindow.draw(rect2);
		}
	}
	void TextEntry::onLeftClick(int clicks){
		beginTyping();
		moveTo(localMousePos());
	}
	void TextEntry::onFocus(){
		beginTyping();
	}
	void TextEntry::write(char ch){
		std::string oldstring = text.getString();
		text.setString(oldstring.substr(0, cursor_index) + ch + oldstring.substr(cursor_index, oldstring.size() - 1));
		cursor_index += 1;
		updateSize();
		onType(text.getString());
	}
	void TextEntry::onBackspace(){
		if (!text.getString().isEmpty() && cursor_index > 0){
			std::string newstring = text.getString();
			newstring.erase(newstring.begin() + cursor_index - 1);
			cursor_index -= 1;
			text.setString(newstring);
			updateSize();
		}
		onType(text.getString());
	}
	void TextEntry::onDelete(){
		if (!text.getString().isEmpty() && cursor_index < text.getString().getSize()){
			std::string newstring = text.getString();
			newstring.erase(cursor_index, 1);
			text.setString(newstring);
			updateSize();
		}
		onType(text.getString());
	}
	void TextEntry::onLeft(){
		if (cursor_index > 0){
			cursor_index -= 1;
			updateSize();
		}
	}
	void TextEntry::onRight(){
		if (cursor_index < text.getString().getSize()){
			cursor_index += 1;
			updateSize();
		}
	}
	void TextEntry::onHome(){
		cursor_index = 0;
		updateSize();
	}
	void TextEntry::onEnd(){
		cursor_index = text.getString().getSize();
		updateSize();
	}
	void TextEntry::updateSize(){
		sf::FloatRect rect = text.getLocalBounds();
		if (rect.width <= 1){
			size = sf::Vector2f(text.getCharacterSize(), text.getCharacterSize());
		} else {
			size = sf::Vector2f(rect.width, text.getCharacterSize());
		}
		cursor_pos = text.findCharacterPos(cursor_index).x;
		if (cursor_index == text.getString().getSize()){
			cursor_width = text.getCharacterSize() * 0.5f;
		} else {
			cursor_width = text.findCharacterPos(cursor_index + 1).x - cursor_pos;
		}
	}



	Window* root(){
		static Window rootwin;
		return &rootwin;
	}
	void addKeyboardCommand(sf::Keyboard::Key trigger_key, void (*handler)()){
		Context::addKeyboardCommand(trigger_key, handler);
	}
	void addKeyboardCommand(sf::Keyboard::Key trigger_key, std::vector<sf::Keyboard::Key> required_keys, void (*handler)()){
		Context::addKeyboardCommand(trigger_key, required_keys, handler);
	}
	void setQuitHandler(bool (*handler)()){
		Context::setQuitHandler(handler);
	}
	long double getProgramTime(){
		return Context::getProgramTime();
	}
	sf::Vector2f getScreenSize(){
		return (sf::Vector2f)Context::getRenderWindow().getSize();
	}
	sf::Vector2f getMousePos(){
		return sf::Vector2f(sf::Mouse::getPosition(Context::getRenderWindow()));
	}

	void init(sf::Vector2f size, std::string title, int target_fps){
		Context::init(size, title, 1.0 / target_fps);
	}
	void quit(bool force){
		Context::handleQuit(force);
	}
	void run(){
		long double prev_time = Context::getProgramTime();
		while (Context::getRenderWindow().isOpen() && !Context::hasQuit()){
			sf::Event event;
			while (Context::getRenderWindow().pollEvent(event)){
				switch (event.type){
					case sf::Event::Closed:
						quit();
						break;
					case sf::Event::Resized:
						Context::resize(event.size.width, event.size.height);
						break;
					case sf::Event::LostFocus:
						if (Context::getCurrentWindow()){
							Context::focusTo(nullptr);
						}
						if (Context::getDraggingWindow()){
							Context::setDraggingWindow(nullptr);
						}
						break;
					case sf::Event::TextEntered:
						if (Context::getTextEntry()){
							if (event.text.unicode != '\b'){
								Context::getTextEntry()->write(static_cast<char>(event.text.unicode));
							}
						}
						break;
					case sf::Event::KeyPressed:
						if (Context::getTextEntry()){
							switch (event.key.code){
								case sf::Keyboard::BackSpace:
									Context::getTextEntry()->onBackspace();
									break;
								case sf::Keyboard::Delete:
									Context::getTextEntry()->onDelete();
									break;
								case sf::Keyboard::Left:
									Context::getTextEntry()->onLeft();
									break;
								case sf::Keyboard::Right:
									Context::getTextEntry()->onRight();
									break;
								case sf::Keyboard::Home:
									Context::getTextEntry()->onHome();
									break;
								case sf::Keyboard::End:
									Context::getTextEntry()->onEnd();
									break;
								case sf::Keyboard::Return:
									Context::getTextEntry()->onReturn(Context::getTextEntry()->getText());
									break;
								default:
									Context::handleKeyPress(event.key.code);
									break;
							}
						} else {
							Context::handleKeyPress(event.key.code);
						}
						
						break;
					case sf::Event::KeyReleased:
						if (Context::getCurrentWindow()){
							Context::getCurrentWindow()->onKeyUp(event.key.code);
						}
						break;
					case sf::Event::MouseButtonPressed:
					{
						Context::handleMouseDown(event.mouseButton.button, sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
						break;
					}
					case sf::Event::MouseButtonReleased:
						Context::handleMouseUp(event.mouseButton.button, sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
						break;
					case sf::Event::MouseWheelScrolled:
						if (Context::getCurrentWindow()){
							if (event.mouseWheelScroll.wheel == sf::Mouse::Wheel::HorizontalWheel){
								Context::getCurrentWindow()->onScroll(event.mouseWheelScroll.delta, 0.0);
							} else if (event.mouseWheelScroll.wheel == sf::Mouse::Wheel::HorizontalWheel){
								Context::getCurrentWindow()->onScroll(0.0, event.mouseWheelScroll.delta);
							}
						}
						break;
				}
			}

			//drag what's being dragged
			Context::handleDrag();

			//mouse-over what needs mousing over
			Context::handleHover();

			//apply transitions
			Context::applyTransitions();

			//clear the screen
			Context::getRenderWindow().clear();
			Context::resetView();

			//render the root window, and all child windows it contains
			root()->size = getScreenSize();
			root()->render(Context::getRenderWindow());

			//highlight current window if alt is pressed
			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt)) && Context::getCurrentWindow()){
				sf::RectangleShape rect(Context::getCurrentWindow()->size);
				rect.setPosition(Context::getCurrentWindow()->absPos());
				rect.setFillColor(sf::Color(0));
				rect.setOutlineColor(sf::Color(0xFFFF0080));
				rect.setOutlineThickness(2);
				Context::getRenderWindow().draw(rect);
			}

			Context::getRenderWindow().display();

			//sleep only as long as needed
			long double now = Context::getProgramTime();
			double delay = Context::getRenderDelay();
			sf::sleep(sf::seconds(std::max(0.0f, (float)(Context::getRenderDelay() - (now - prev_time)))));
			prev_time = now;
		}

		root()->clear();
	}
}