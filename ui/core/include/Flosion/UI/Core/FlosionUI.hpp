#pragma once

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/UI/Core/Box.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

	// The entry point of it all
	class FlosionUI : public ui::GridContainer {
    public:
		FlosionUI();

	private:
		MainBox& m_box;
	};

}