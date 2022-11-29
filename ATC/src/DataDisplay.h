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


//typedef struct {
//	int type; //TODO: define type/value for each incoming message to know what it is (ex: id, velocity, position, etc.)
//	union {
//		double flightLevel;
//		double speed;
//		double position;
//		void dataDisplaying(int type);
//	};
//} dataDisplay;

class DataDisplay {
private:
	void run(); //don't need timers code
	void receiveMessage();

	int chid;
public:
	DataDisplay();
	virtual ~DataDisplay();

	void getChid() const;
	static void* start(void* context);


};

#endif /* SRC_DATADISPLAY_H_ */
