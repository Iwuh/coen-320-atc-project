/*
 * Plane.h
 *
 *  Created on: Oct. 24, 2022
 *      Author: Matthew Faigan
 */

#ifndef SRC_PLANE_H_
#define SRC_PLANE_H_

#include <stdlib.h>

typedef struct
{
	int x;
	int y;
	int z;
} Vec3;

typedef struct
{
	int id;
	int arrivalTime;
	Vec3 initialPosition;
	Vec3 initialVelocity;
} PlaneStartParams;

class Plane {

private:
	Plane(PlaneStartParams& params);

	void Run();

	PlaneStartParams startParams;

public:
	// Create a new instance of the Plane class with the specified parameters.
	static Plane* CreateWithParams(PlaneStartParams& params);

	// Thread host function to initialize the plane. Use as target for pthread_create.
	static void* Start(void* context);
};

#endif /* SRC_PLANE_H_ */
