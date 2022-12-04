

#ifndef SRC_RADAR_H_
#define SRC_RADAR_H_

#include <stdlib.h>
#include <sys/neutrino.h>
#include <iostream>

#include "Plane.h"



class Radar {

public:

	int getChid() const;



	PlanePositionResponse pingPlane(Plane &p);
	planePositionResponse pingMultiplePlanes(std::vector<Plane&> planes);

private:
	int chid;

};

#endif /* SRC_RADAR_H_ */

