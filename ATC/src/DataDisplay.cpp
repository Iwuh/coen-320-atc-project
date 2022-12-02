/*
 * DataDisplay.cpp
 *
 *  Created on: Nov. 16, 2022
 *      Author: coen320
 */

#include "DataDisplay.h"
#include <sys/neutrino.h>

DataDisplay::DataDisplay() {
	// TODO Auto-generated constructor stub
	chid = -1;
}

DataDisplay::~DataDisplay() {
	// TODO Auto-generated destructor stub
}

DataDisplay::GraphicalPlanes() {
}

int DataDisplay::getChid() const {
	return chid;
}

void DataDisplay::run() {
	if (chid = ChannelCreate(0) == -1) {
		std::cout <<"channel creation failed. Exiting thread." <<std::endl;
		return;
	}


}

void DataDisplay::receiveMessage() {
	int rcvid; //receive id
	dataDisplayCommandMessage msg;
	while(1) {

		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

		switch(msg.commandType) {
		case COMMAND_ONE_PLANE:
		{
			MsgReply(rcvid, EOK, NULL, 0); //sending basic ACK
			std::cout <<"Aircraft ID: " << msg.CommandBody.one.aircraftID <<"  " <<"Aircraft position: " << msg.commandBody.one.position <<"  " <<"Aircraft velocity " << msg.commandBody.one.velocity <<std::endl;
			break;
		}
		case COMMAND_MULTIPLE_PLANE:
		{
			MsgReply(rcvid, EOK, NULL, 0);
			for (int i = 0; i < msg.CommandBody.multiple.numberOfAircrafts; i++) {
			//std::cout <<"Aircraft positions: " <<msg.CommandBody.multiple->positionArray <<"  " <<"Aircraft velocities: " <<msg.commandBody.multiple->velocityArray <<std::endl;
				std::cout <<"Aircraft " <<i + 1 <<" with position: " <<msg.CommandBody.multiple.positionArray[i] <<" and velocity: " <<msg.CommandBody.multiple.velocity[i] <<std:endl;
			}
			//TODO: print data for multiple aircrafts
			break;
		}
		case COMMAND_WARNING:
		{
			MsgReply(rcvid, EOK, NULL, 0);
			std::cout <<"Airspace separation constraint of aircraft with ID: " <<msg.commandType.one.aircraftID <<" and position: " <<msg.commandType.one.position <<std::endl;
			break;
		}
		case COMMAND_GRID: //ignoring z-axis, doing x and y (top-view)
		{
			MsgReply(rcvid, EOK, NULL, 0);

			int rowSize = 5;
			int columnSize = 10;
			int cellSize = 20;

			string grid[rowSize][columnSize]; //grid 100m x 200m with each square being 20m
			//storing into grid
			for (int i = 0; i < msg.commandBody.multiple.numberOfAircrafts; i++) {
				for (int j = 0; j < rowSize; j++) {
					if (msg.commandBody.mutiple.positionArray[i].y >= (cellSize * j)  && msg.commandBody.mutiple.positionArray[i].y < (cellSize * (j + 1))) { //checking y
						for (int k = 0; k < columnSize; k++) {
							//only for x
							if (msg.commandBody.mutiple.positionArray[i].x >= (cellSize * k)  && msg.commandBody.mutiple.positionArray[i].x < (cellSize * (k + 1))) { // if plane in row i is between 0 and 20 not included, add to string
								grid[j][k] += msg.commandBody.mutiple.planeIDArray[i] + ",";
							}
						}
					}
				}
			}

			for (int i = 0; i < rowSize; i++) {
				cout <<std::endl;
				for (int j = 0; j < columnSize; j++) {
					cout <<"| " + grid[i][j] + " ";
				}
			}

			//print grid
			break;
		}
		}
	}
}


void* DataDisplay::start(void *context) {
	auto p = (DataDisplay*)context;
	p->run();
	return NULL;
}
