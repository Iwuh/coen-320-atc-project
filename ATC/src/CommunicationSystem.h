/*
 * CommunicationSystem.h
 *
 *  Created on: Nov. 24, 2022
 *      Author: coen320
 */

#ifndef SRC_COMMUNICATIONSYSTEM_H_
#define SRC_COMMUNICATIONSYSTEM_H_

#include "Plane.h"
#include <vector>

class CommunicationSystem {
public:
	CommunicationSystem(std::vector<Plane> &planes);

	bool send(Plane &R, Vec3 &newVelocity); //returns true if sending message is successful and false if not
	bool send(int &planeNumber, Vec3 &newVelocity);

private:
	std::vector<Plane> planes;
};

#endif /* SRC_COMMUNICATIONSYSTEM_H_ */
