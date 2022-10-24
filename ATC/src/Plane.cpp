/*
 * Plane.cpp
 *
 *  Created on: Oct. 24, 2022
 *      Author: Matthew Faigan
 */

#include "Plane.h"

Plane::Plane(PlaneStartParams &params)
	: startParams(params)
{}

void Plane::Run()
{
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
