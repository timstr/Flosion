#pragma once

#include <gui/gui.h>
#include "objectUI.h"
#include "numberinputUI.h"
#include "numberoutputUI.h"

namespace fui {

	// NumberWire connects a NumberInput to a NumberOutput
	struct NumberWire : Object {
		struct Head : ui::Window {
			Head(NumberWire* _wire);
			void onLeftClick(int clicks);
			bool onDropWindow(Window* window) override;
			NumberWire* wire;
		};
		struct Tail : ui::Window {
			Tail(NumberWire* _wire);
			void onLeftClick(int clicks);
			bool onDropWindow(Window* window) override;
			NumberWire* wire;
		};

		NumberWire();
		~NumberWire();

		bool safeToConnect(NumberInput* input) const;
		bool safeToConnect(NumberOutput* output) const;
		void connectHeadTo(NumberInput* input);
		void connectTailTo(NumberOutput* output);

		void render(sf::RenderWindow& rw) override;

		void dragHead();
		void dragTail();

		void update();

		private:
		NumberInput* dst;
		NumberOutput* src;
		Head* head;
		Tail* tail;
		std::vector<sf::Vertex> vertices;

		friend struct NumberInput;
		friend struct NumberOutput;
	};

} // namespace fui