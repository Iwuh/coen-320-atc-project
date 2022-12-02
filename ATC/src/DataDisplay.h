/*
 * DataDisplay.h
 *
 *  Created on: Nov. 16, 2022
 *      Author: coen320
 */

#ifndef SRC_DATADISPLAY_H_
#define SRC_DATADISPLAY_H_

#include "Plane.h"

#define COMMAND_ONE_PLANE 1
#define COMMAND_MULTIPLE_PLANE 2
#define COMMAND_WARNING 3
#define COMMAND_GRID 4


typedef struct //for one aircraft
{
	int aircraftID;
	Vec3 position; //can read flight level / altitude from z component of position vector
	Vec3 velocity;
} oneAircraftDisplay;

typedef struct //for displaying multiple aircrafts
{
	size_t numberOfAircrafts;
	Vec3* positionArray;
	Vec3* velocityArray;
	int* planeIDArray;
} multipleAircraftDisplay;

typedef struct
{
	int commandType; //defining which of union element it is
	union commandBody
	{
		oneAircraftDisplay one;
		multipleAircraftDisplay mutiple;
		string warning;
	};
} dataDisplayCommandMessage;

class DataDisplay {
private:
	void run(); //don't need timers code
	void receiveMessage();

	int chid;

	Vec3 airspace[50][50]; //creating a 50 by 50 airspace to display

public:
	DataDisplay();
	virtual ~DataDisplay();

	void getChid() const;
	static void* start(void* context);


};

#endif /* SRC_DATADISPLAY_H_ */
