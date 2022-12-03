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
#include <queue>
#include <pthread.h>
#include <vector>
#include <string>
#include "Plane.h"

#define OPCON_CONSOLE_COMMAND_GET_USER_COMMAND 1
#define OPCON_CONSOLE_COMMAND_EXIT_THREAD 2

#define OPCON_USER_COMMAND_NO_COMMAND_AVAILABLE 1
#define OPCON_USER_COMMAND_DISPLAY_PLANE_INFO 2
#define OPCON_USER_COMMAND_SET_PLANE_VELOCITY 3

#define OPCON_COMMAND_STRING_SHOW_PLANE "showplane"
#define OPCON_COMMAND_STRING_SET_VELOCITY "setvelocity"

#define OPCON_CODE_TIMER 1

typedef struct {
	int systemCommandType;
} OperatorConsoleCommandMessage;

typedef struct {
	int userCommandType;
	int planeNumber;
	Vec3 newVelocity;
} OperatorConsoleResponseMessage;

class OperatorConsole {
public:
	OperatorConsole();
	int getChid() const;

private:
	void run();
	void listen();

	int chid;

public:
	static void* start(void *context);
private:
	static pthread_mutex_t mutex;
	static std::queue<OperatorConsoleResponseMessage> responseQueue;
	static void* cinRead(void *param);
	static void tokenize(std::vector<std::string> &dest, std::string &str);
};

#endif /* SRC_OPERATORCONSOLE_H_ */
