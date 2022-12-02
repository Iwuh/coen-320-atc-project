/*
 * CommunicationSystem.h
 *
 *  Created on: Nov. 24, 2022
 *      Author: coen320
 */

#ifndef SRC_COMMUNICATIONSYSTEM_H_
#define SRC_COMMUNICATIONSYSTEM_H_

//TODO: Define types for command messages

#include "Plane.h"

#include <sys/neutrino.h>


class CommunicationSystem {
public:
	CommunicationSystem();
	virtual ~CommunicationSystem();

private:

	bool send(Plane R, Vec3 newVelocity); //returns true if sending message is successful and false if not
};

#endif /* SRC_COMMUNICATIONSYSTEM_H_ */
