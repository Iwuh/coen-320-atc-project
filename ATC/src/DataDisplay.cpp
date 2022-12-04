/*
 * DataDisplay.cpp
 *
 *  Created on: Nov. 16, 2022
 *      Author: coen320
 */

#include "DataDisplay.h"
#include <sys/neutrino.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sstream>
#include "commandCodes.h"

DataDisplay::DataDisplay()
	: chid(-1), fd(-1){
}

int DataDisplay::getChid() const {
	return chid;
}

void DataDisplay::run() {
	if ((chid = ChannelCreate(0)) == -1) {
		std::cout << "channel creation failed. Exiting thread." << std::endl;
		return;
	}

	// Open log file
	fd = creat("/data/home/qnxuser/airspacelog.txt",
				S_IRUSR | S_IWUSR | S_IXUSR);
	if (fd == -1) {
		std::cout << "DataDisplay: " << "Failed to open logfile. Errno is "
				<< errno << std::endl;
	}

	receiveMessage(); //start to listen for messages

	// Close log file
	if (fd != -1) {
			close(fd);
	}
}

void DataDisplay::receiveMessage() {
	int rcvid; //receive id
	dataDisplayCommandMessage msg;
	while (1) {

		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

		switch (msg.commandType) {
		case COMMAND_ONE_PLANE: {
			MsgReply(rcvid, EOK, NULL, 0); //sending basic ACK
			std::cout << "Aircraft ID: " << msg.commandBody.one.aircraftID
					<< "  " << "Aircraft position: "
					<< msg.commandBody.one.position << "  "
					<< "Aircraft velocity " << msg.commandBody.one.velocity
					<< std::endl;
			break;
		}
		case COMMAND_MULTIPLE_PLANE: {
			MsgReply(rcvid, EOK, NULL, 0);
			for (size_t i = 0; i < msg.commandBody.multiple.numberOfAircrafts;
					i++) {
				//std::cout <<"Aircraft positions: " <<msg.CommandBody.multiple->positionArray <<"  " <<"Aircraft velocities: " <<msg.commandBody.multiple->velocityArray <<std::endl;
				std::cout << "Aircraft " << i + 1 << " with position: "
						<< msg.commandBody.multiple.positionArray[i]
						<< " and velocity: "
						<< msg.commandBody.multiple.velocityArray[i]
						<< std::endl;
			}
			//TODO: print data for multiple aircrafts
			break;
		}
		case COMMAND_WARNING: {
			MsgReply(rcvid, EOK, NULL, 0);
			std::cout << "Airspace separation constraint of aircraft with ID: "
					<< msg.commandBody.one.aircraftID << " and position: "
					<< msg.commandBody.one.position << std::endl;
			break;
		}
		case COMMAND_GRID: //ignoring z-axis, doing x and y (top-view)
		{
			MsgReply(rcvid, EOK, NULL, 0);
			std::cout << generateGrid(msg.commandBody.multiple) << std::endl;
			break;
		}
		case COMMAND_LOG:
		{
			MsgReply(rcvid, EOK, NULL, 0);
			std::string grid = generateGrid(msg.commandBody.multiple);
			if (fd != -1) {
				char* buffer = new char[grid.length() + 1];
				strncpy(buffer, grid.c_str(), grid.length() + 1);
				write(fd, buffer, grid.length() + 1);
				write(fd, "\n", 1);
				delete[] buffer;
			} else {
				std::cout << "DataDisplay: Received a log command but the log file is not opened." << std::endl;
			}
			break;
		}
		case COMMAND_EXIT_THREAD:
			MsgReply(rcvid, EOK, NULL, 0);
			return;
		}
	}
}

std::string DataDisplay::generateGrid(multipleAircraftDisplay &airspaceInfo)
{
	int rowSize = 100;
	int columnSize = 100;
	int cellSize = 1000;

	std::string grid[rowSize][columnSize]; //grid 100000ft x 100000ft with each square being 1000ft
	//storing into grid
	for (size_t i = 0; i < airspaceInfo.numberOfAircrafts;
			i++) {
		for (int j = 0; j < rowSize; j++) {
			if (airspaceInfo.positionArray[i].y
					>= (cellSize * j)
					&& airspaceInfo.positionArray[i].y
							< (cellSize * (j + 1))) { //checking y
				for (int k = 0; k < columnSize; k++) {
					//only for x
					if (airspaceInfo.positionArray[i].x
							>= (cellSize * k)
							&& airspaceInfo.positionArray[i].x
									< (cellSize * (k + 1))) { // if plane in row i is between 0 and 20 not included, add to string
						if (grid[j][k] != "") {
							grid[j][k] += ",";
						}
						grid[j][k] +=
								airspaceInfo.planeIDArray[i];
					}
				}
			}
		}
	}
	//printing grid
	std::stringstream output;
	for (int i = 0; i < rowSize; i++) {
		output << std::endl;
		for (int j = 0; j < columnSize; j++) {
			if (grid[i][j] == "") {
				output << "| ";
			} else {
				output << "|" + grid[i][j];
			}
		}
	}
	return output.str();
}

void* DataDisplay::start(void *context) {
	auto p = (DataDisplay*) context;
	p->run();
	return NULL;
}
