/*
 * OperatorConsole.cpp
 *
 *  Created on: Nov. 23, 2022
 *      Author: Matthew Faigan
 */

#include "OperatorConsole.h"

void OperatorConsole::run()
{

}

void* OperatorConsole::start(void* context)
{
	auto c = (OperatorConsole*)context;
	c->run();
	return NULL;
}
