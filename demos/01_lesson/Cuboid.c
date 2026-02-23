#include "cuboid.h"
#include <math.h>

void set_size(Cuboid* cuboid, double length, double width, double height)
{
	if (length > 0.0) {
		cuboid->length = length;
	} else {
		cuboid->length = NAN;
	}
	
	if (width > 0.0) {
		cuboid->width = width;
	} else {
		cuboid->width = NAN;
	}
	
	if (height > 0.0) {
		cuboid->height = height;
	} else {
		cuboid->height = NAN;
	}
}

double calc_volume(const Cuboid* cuboid)
{
	double volume = cuboid->length * cuboid->width * cuboid->height;
	return volume;
}

double calc_surface(const Cuboid* cuboid)
{
	double surface = 2.0 * (cuboid->length * cuboid->width + cuboid->length * cuboid->height + cuboid->width * cuboid->height);
	return surface;
}

bool has_square_face(const Cuboid* cuboid)
{
	const double EPSILON = 1e-9;
	
	if (fabs(cuboid->length - cuboid->width) < EPSILON) {
		return true;
	}
	
	if (fabs(cuboid->length - cuboid->height) < EPSILON) {
		return true;
	}
	
	if (fabs(cuboid->width - cuboid->height) < EPSILON) {
		return true;
	}
	
	return false;
}
