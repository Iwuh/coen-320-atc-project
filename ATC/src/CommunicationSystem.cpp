/*
 * CommunicationSystem.cpp
 *
 *  Created on: Nov. 24, 2022
 *      Author: coen320
 */

#include "CommunicationSystem.h"
#include "commandCodes.h"

CommunicationSystem::CommunicationSystem() {
}
CommunicationSystem::CommunicationSystem(std::vector<Plane> &planes) :
		planes(planes) {

}

bool CommunicationSystem::send(int planeNumber, Vec3 &newVelocity) {
	for (size_t i = 0; i < planes.size(); i++) {
		if (planes[i].getPlaneId() == planeNumber) {
			return send(planes[i], newVelocity);
		}
	}
	return false;
}

bool CommunicationSystem::send(Plane &R, Vec3 &newVelocity) {
	int planeChid = R.getChid(); //for sending to correct plane

	int sndid; //send id

	int coid; //client id
	if ((coid = ConnectAttach(0, 0, planeChid, 0, 0)) == -1) {
		std::cout << "client connection failed. Exiting thread" << std::endl;
		return false;
	}

	PlaneCommandMessage msg; //message of the plane class
	msg.command = COMMAND_SET_VELOCITY; //determining the type
	msg.newVelocity = newVelocity; //setting the new velocity

	sndid = MsgSend(coid, &msg, sizeof(msg), NULL, 0); //NULL & 0 since not expecting a reply
	ConnectDetach(coid);
	if (sndid == -1) {
		std::cout << "Message failed to send!" << std::endl;
		return false;
	}

	return true;

}
