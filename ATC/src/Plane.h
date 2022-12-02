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
#include "commandCodes.h"
#include <string>
#include <iostream>
#include <cmath>

using namespace std;

using std::string;
// How often the plane should update its position.
#define POSITION_UPDATE_INTERVAL_SECONDS 1
// Used internally to identify when the position update timer has fired.
#define CODE_TIMER 1


struct Vec3
{
	Vec3 sum(Vec3 b){
		return {x+b.x, y+b.y, z+b.z};
	}
	Vec3 diff(Vec3 b){
		return {x-b.x,y-b.y,z-b.z};
	}
	// cross product between 2 3d vectors is
	// A X B = x(AyBz - AzBy) + y(AzBx - AxBz) + z(AxBy - AyBx)
	Vec3 cross(Vec3 b){
		float x1 = y*b.z - z*b.y;
		float y1 = z*b.x - x*b.z;
		float z1 = x*b.y - y*b.x;
		return {x1,y1,z1};
	}
	float dot(Vec3 b){
		return x * b.x +y * b.y + z * b.z;
	}
	bool equals(Vec3 b){
		return (x == b.x && y == b.y && z == b.z);
	}
	string print(){
		return std::to_string(x) + "," + std::to_string(y) + "," +std::to_string(z);
	}
	Vec3 scalarMultiplication(float scalarMultiplier){
		return {x*scalarMultiplier, y*scalarMultiplier, z*scalarMultiplier};
	}
	float magnitude(){
		return sqrt(pow(x,2)+pow(y,2)+pow(z,2));
	}
	float x;
	float y;
	float z;
};

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
	Vec3 newVelocity;
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
