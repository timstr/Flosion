#pragma once

#include <SFML\Graphics.hpp>
#include <map>
#include <functional>

// TODO:
// - find improvements anywhere, really
// - put comments on every function so I don't need to remember all this stuff accross the years
// - tab key navigation through a window's child windows? Enter/Escape to move to child/parent?
// - use SFML views to offset/clip child window rendering

extern const double PI;

namespace ui {

	struct Window;
	struct Text;
	struct TextEntry;
	struct Transition;

	namespace {
		struct Context {
			static void init(sf::Vector2f size, std::string title, double _render_delay);

			static void addTransition(const Transition& transition);
			static void applyTransitions();
			static void clearTransitions(Window* target);

			static void addKeyboardCommand(sf::Keyboard::Key, void (*)());
			static void addKeyboardCommand(sf::Keyboard::Key, std::vector<sf::Keyboard::Key>, void (*)());
			static void setQuitHandler(bool (*handler)());

			static void handleKeyPress(sf::Keyboard::Key key);
			static void handleMouseUp(sf::Mouse::Button button, sf::Vector2f pos);
			static void handleDrag();
			static void handleHover();
			static void handleQuit(bool force);

			static bool hasQuit();


			static long double getProgramTime();

			// store target window on first click
			// compare to hit window on second click
			// if match, perform double click
			// otherwise, perform single click on both
			static void performClick(sf::Mouse::Button button, sf::Vector2f);

			static sf::RenderWindow& getRenderWindow();
			static double getRenderDelay();

			static sf::Vector2f getMousePosition();

			static Window* getDraggingWindow();
			static void setDraggingWindow(Window* window, sf::Vector2f offset = sf::Vector2f(0, 0));

			static void focusTo(Window* window);
			static Window* getCurrentWindow();
			static TextEntry* getTextEntry();
			static void setTextEntry(TextEntry* textentry);

			private:

			static bool quit;
			static double render_delay;

			static sf::Vector2f drag_offset;

			static sf::RenderWindow renderwindow;

			static Window* dragging_window;
			static Window* current_window;
			static TextEntry* text_entry;

			static std::vector<Transition> transitions;

			static sf::Clock clock;

			static std::map<std::pair<sf::Keyboard::Key, std::vector<sf::Keyboard::Key>>, void (*)()> commands;
			static bool (*quit_handler)();

			static const int doubleclicktime;

			static uint32_t click_timestamp;
			static sf::Mouse::Button click_button;
			static Window* click_window;
		};
	}

	struct Transition {
		Transition(Window* _target, double _duration, const std::function<void(double)>& _transitionFn, const std::function<void()>& _onComplete = {});

		void apply();

		bool complete() const;

		const Window* target;

		private:
		std::function<void(double)> transitionFn;
		std::function<void()> onComplete;
		bool completed;
		double duration;
		long double timestamp;
	};

	struct Window {
		bool disabled = false;
		bool visible = true;
		sf::Vector2f pos;
		sf::Vector2f size;

		virtual ~Window();

		void close();

		// returns true if 'testpos' is inside the bounds of the window
		// 'testpos' shall be relative to parent window
		virtual bool hit(sf::Vector2f testpos);

		sf::Vector2f localMousePos();
		sf::Vector2f absPos();

		virtual void onLeftClick(int clicks);
		virtual void onLeftRelease();
		virtual void onRightClick(int clicks);
		virtual void onRightRelease();

		void startDrag();
		virtual void onDrag();

		virtual void onHover();
		virtual void onHoverWithDrag(Window *drag_window);
		virtual void onDropDragWindow(Window *window);

		virtual void onFocus();
		virtual void onLoseFocus();
		void grabFocus();

		virtual void onKeyDown(sf::Keyboard::Key key);
		virtual void onKeyUp(sf::Keyboard::Key key);
		bool keyDown(sf::Keyboard::Key key);

		void addChildWindow(Window *window);
		void addChildWindow(Window *window, sf::Vector2f pos);
		void releaseChildWindow(Window* window);
		void bringChildToFront(Window *window);
		void clear();

		Window* findWindowAt(sf::Vector2f _pos);

		virtual void render(sf::RenderWindow& renderwindow, sf::Vector2f offset);
		void renderChildWindows(sf::RenderWindow& renderwindow, sf::Vector2f offset);

		void startTransition(double duration, const std::function<void(double)> transitionFn, const std::function<void()>& onComplete = {});

		protected:
		std::vector<Window*> childwindows;

		private:
		Window* parent = nullptr;

		friend struct Context;
	};

	struct Text : Window {
		Text(const std::string& _text, const sf::Font& _font, sf::Color color = sf::Color(0xFF), int charsize = 15);

		void setText(const std::string& _text);
		const std::string& getText();

		void render(sf::RenderWindow& renderwin, sf::Vector2f offset) override;

		private:

		void updateSize();

		sf::Text text;
	};

	struct TextEntry : Window {
		TextEntry(sf::Font& font, int charsize = 15);
		TextEntry(const std::string& str, sf::Font& font, int charsize = 15);

		void setText(const std::string& str);
		std::string getText() const;

		void clearText();

		//to be overridden and used to deal with submission of newly typed text
		virtual void onReturn(std::string entered_text);

		void render(sf::RenderWindow& renderwindow, sf::Vector2f offset) override;

		void onLeftClick(int clicks) override;

		private:
		// common text entry functions...
		void write(char ch);

		void onBackspace();
		void onDelete();
		void onLeft();
		void onRight();
		void onHome();
		void onEnd();

		void updateSize();

		sf::Text text;
		int cursor_index = 0;
		float cursor_pos;
		float cursor_width;

		friend void run();
	};

	Window* root();

	void addKeyboardCommand(sf::Keyboard::Key trigger_key, void (*handler)());
	void addKeyboardCommand(sf::Keyboard::Key trigger_key, std::vector<sf::Keyboard::Key> required_keys, void (*handler)());
	void setQuitHandler(bool (*handler)());

	long double getProgramTime();

	sf::Vector2f getScreenSize();

	sf::Vector2f getMousePos();

	void init(sf::Vector2f size = sf::Vector2f(500, 500), std::string title = "Behold", int target_fps = 30); //TODO: this might goof up, make bool?

	void quit(bool force = false);

	void run();
}