#include <Flosion/UI/Core/NumberWire.hpp>

namespace flui {

	ui::Ref<NumberInput::WireType> NumberInput::createWire(ui::Ref<Box> box){
		return ui::create<NumberWire>(box);
	}

	ui::Ref<NumberOutput::WireType> NumberOutput::createWire(ui::Ref<Box> box){
		return ui::create<NumberWire>(box);
	}

} // namespace flui