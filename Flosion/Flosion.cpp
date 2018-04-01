#include "musical.h"

#include <SFML/Graphics.hpp>
#include <math.h>
#include "filebrowser.h"
#include <chrono>

#include <gui/gui.h>

#include "constantUI.h"
#include "functionsUI.h"
#include "dacUI.h"
#include "audioUI.h"
#include "wavegenUI.h"
#include "timestretchUI.h"
#include "samplerUI.h"
#include "ensembleUI.h"
#include "splineUI.h"
#include "filterUI.h"
#include "phasevocoderUI.h"
#include "convolverUI.h"
#include "distortionUI.h"
#include "wavetableUI.h"

#include <iostream>
#include <iomanip>

#include "fft.h"

// TODO: optimize for RVO and move semantics
// TODO: synchronize this stuff
// TODO: cache things
// TODO: scatter
// TODO: ring mod
// TODO: feedback
// TODO: make window sizes customizable for filter
// TODO: clipboard
// TODO: serialization and file loading/saving

int main(){

	srand(unsigned(time(nullptr)));

	ui::init(1000, 700, "Flosion", 30);

	fui::MasterBox* box = new fui::MasterBox();

	box->addObject(new fui::SliderObject(0.0f, 1000.0f));
	box->addObject(new fui::SliderObject(0.0f, 1000.0f));
	box->addObject(new fui::SliderObject(0.0f, 2.0f));
	box->addObject(new fui::SliderObject(0.0f, 0.1f));

	ui::root()->addChildWindow(box);

	ui::run();

	return 0;
}