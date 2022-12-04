

#ifndef SRC_RADAR_H_
#define SRC_RADAR_H_

#include <vector>

#include "Plane.h"

class Radar {
public:
	PlanePositionResponse pingPlane(Plane &p);
	std::vector<PlanePositionResponse> pingMultiplePlanes(std::vector<Plane> &planes);
};

#endif /* SRC_RADAR_H_ */

