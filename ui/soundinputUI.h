#pragma once

#include <gui/gui.h>
#include "SoundSource.h"

namespace fui {

	struct Object;
	struct SoundWire;

	// the destination for a sound wire
	// corresponds to a sound input of a processing object
	struct SoundInput : ui::Window {
		SoundInput(musical::SoundInput* _target, Object* _parent, std::string _caption = "Sound Input");
		~SoundInput();

		void setWireIn(SoundWire* wire);

		bool onDropWindow(Window* window) override;

		void onHover() override;
		void onHoverWithWindow(Window* win) override;

		void render(sf::RenderWindow& rw) override;

		void onLeftClick(int clicks) override;

		std::string getCaption() const;

		musical::SoundInput* getInput() const;

		vec2 getWireDirection() const;

		private:
		Object* const owner_object;
		musical::SoundInput* const target;
		SoundWire* wire_in;
		ui::Text* caption;
		double hover_timestamp;

		friend struct SoundWire;
	};

} // namespace fui