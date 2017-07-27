
#include <iostream>
#include <SFML/Graphics.hpp>
#include <math.h>
#include "filebrowser.h"

#include "gui.h"

#define MUSICAL_CHUNK_SIZE 2048;

#include "musical.h"
#include "audio.h"
#include "dac.h"

#include "FlosionUI.h"

// TODO: synchronize this stuff
// TODO: switch from SFML soundstream to PortAudio?
int main(){

	srand(unsigned(time(nullptr)));

	ui::init({1000, 700}, "Oy Oy Oy Oh My Oy", 30);

	fui::MasterContainer* container = new fui::MasterContainer();

	std::vector<std::string> names = {"const", "+", "d0", "var", "slider", "usine", "usine", "dac", "dac", "audio", "wavegen", "square", "saw", "timestretch", "timestretch", "sampler", "+", "+", "-", "-", "*", "*", "/", "/", "ensemble"};

	for (const std::string& name : names){
		fui::Object* obj = fui::Factory::createObject(name);
		if (obj){
			sf::Vector2f start = sf::Vector2f(rand() % 900, -200);
			sf::Vector2f end = sf::Vector2f(rand() % 900, rand() % 600);
			obj->startTransition(1.0, [=](double x){
				obj->pos = start + (end - start) * (float)(0.5 - 0.5 * cos(x * PI));
			});

			container->addObject(obj);
		} else {
			std::cout << "The object \"" << name << "\" couldn't be instantiated\n";
		}
	}

	container->addObject(new fui::SliderObject(0, 1000));
	container->addObject(new fui::SliderObject(0, 2));

	ui::root()->addChildWindow(container);

	ui::run();

	return 0;
}