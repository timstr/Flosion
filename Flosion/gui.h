#pragma once

#include <SFML\Graphics.hpp>
#include <map>
#include <functional>

// TODO:
// - find improvements anywhere, really
// - put comments on every function so I don't need to remember all this stuff accross the years
// - tab key navigation through a window's child windows? Enter/Escape to move to child/parent?
// - use SFML views to offset/clip child window rendering

const double PI = 3.14159265358979323846264338327950288;

namespace ui {

	struct Window;
	struct TextEntry;

	struct TransitionBase {
		virtual ~TransitionBase(){

		}
		uint32_t duration;
		uint32_t timestamp;
		bool completed = false;
		virtual void transition() = 0;
		virtual void complete() = 0;
		virtual bool belongsTo(Window* win) = 0;
	};

	//represents all shared information used in the GUI
	struct GUIContext {
		private:
		bool quit = false;
		int render_delay = 25;

		sf::Vector2f drag_offset = sf::Vector2f();

		sf::RenderWindow renderwindow;

		sf::Font font;

		Window* dragging_window = nullptr;
		Window* current_window = nullptr;
		TextEntry* text_entry = nullptr;

		std::vector<TransitionBase*> transitions;

		void applyTransitions(){
			for (int i = 0; i < transitions.size(); i++){
				if (!transitions[i]->completed){
					if ((transitions[i]->timestamp + transitions[i]->duration) <= clock.getElapsedTime().asMilliseconds()){
						transitions[i]->complete();
					} else {
						transitions[i]->transition();
					}
				}
			}

			for (int i = 0; i < transitions.size(); i++){
				if (transitions[i]->completed){
					delete transitions[i];
					transitions.erase(transitions.begin() + i);
					i -= 1;
				}
			}
		}

		sf::Clock clock;

		std::map<std::pair<sf::Keyboard::Key, std::vector<sf::Keyboard::Key>>, void (*)()> commands;
		bool (*quit_handler)() = nullptr;

		void focusTo(Window* window);

		const int doubleclicktime = 250;

		uint32_t click_timestamp;
		sf::Mouse::Button click_button;
		Window* click_window;

		// store target window on first click
		// compare to hit window on second click
		// if match, perform double click
		// otherwise, perform single click on both

		sf::Vector2f getMousePosition(){
			return (sf::Vector2f)sf::Mouse::getPosition(renderwindow);
		}

		void performClick(sf::Mouse::Button button, sf::Vector2f);

		friend sf::Font& getFont();
		friend void startTransition(TransitionBase* trans);
		friend void clearTransitions(Window* win);
		friend void addKeyboardCommand(sf::Keyboard::Key, void (*)());
		friend void addKeyboardCommand(sf::Keyboard::Key, std::vector<sf::Keyboard::Key>, void (*)());
		friend void setQuitHandler(bool (*)());
		friend long double getProgramTime();
		friend sf::Vector2f getScreenSize();
		friend sf::Vector2f getMousePos();
		friend void init(sf::Vector2f, std::string, int);
		friend void run();
		template<class WindowType, class Data>
		friend struct Transition;
		friend class Window;
		friend class TextEntry;
		friend void quit(bool force);
		friend Window* root();
	} Context;

	template<class Data, class WindowType=Window>
	struct Transition : TransitionBase {
		Transition(WindowType* _window, uint32_t _duration, const std::function<void(WindowType*,double,Data)>& _transitionFn, const std::function<void(WindowType*)> _onComplete = {}, Data _data = Data()){
			window = _window;
			duration = _duration;
			transitionFn = _transitionFn;
			onComplete = _onComplete;
			data = _data;
		}
		WindowType* window;
		std::function<void(WindowType*, double, Data)> transitionFn;
		std::function<void(WindowType*)> onComplete;
		Data data;

		void transition() override {
			if (transitionFn){
				double progress = (Context.clock.getElapsedTime().asMilliseconds() - timestamp) / (double)duration;
				transitionFn(window, progress, data);
			}
		}

		void complete() override {
			if (transitionFn){
				transitionFn(window, 1.0, data);
			}
			if (!completed && onComplete){
				onComplete(window);
			}
			completed = true;
		}

		bool belongsTo(Window* win) override {
			return (window == win);
		}
	};

	struct Window {
		bool disabled = false;
		bool visible = true;
		sf::Vector2f pos;
		sf::Vector2f size;

		virtual ~Window(){
			if (parent){
				parent->releaseChildWindow(this);
				Context.focusTo(parent);
			}

			clearTransitions(this);
			clear();
			if (Context.dragging_window == this){
				Context.dragging_window = nullptr;
			}
		}

		void close(){
			delete this;
		}

		// returns true if 'testpos' is inside the bounds of the window
		// 'testpos' shall be relative to parent window
		virtual bool hit(sf::Vector2f testpos){
			return ((testpos.x >= pos.x) && (testpos.x < pos.x + size.x) && (testpos.y >= pos.y) && (testpos.y < pos.y + size.y));
		}

		sf::Vector2f localMousePos(){
			sf::Vector2f pos = (sf::Vector2f)sf::Mouse::getPosition(Context.renderwindow);
			Window *window = this;
			while (window){
				pos -= window->pos;
				window = window->parent;
			}
			return(pos);
		}
		sf::Vector2f absPos(){
			sf::Vector2f pos = sf::Vector2f();
			Window *window = this;
			while (window != nullptr){
				pos += window->pos;
				window = window->parent;
			}
			return(pos);
		}

		virtual void onLeftClick(int clicks){

		}
		virtual void onLeftRelease(){

		}
		virtual void onRightClick(int clicks){

		}
		virtual void onRightRelease(){

		}

		void startDrag(){
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right)){
				if (Context.dragging_window != this){
					Context.focusTo(this);
					Context.drag_offset = (sf::Vector2f)sf::Mouse::getPosition(Context.renderwindow) - pos;
					Context.dragging_window = this;
				}
			}
		}
		virtual void onDrag(){

		}

		virtual void onHover(){

		}
		virtual void onHoverWithDrag(Window *drag_window){

		}
		virtual void onDropDragWindow(Window *window){

		}

		virtual void onFocus(){

		}
		virtual void onLoseFocus(){

		}
		void grabFocus(){
			Context.focusTo(this);
		}

		virtual void onKeyDown(sf::Keyboard::Key key){

		}
		virtual void onKeyUp(sf::Keyboard::Key key){

		}
		bool keyDown(sf::Keyboard::Key key){
			if (Context.current_window == this){
				return(sf::Keyboard::isKeyPressed(key));
			} else {
				return(false);
			}
		}

		void addChildWindow(Window *window){
			if (window->parent != nullptr){
				throw;
			}
			window->parent = this;
			childwindows.insert(childwindows.begin(), window);
		}
		void addChildWindow(Window *window, sf::Vector2f pos){
			window->pos = pos;
			addChildWindow(window);
		}
		void releaseChildWindow(Window* window){
			for (int i = 0; i < childwindows.size(); i++){
				if (childwindows[i] == window){
					childwindows[i]->parent = nullptr;
					childwindows.erase(childwindows.begin() + i);
					return;
				}
			}
			throw;
		}
		void bringChildToFront(Window *window){
			for (int i = 0; i < childwindows.size(); i++){
				if (childwindows[i] == window){
					childwindows.erase(childwindows.begin() + i);
					childwindows.insert(childwindows.begin(), window);
					return;
				}
			}
			throw;
		}
		void clear(){
			while (childwindows.size() > 0){
				childwindows[0]->close();
			}
		}

		Window* findWindowAt(sf::Vector2f _pos){
			if (!visible || disabled){
				return nullptr;
			}

			Window* window = nullptr;
			for (int i = 0; i < childwindows.size() && window == nullptr; i++){
				window = childwindows[i]->findWindowAt(_pos - pos);
				if (window && (window == Context.dragging_window)){
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

		virtual void render(sf::RenderWindow& renderwindow, sf::Vector2f offset){
			sf::RectangleShape rectshape;
			rectshape.setPosition(pos + offset);
			rectshape.setSize(size);
			rectshape.setFillColor(sf::Color((((uint32_t)std::hash<Window*>{}(this)) & 0xFFFFFF00) | 0x80));
			rectshape.setOutlineColor(sf::Color(0xFF));
			rectshape.setOutlineThickness(1);
			renderwindow.draw(rectshape);
			renderChildWindows(renderwindow, offset);
		}
		void renderChildWindows(sf::RenderWindow& renderwindow, sf::Vector2f offset){
			for (int i = childwindows.size() - 1; i >= 0; i -= 1){
				if (childwindows[i]->visible){
					childwindows[i]->render(renderwindow, offset + pos);
				}
			}
		}

		protected:
		std::vector<Window*> childwindows;

		private:
		Window* parent = nullptr;

		friend class GUIContext;
	};

	void GUIContext::focusTo(Window* window){
		if (Context.dragging_window){
			Context.dragging_window = nullptr;
		}
		if (Context.text_entry){
			Context.text_entry = nullptr;
		}
		if (Context.current_window != window){
			std::vector<Window*> current_path;
			std::vector<Window*> new_path;

			Window* twindow = Context.current_window;
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
				if (new_path.back()->parent){
					new_path.back()->parent->bringChildToFront(new_path.back());
				}
				new_path.back()->onFocus();
				new_path.pop_back();
			}

			Context.current_window = window;
		}
	}

	struct Text : Window {
		Text(const std::string& _text, const sf::Font& _font, sf::Color color = sf::Color(0xFF), int charsize = 15){
			text = sf::Text("", _font, charsize);
			text.setString(_text);
			text.setFillColor(color);
			updateSize();
			disabled = true;
		}

		void setText(const std::string& _text){
			text.setString(_text);
			updateSize();
		}
		const std::string& getText(){
			return text.getString();
		}

		void render(sf::RenderWindow& renderwin, sf::Vector2f offset){
			text.setPosition(pos + offset);
			renderwin.draw(text);
		}

		private:

		void updateSize(){
			size.x = text.getLocalBounds().width;
			size.y = text.getLocalBounds().height;
		}

		sf::Text text;
	};

	struct TextEntry : Window {
		TextEntry(sf::Font& font, int charsize = 15){
			text = sf::Text("", font, charsize);
			updateSize();
		}
		TextEntry(const std::string& str, sf::Font& font, int charsize = 15){
			text = sf::Text(str, font, charsize);
			updateSize();
		}
		TextEntry(const std::string& str){
			text = sf::Text(str, Context.font, 15);
			updateSize();
		}

		void setText(const std::string& str){
			text.setString(str);
		}
		std::string getText() const {
			return text.getString().toAnsiString();
		}

		void clearText(){
			text.setString("");
			cursor_index = 0;
			updateSize();
		}

		//to be overridden and used to deal with submission of newly typed text
		virtual void onReturn(std::string entered_text){

		}

		void render(sf::RenderWindow& renderwindow, sf::Vector2f offset) override {
			sf::RectangleShape rect(size);
			rect.setPosition(pos + offset);
			rect.setFillColor(sf::Color(0xFF));
			rect.setOutlineColor(sf::Color(0x808080FF));
			rect.setOutlineThickness(1);
			renderwindow.draw(rect);
			text.setPosition(offset + pos);
			renderwindow.draw(text);
			if (Context.text_entry == this){
				sf::RectangleShape rect2(sf::Vector2f(cursor_width, 15));
				rect2.setFillColor(sf::Color(
					255,
					255,
					0,
					128 * (0.5 + 0.5 * sin(getProgramTime() * PI / 1.5))));
				rect2.setPosition(pos + offset + sf::Vector2f(cursor_pos, 0));
				renderwindow.draw(rect2);
			}
		}

		void onLeftClick(int clicks) override {
			Context.text_entry = this;
			sf::Vector2f mousepos = localMousePos();
			for (int i = 0; i < text.getString().getSize(); i++){
				sf::Vector2f charpos = text.findCharacterPos(i);
				if (mousepos.x < charpos.x){
					cursor_index = i - 1;
					updateSize();
					return;
				}
			}
			cursor_index = text.getString().getSize();
			updateSize();
		}

		private:
		// common text entry functions...
		void write(char ch){
			std::string oldstring = text.getString();
			text.setString(oldstring.substr(0, cursor_index) + ch + oldstring.substr(cursor_index, oldstring.size() - 1));
			cursor_index += 1;
			updateSize();
		}

		void onBackspace(){
			if (!text.getString().isEmpty() && cursor_index > 0){
				std::string newstring = text.getString();
				newstring.erase(newstring.begin() + cursor_index - 1);
				cursor_index -= 1;
				text.setString(newstring);
				updateSize();
			}
		}
		void onDelete(){
			if (!text.getString().isEmpty() && cursor_index < text.getString().getSize()){
				std::string newstring = text.getString();
				newstring.erase(cursor_index, 1);
				text.setString(newstring);
				updateSize();
			}
		}
		void onLeft(){
			if (cursor_index > 0){
				cursor_index -= 1;
				updateSize();
			}
		}
		void onRight(){
			if (cursor_index < text.getString().getSize()){
				cursor_index += 1;
				updateSize();
			}
		}
		void onHome(){
			cursor_index = 0;
			updateSize();
		}
		void onEnd(){
			cursor_index = text.getString().getSize();
			updateSize();
		}

		void updateSize(){
			sf::FloatRect rect = text.getLocalBounds();
			if (rect.width <= 1){
				size = {15, 15};
			} else {
				size = {rect.width, 15};
			}
			cursor_pos = text.findCharacterPos(cursor_index).x;
			if (cursor_index == text.getString().getSize()){
				cursor_width = 7.5;
			} else {
				cursor_width = text.findCharacterPos(cursor_index + 1).x - cursor_pos;
			}
		}

		sf::Text text;
		int cursor_index = 0;
		float cursor_pos;
		float cursor_width;

		friend void run();
	};

	Window* root(){
		struct RootWindow : Window {
			~RootWindow(){
				if (!Context.quit){
					throw std::runtime_error("The GUI was not closed correctly");
				}
			}
		};
		static RootWindow rootwin;
		return &rootwin;
	}

	void GUIContext::performClick(sf::Mouse::Button button, sf::Vector2f pos){
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

					Context.focusTo(hitwin);
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

				Context.focusTo(hitwin);

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

			Context.focusTo(hitwin);
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

	sf::Font& getFont(){
		return Context.font;
	}

	void clearTransitions(Window* win){
		for (int i = 0; i < Context.transitions.size(); i++){
			if (Context.transitions[i]->belongsTo(win)){
				Context.transitions[i]->completed = true;
			}
		}
	}

	void startTransition(TransitionBase* trans){
		trans->timestamp = Context.clock.getElapsedTime().asMilliseconds();
		trans->completed = false;
		Context.transitions.push_back(trans);
	}

	void addKeyboardCommand(sf::Keyboard::Key trigger_key, void (*handler)()){
		auto pair = std::pair<sf::Keyboard::Key, std::vector<sf::Keyboard::Key>>(trigger_key, {});
		auto it = Context.commands.find(pair);
		if (it == Context.commands.end()){
			Context.commands[pair] = handler;
		} else {
			throw;
		}
	}
	void addKeyboardCommand(sf::Keyboard::Key trigger_key, std::vector<sf::Keyboard::Key> required_keys, void (*handler)()){
		auto pair = std::pair<sf::Keyboard::Key, std::vector<sf::Keyboard::Key>>(trigger_key, required_keys);
		auto it = Context.commands.find(pair);
		if (it == Context.commands.end()){
			Context.commands[pair] = handler;
		} else {
			throw;
		}
	}
	void setQuitHandler(bool (*handler)()){
		Context.quit_handler = handler;
	}

	long double getProgramTime(){
		return Context.clock.getElapsedTime().asMilliseconds() / 1000.0;
	}

	sf::Vector2f getScreenSize(){
		return (sf::Vector2f)Context.renderwindow.getSize();
	}

	sf::Vector2f getMousePos(){
		return sf::Vector2f(sf::Mouse::getPosition(Context.renderwindow));
	}

	void init(sf::Vector2f size = sf::Vector2f(500, 500), std::string title = "Behold", int render_delay = 25){//TODO: this might goof up, make bool?
		Context.current_window = root();

		Context.render_delay = render_delay;

		sf::ContextSettings settings;
		settings.antialiasingLevel = 8;

		Context.renderwindow.create(sf::VideoMode(size.x, size.y), title, sf::Style::Default, settings);

		Context.font.loadFromFile("seguisb.ttf");

		Context.clock.restart();

		srand(unsigned(time(nullptr)));
	}

	void quit(bool force = false){
		if (Context.quit_handler && !force){
			if (Context.quit_handler()){
				Context.quit = true;
			}
		} else {
			Context.quit = true;
		}
	}

	void run(){
		int prev_time = Context.clock.getElapsedTime().asMilliseconds();
		while (Context.renderwindow.isOpen() && !Context.quit){
			sf::Event event;
			while (Context.renderwindow.pollEvent(event)){
				switch (event.type){
					case sf::Event::Closed:
						quit();
						break;
					case sf::Event::Resized:
						Context.renderwindow.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
						break;
					case sf::Event::LostFocus:
						if (Context.current_window){
							Context.focusTo(nullptr);
						}
						if (Context.dragging_window){
							Context.dragging_window = nullptr;
						}
						break;
					case sf::Event::TextEntered:
						if (Context.text_entry){
							if (event.text.unicode != '\b'){
								Context.text_entry->write(static_cast<char>(event.text.unicode));
							}
						}
						break;
					case sf::Event::KeyPressed:
						if (Context.text_entry){
							switch (event.key.code){
								case sf::Keyboard::BackSpace:
									Context.text_entry->onBackspace();
									break;
								case sf::Keyboard::Delete:
									Context.text_entry->onDelete();
									break;
								case sf::Keyboard::Left:
									Context.text_entry->onLeft();
									break;
								case sf::Keyboard::Right:
									Context.text_entry->onRight();
									break;
								case sf::Keyboard::Home:
									Context.text_entry->onHome();
									break;
								case sf::Keyboard::End:
									Context.text_entry->onEnd();
									break;
								case sf::Keyboard::Return:
									Context.text_entry->onReturn(Context.text_entry->getText());
									break;
							}
						} else {
							auto it = Context.commands.begin();
							int max = 0;
							auto current_it = Context.commands.end();
							while (it != Context.commands.end()){
								if (it->first.first == event.key.code){
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

							if (current_it != Context.commands.end()){
								current_it->second();
							} else if (Context.current_window){
								Context.current_window->onKeyDown(event.key.code);
							}
						}
						break;
					case sf::Event::KeyReleased:
						if (Context.current_window){
							Context.current_window->onKeyUp(event.key.code);
						}
						break;
					case sf::Event::MouseButtonPressed:
					{
						Context.performClick(event.mouseButton.button, sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
						break;
					}
					case sf::Event::MouseButtonReleased:
						if (Context.dragging_window){
							if (event.mouseButton.button == sf::Mouse::Left){
								Context.dragging_window->onLeftRelease();
							} else if (event.mouseButton.button == sf::Mouse::Right){
								Context.dragging_window->onRightRelease();
							}

							Window* hover_window = root()->findWindowAt(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
							if (hover_window){
								hover_window->onDropDragWindow(Context.dragging_window);
							}
							Context.dragging_window = nullptr;
						} else if (Context.current_window){
							if (event.mouseButton.button == sf::Mouse::Left){
								Context.current_window->onLeftRelease();
							} else if (event.mouseButton.button == sf::Mouse::Right){
								Context.current_window->onRightRelease();
							}
						}
						break;
				}
			}

			//drag what's being dragged
			if (Context.dragging_window){
				Context.dragging_window->pos = (sf::Vector2f)sf::Mouse::getPosition(Context.renderwindow) - Context.drag_offset;
				Context.dragging_window->onDrag();
			}

			//mouse-over what needs mousing over
			Window* hover_window = root()->findWindowAt((sf::Vector2f)sf::Mouse::getPosition(Context.renderwindow));
			if (hover_window){
				if (Context.dragging_window){
					hover_window->onHoverWithDrag(Context.dragging_window);
				} else {
					hover_window->onHover();
				}
			}

			//apply transitions
			Context.applyTransitions();

			//clear the screen
			Context.renderwindow.clear();

			//render the root window, and all child windows it contains
			root()->size = getScreenSize();
			root()->render(Context.renderwindow, {0, 0});

			//highlight current window if alt is pressed
			if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt)) && Context.current_window){
				sf::RectangleShape rect(Context.current_window->size);
				rect.setPosition(Context.current_window->absPos());
				rect.setFillColor(sf::Color(0));
				rect.setOutlineColor(sf::Color(0xFFFF0080));
				rect.setOutlineThickness(2);
				Context.renderwindow.draw(rect);
			}

			Context.renderwindow.display();

			//sleep only as long as needed
			sf::sleep(sf::milliseconds(Context.render_delay - (Context.clock.getElapsedTime().asMilliseconds() - prev_time)));
			prev_time = Context.clock.getElapsedTime().asMilliseconds();
		}

		root()->clear();
	}
}