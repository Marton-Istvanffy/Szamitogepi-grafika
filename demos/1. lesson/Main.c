#include "cuboid.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
	Cuboid cuboid;
	double volume, surface;
	bool has_square;
	
	printf("=== Első téglatest ===\n");
	set_size(&cuboid, 5.0, 8.0, 10.0);
	volume = calc_volume(&cuboid);
	surface = calc_surface(&cuboid);
	has_square = has_square_face(&cuboid);
	
	printf("Élek: %.2lf x %.2lf x %.2lf\n", cuboid.length, cuboid.width, cuboid.height);
	printf("Térfogat: %.2lf\n", volume);
	printf("Felszín: %.2lf\n", surface);
	printf("Van négyzet alakú lapja: %s\n\n", has_square ? "igen" : "nem");
	
	printf("=== Második téglatest ===\n");
	set_size(&cuboid, 6.0, 6.0, 10.0);
	volume = calc_volume(&cuboid);
	surface = calc_surface(&cuboid);
	has_square = has_square_face(&cuboid);
	
	printf("Élek: %.2lf x %.2lf x %.2lf\n", cuboid.length, cuboid.width, cuboid.height);
	printf("Térfogat: %.2lf\n", volume);
	printf("Felszín: %.2lf\n", surface);
	printf("Van négyzet alakú lapja: %s\n\n", has_square ? "igen" : "nem");
	
	printf("=== Harmadik téglatest (kocka) ===\n");
	set_size(&cuboid, 7.0, 7.0, 7.0);
	volume = calc_volume(&cuboid);
	surface = calc_surface(&cuboid);
	has_square = has_square_face(&cuboid);
	
	printf("Élek: %.2lf x %.2lf x %.2lf\n", cuboid.length, cuboid.width, cuboid.height);
	printf("Térfogat: %.2lf\n", volume);
	printf("Felszín: %.2lf\n", surface);
	printf("Van négyzet alakú lapja: %s\n", has_square ? "igen" : "nem");
	
	return 0;
}