#include <SFML/Graphics.hpp>
#include <math.h>
#include "filebrowser.h"

#include "gui.h"

#define MUSICAL_CHUNK_SIZE 1024;

#include "constantUI.h"
#include "functionsUI.h"
#include "dacUI.h"
#include "audioUI.h"
#include "wavegenUI.h"
#include "timestretchUI.h"
#include "samplerUI.h"
#include "ensembleUI.h"
#include "splineUI.h"

#include <iostream>

// TODO: synchronize this stuff
// TODO: cache things
int main(){

	srand(unsigned(time(nullptr)));

	ui::init({1000, 700}, "Oy Oy Oy Oh My Oy", 30);

	fui::MasterContainer* container = new fui::MasterContainer();

	container->addObject(new fui::SliderObject(0, 1000));
	container->addObject(new fui::SliderObject(0, 1000));
	container->addObject(new fui::SliderObject(0, 2));
	container->addObject(new fui::SliderObject(0, 0.1));

	ui::root()->addChildWindow(container);

	ui::run();

	return 0;
}