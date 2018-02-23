#pragma once

#include <gui/gui.h>
#include "objectUI.h"

namespace fui {

	// TODO: rename to 'Box'
	
	// Container is what holds all objects
	// allows convenient clustering of objects and collapsing into a more compact view
	// TODO:
	// Enable selections
	struct Container : Object {

		// add a new object
		void addObject(Object* obj);
		// remove an object, without destroying
		void releaseObject(Object* obj);

		// collapse the container such that its contained objects aren't rendered
		// and it takes up much less screen space
		void collapse();
		// expand the container to show its contents
		void expand();

		void render(sf::RenderWindow& renderwin) override;

		void onLeftClick(int clicks) override;

		private:
		std::vector<Object*> objects;

		struct Menu : ui::Window {
			Menu(Container* _container);
			void beginTyping();
			void onLoseFocus() override;
			void render(sf::RenderWindow& rw) override;
			void refreshList(const std::string& text);
			private:
			struct ListItem : ui::Window {
				ListItem(Menu* _menu, const std::string& _str);
				void onLeftClick(int clicks) override;
				void onKeyDown(sf::Keyboard::Key key) override;
				void render(sf::RenderWindow& rw) override;	
				Menu* menu;
				std::string str;
			};
			std::vector<ListItem*> listitems;
			struct TextField : ui::TextEntry {
				TextField(Menu* _menu);
				void onType(std::string text) override;
				void onReturn(std::string text) override;
				void onKeyDown(sf::Keyboard::Key key) override;
				Menu* menu;
			}* textentry;
			Container* container;
			Object* createObject(const std::string& str);
			Object* createConstant(const std::string& str);
			void addObject(Object* obj);
		};

		friend struct Object;
	};

	// MasterContainer is the singleton-intended container that holds everything in the current work area
	struct MasterContainer : Container {
		void render(sf::RenderWindow& renderwin) override;
	};

} // namespace fui