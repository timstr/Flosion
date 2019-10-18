#include <Flosion/UI/Core/FlosionUI.hpp>

#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

	FlosionUI::FlosionUI()
		: GridContainer(1, 1)
        , m_box(putCell<MainBox>(0, 0)) {
		
	}

} // namespace flui
