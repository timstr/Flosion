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

		bool safeToConnect(NumberInput* input);
		bool safeToConnect(NumberOutput* output);
		void ConnectHeadTo(NumberInput* input);
		void ConnectTailTo(NumberOutput* output);

		void render(sf::RenderWindow& rw) override;

		void dragHead();
		void dragTail();

		private:
		NumberInput* dst;
		NumberOutput* src;
		Head* head;
		Tail* tail;

		friend struct NumberInput;
		friend struct NumberOutput;
	};

} // namespace fui