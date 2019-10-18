#include <Flosion/UI/Core/Font.hpp>

namespace flui {

	const sf::Font& getFont(){
		static bool init;
		static sf::Font font;
		if (!init){
			font.loadFromFile("fonts/JosefinSans-Bold.ttf");
			init = true;
		}
		return font;
	}

}