/*
 * OperatorConsole.h
 *
 *  Created on: Nov. 23, 2022
 *      Author: Matthew Faigan
 */

#ifndef SRC_OPERATORCONSOLE_H_
#define SRC_OPERATORCONSOLE_H_

#include <stdlib.h>
#include <sys/neutrino.h>
#include "Plane.h"

#define OPCON_CONSOLE_COMMAND_GET_USER_COMMAND 1
#define OPCON_CONSOLE_COMMAND_EXIT_THREAD 2

#define OPCON_USER_COMMAND_NO_COMMAND_AVAILABLE 1
#define OPCON_USER_COMMAND_DISPLAY_PLANE_INFO 2
#define OPCON_USER_COMMAND_SET_PLANE_VELOCITY 3

#define OPCON_CODE_TIMER 1

typedef struct
{
	struct _pulse header;
	int query;
} OperatorConsoleCommandMessage;

typedef struct
{
	int userCommandType;
	union commandParams {
		int planeNumber;
		Vec3 newVelocity;
	};
} OperatorConsoleResponseMessage;

class OperatorConsole {
public:
	int getChid() const;

private:
	void run();
	void listen();

	int chid;

public:
	static void* start(void* context);
};

#endif /* SRC_OPERATORCONSOLE_H_ */
