#include <iostream>
#include <signal.h>
#include "Plane.h"

int main() {

	PlaneStartParams params;
	params.arrivalTime = 10;
	params.id = 1;
	params.initialPosition = {1,1,1};
	params.initialVelocity = {1,1,1};

	Plane myPlane(params);
	pthread_t tid;
	pthread_create(&tid, NULL, &Plane::start, &myPlane);
	return 0;
}
