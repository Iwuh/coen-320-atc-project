/*
 * Plane.cpp
 *
 *  Created on: Oct. 24, 2022
 *      Author: Matthew Faigan
 */

#include "Plane.h"
#include <iostream>
using namespace std;

Plane::Plane(PlaneStartParams &params)
	: startParams(params)
{}

void Plane::Run()
{
	cout << startParams.arrivalTime;
}

Plane* Plane::CreateWithParams(PlaneStartParams &params)
{
	return new Plane(params);
}

void* Plane::Start(void *context)
{
	auto p = (Plane*)context;
	p->Run();
	return NULL;
}

Vec3 Plane::getVelocity()
{
	return startParams.initialVelocity;
}

int Plane::getId(){
	return startParams.id;
}
