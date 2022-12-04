/*
 * InputStrings.h
 *
 *  Created on: Dec. 4, 2022
 *      Author: matt
 */

#ifndef SRC_INPUTSTRINGS_H_
#define SRC_INPUTSTRINGS_H_

// Time ID PX PY PZ VX VY VZ

constexpr auto LOW_LOAD = "1 1 0 50000 20000 1000 0 0\n"
		"1 2 50000 0 20000 0 1000 0\n"
		"5 3 0 0 15000 250 250 500\n";

constexpr auto MED_LOAD = "1 1 0 50000 20000 1000 0 0\n"
		"1 2 50000 0 20000 0 1000 0\n"
		"5 3 0 0 15000 250 250 500\n"
		"5 3 0 0 15000 250 250 500\n"
		"5 3 0 0 15000 250 250 500\n"
		"5 3 0 0 15000 250 250 500\n";

constexpr auto HIGH_LOAD = "";

#endif /* SRC_INPUTSTRINGS_H_ */
