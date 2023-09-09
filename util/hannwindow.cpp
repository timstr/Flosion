#include "hannwindow.h"
#include <math.h>

// TODO: split in half and mirror to make more space efficient
// TODO: measure performance tradeoff of LUT
float hannwindow_256[256];
float hannwindow_512[512];
float hannwindow_1024[1024];
float hannwindow_2048[2048];
float hannwindow_4096[4096];
float hannwindow_8192[8192];
float hannwindow_16384[16384];
float hannwindow_32768[32768];

float getHannWindow(unsigned int x, unsigned int windowsize){
	switch (windowsize){
		case 256:
			return hannwindow_256[x];
		case 512:
			return hannwindow_512[x];
		case 1024:
			return hannwindow_1024[x];
		case 2048:
			return hannwindow_2048[x];
		case 4096:
			return hannwindow_4096[x];
		case 8192:
			return hannwindow_8192[x];
		case 16384:
			return hannwindow_16384[x];
		case 32768:
			return hannwindow_32768[x];
	}
	return (float)pow(sin(3.141592654f * (float)x / (float)(windowsize - 1)), 2.0f);
}

struct HannWindowInit {
	HannWindowInit(){
		init(hannwindow_256, 256);
		init(hannwindow_512, 512);
		init(hannwindow_1024, 1024);
		init(hannwindow_2048, 2048);
		init(hannwindow_4096, 4096);
		init(hannwindow_8192, 8192);
		init(hannwindow_16384, 16384);
		init(hannwindow_32768, 32768);
	}

	void init(float* window, unsigned int size){
		for (unsigned int i = 0; i < size; i++){
			window[i] = (float)pow(sin(3.141592654f * (float)i / (float)(size - 1)), 2.0f);
		}
	}
} hannwindowinit;