#pragma once

#include <gui/gui.h>
#include "objectUI.h"

namespace fui {

	// SoundWire connects a SoundInput to a SoundOutput
	struct SoundWire : Object {
		struct Head : ui::Window {
			Head(SoundWire* _wire);
			void onLeftClick(int clicks);
			bool onDropWindow(Window* window) override;
			SoundWire* wire;
		};
		struct Tail : ui::Window {
			Tail(SoundWire* _wire);
			void onLeftClick(int clicks);
			bool onDropWindow(Window* window) override;
			SoundWire* wire;
		};

		SoundWire();

		void ConnectHeadTo(SoundInput* input);
		void ConnectTailTo(SoundOutput* output);

		void render(sf::RenderWindow& rw) override;

		void dragHead();
		void dragTail();

		private:
		SoundInput* dst;
		SoundOutput* src;
		Head* head;
		Tail* tail;

		friend struct SoundInput;
		friend struct SoundOutput;
	};

} // namespace fui