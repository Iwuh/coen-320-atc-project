/*
 * CommunicationSystem.cpp
 *
 *  Created on: Nov. 24, 2022
 *      Author: coen320
 */

#include "CommunicationSystem.h"
#include "commandCodes.h"

CommunicationSystem::CommunicationSystem() {
	// TODO Auto-generated constructor stub

}

CommunicationSystem::~CommunicationSystem() {
	// TODO Auto-generated destructor stub
}

bool CommunicationSystem::send(Plane R, Vec3 newVelocity) {
	int planeChid = R.getChid(); //for sending to correct plane

	int sndid; //send id

	int coid; //client id
	if ((coid = ConnectAttach(0,0,planeChid,0,0)) == -1) {
		std::cout <<"client connection failed. Exiting thread" << std::endl;
		return false;
	}

	PlaneCommandMessage msg; //message of the plane class
	msg.command = COMMAND_SET_VELOCITY; //determining the type
	msg.newVelocity = newVelocity; //setting the new velocity

	sndid = MsgSend(coid, &msg, sizeof(msg), NULL, 0); //NULL & 0 since not expecting a reply

	if (sndid == -1) {
		std::cout <<"Message failed to send!" <<std::endl;
		return false;
	}

	return true;

}