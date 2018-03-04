#pragma once

#include <gui/gui.h>
#include "SoundSource.h"

namespace fui {

	struct Object;
	struct SoundWire;

	// the source of a sound wire
	// corresponds to a sound source
	struct SoundOutput : ui::Window {
		SoundOutput(musical::SoundSource* _target, Object* _parent, std::string _caption = "Sound Output");
		~SoundOutput();

		void addWireOut(SoundWire* wire);
		void removeWireOut(SoundWire* wire);

		void render(sf::RenderWindow& rw) override;

		bool onDropWindow(Window* window) override;

		void onHover() override;
		void onHoverWithWindow(Window* win) override;

		void onLeftClick(int clicks) override;

		std::string getCaption() const;

		musical::SoundSource* getSource();

		vec2 getWireDirection() const;

		private:
		Object* const owner_object;
		musical::SoundSource* const target;
		std::vector<SoundWire*> wires_out;
		ui::Text* caption;
		double hover_timestamp;

		friend struct SoundWire;
	};

} // namespace fui