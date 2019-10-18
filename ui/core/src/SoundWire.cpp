#include "soundwireUI.h"

namespace flui {

	ui::Ref<SoundInput::WireType> SoundInput::createWire(ui::Ref<Box> box) {
		return ui::create<SoundWire>(box);
	}

	ui::Ref<SoundOutput::WireType> SoundOutput::createWire(ui::Ref<Box> box) {
		return ui::create<SoundWire>(box);
	}

} // namespace flui
