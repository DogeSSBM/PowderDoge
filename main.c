#include "Includes.h"

int main(int argc, char const *argv[])
{

	const Length window = coordMul(grid, SCALE);
	init(window);

	while(1){
		Ticks frameStart = getTicks();
		clear();



		draw();
		events(frameStart + TPF);
	}
	return 0;
}
