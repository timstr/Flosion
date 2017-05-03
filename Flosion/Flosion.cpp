
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
// TODO: switch from SFML soundstream to PortAudio
int main(){

	ui::init({1000, 700}, "Oy Oy Oy Oh My Oy", 10);

	fui::MasterContainer* container = new fui::MasterContainer();

	std::vector<std::string> names = {"const", "+", "d0", "var", "slider", "usine", "usine", "dac", "dac", "audio", "wavegen", "square", "saw", "timestretch", "timestretch", "sampler", "+", "+", "-", "-", "*", "*", "/", "/", "approacher", "ensemble"};

	for (const std::string& name : names){
		fui::Object* obj = fui::Factory::createObject(name);
		if (obj){
			struct Data {
				sf::Vector2f start = {rand() % 900, -200};
				sf::Vector2f end = sf::Vector2f(rand() % 900, rand() % 600);
			} data;
			auto transfn = [](fui::Object* object, double x, Data data){
				object->pos = data.start + (data.end - data.start) * (float)(0.5 - 0.5 * cos(x * PI));
			};
			auto trans = new ui::Transition<Data, fui::Object>(obj, 1000, transfn, {}, data);
			ui::startTransition(trans);
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