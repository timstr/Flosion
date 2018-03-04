#pragma once

#include <gui/gui.h>
#include "SoundSource.h"

namespace fui {

	struct Object;
	struct SoundWire;

	// the destination for a sound wire
	// corresponds to a sound input of a processing object
	struct SoundInput : ui::Window {
		SoundInput(musical::SoundInput* _target, Object* _parent);
		~SoundInput();

		void setWireIn(SoundWire* wire);

		bool onDropWindow(Window* window) override;

		void render(sf::RenderWindow& rw) override;

		void onLeftClick(int clicks) override;

		vec2 getWireDirection() const;

		private:
		Object* const owner_object;
		musical::SoundInput* const target;
		SoundWire* wire_in;

		friend struct SoundWire;
	};

} // namespace fui