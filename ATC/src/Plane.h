/*
 * Plane.h
 *
 *  Created on: Oct. 24, 2022
 *      Author: Matthew Faigan
 */

#ifndef SRC_PLANE_H_
#define SRC_PLANE_H_

#include <stdlib.h>
#include <sys/neutrino.h>

// Command to have the plane reply with its current position and velocity.
#define COMMAND_RADAR_PING 1
// Command to have the plane thread terminate.
#define COMMAND_EXIT_THREAD 2

// How often the plane should update its position.
#define POSITION_UPDATE_INTERVAL_SECONDS 1
// Used internally to identify when the position update timer has fired.
#define CODE_TIMER 1

typedef struct
{
	int x;
	int y;
	int z;
} Vec3;

typedef struct
{
	int id;
	int arrivalTime;
	Vec3 initialPosition;
	Vec3 initialVelocity;
} PlaneStartParams;

typedef struct
{
	struct _pulse header;
	int command;
} PlaneCommandMessage;

typedef struct
{
	Vec3 currentPosition;
	Vec3 currentVelocity;
} PlanePositionResponse;

class Plane
{
public:
	Plane(PlaneStartParams& params);
	int getChid() const;

private:
	void run();
	void listen();
	void updatePosition();

	PlaneStartParams startParams;
	Vec3 currentPosition;
	Vec3 currentVelocity;
	bool arrived;
	int chid;

public:
	// Thread host function to initialize the plane. Use as target for pthread_create.
	static void* start(void* context);
};

#endif /* SRC_PLANE_H_ */
