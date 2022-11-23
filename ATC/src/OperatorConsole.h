/*
 * OperatorConsole.h
 *
 *  Created on: Nov. 23, 2022
 *      Author: Matthew Faigan
 */

#ifndef SRC_OPERATORCONSOLE_H_
#define SRC_OPERATORCONSOLE_H_

class OperatorConsole {
private:
	void run();

public:
	static void* start(void* context);
};

#endif /* SRC_OPERATORCONSOLE_H_ */
