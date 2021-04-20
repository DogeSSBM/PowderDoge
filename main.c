#include "Includes.h"
#define DEFAULTX		800
#define DEFAULTY		600
#define DEFAULTLEN	(DEFAULTX*DEFAULTY)

typedef enum{P_VOID, P_SOLID, P_WATER}Particle;

uint lin(const Length len, const uint x, const uint y)
{
	return clamp(y, 0, len.y) * len.x + clamp(x, 0, len.x);
}

uint linCoord(const Length len, const Coord coord)
{
	return clamp(coord.y, 0, len.y) * len.x + clamp(coord.x, 0, len.x);
}

Coord delin(const Length len, const uint lin)
{
	return (Coord){lin%len.x, lin/len.x};
}

void initGrid(const Length len, Particle *const grid)
{
	for(uint i = 0; i < len.x; i++){
		grid[linCoord(len, (Coord){i, 0})]=P_SOLID;
		grid[linCoord(len, (Coord){i, len.y-1})]=P_SOLID;
	}
	for(uint i = 1; i < len.y-1; i++){
		grid[linCoord(len, (Coord){0,i})]=P_SOLID;
		grid[linCoord(len, (Coord){len.x-1,i})]=P_SOLID;
	}
}

void downInPlace(const Length len, Particle *const grid)
{
	for(uint y = len.y-2; y > 1; y--){
		for(uint x = 1; x < len.x-1; x++){
			const uint lpos = lin(len, x ,y);
			const uint dpos = lin(len, x, y + 1);
			if(grid[lpos] == P_WATER && grid[dpos] == P_VOID){
				grid[dpos] = P_WATER;
				grid[lpos] = P_VOID;
			}
		}
	}
}

void rInPlace(const Length len, Particle *const grid)
{
	for(uint x = len.x-2; x > 1; x--){
		for(uint y = 1; y < len.y-1; y++){
			const uint lpos = lin(len, x ,y);
			const uint dpos = lin(len, x, y + 1);
			const uint Rpos = lin(len, x + 1, y);
			if(grid[lpos] == P_WATER && grid[dpos] != P_VOID && grid[Rpos] == P_VOID && (rand() & 1)){
				grid[Rpos] = P_WATER;
				grid[lpos] = P_VOID;
			}
		}
	}
}

void lInPlace(const Length len, Particle *const grid)
{
	for(uint x = 1; x < len.x-1; x++){
		for(uint y = 1; y < len.y-1; y++){
			const uint lpos = lin(len, x ,y);
			const uint dpos = lin(len, x, y + 1);
			const uint Lpos = lin(len, x - 1, y);
			if(grid[lpos] == P_WATER && grid[dpos] != P_VOID && grid[Lpos] == P_VOID && (rand() & 1)){
				grid[Lpos] = P_WATER;
				grid[lpos] = P_VOID;
			}
		}
	}
}

void drawGrid(const Length len, Particle *const grid)
{
	for(uint y = 0; y < len.y; y++){
		for(uint x = 0; x < len.x; x++){
			switch (grid[lin(len,x,y)]) {
				case P_WATER:
					setColor(BLUE);
					break;
				case P_SOLID:
					setColor(GREY);
					break;
				default:
					continue;
					break;
			}
			drawPixel(x,y);
		}
	}
}

void brush(const Coord pos, const uint rad, const Length len, Particle *const grid, const Particle fill)
{
	printf("brush: %u, %u\n", pos.x, pos.y);
	for(int y = -rad; y < rad; y++){
		for(int x = -rad; x < rad; x++){
			grid[lin(len,x+pos.x,y+pos.y)] = fill;
		}
	}
}

int main(int argc, char const *argv[])
{
	Length len = {DEFAULTX, DEFAULTY};
	init(len);
	Particle *grid = calloc(len.x * len.y, sizeof(Particle));
	initGrid(len, grid);

	uint rad = 5;
	while(1){
		Ticks frameStart = getTicks();
		clear();

		if(mouseScrolled(MW_D)){
			rad = clamp(rad-1, 0, 100);
			printf("rad: %u\n", rad);
		}
		if(mouseScrolled(MW_U)){
			rad = clamp(rad+1, 0, 100);
			printf("rad: %u\n", rad);
		}

		if(mouseBtnState(MOUSE_L)){
			if(inBound(mouse.pos.x, 1, len.x-1)&&inBound(mouse.pos.y, 1, len.y-1)){
				brush(mouse.pos, rad, len, grid, P_WATER);
			}
		}

		if(mouseBtnState(MOUSE_M)){
			if(inBound(mouse.pos.x, 1, len.x-1)&&inBound(mouse.pos.y, 1, len.y-1)){
				brush(mouse.pos, rad, len, grid, P_SOLID);
			}
		}

		if(mouseBtnState(MOUSE_R)){
			if(inBound(mouse.pos.x, 1, len.x-1)&&inBound(mouse.pos.y, 1, len.y-1)){
				brush(mouse.pos, rad, len, grid, P_VOID);
			}
		}

		lInPlace(len, grid);
		rInPlace(len, grid);
		downInPlace(len, grid);
		drawGrid(len, grid);

		draw();
		events(frameStart + TPF);
	}
	return 0;
}
