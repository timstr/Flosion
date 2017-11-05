#include "hannwindow.h"
#include <math.h>

float hannwindow[musical::CHUNK_SIZE];

float getHannWindow(unsigned int x){
	return hannwindow[x];
}

struct HannWindowInit {
	HannWindowInit(){
		for (unsigned int i = 0; i < musical::CHUNK_SIZE; i++){
			hannwindow[i] = (float)pow(sin(3.141592654f * (float)i / (float)(musical::CHUNK_SIZE - 1)), 2.0f);
		}
	}
} hannwindowinit;