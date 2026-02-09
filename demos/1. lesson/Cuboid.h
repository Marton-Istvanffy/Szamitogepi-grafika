#ifndef CUBOID_H
#define CUBOID_H

#include <stdbool.h>


typedef struct Cuboid
{
	double length;  
	double width;   
	double height; 
} Cuboid;

void set_size(Cuboid* cuboid, double length, double width, double height);

double calc_volume(const Cuboid* cuboid);

double calc_surface(const Cuboid* cuboid);

bool has_square_face(const Cuboid* cuboid);

#endif 