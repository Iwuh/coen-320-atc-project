#include "Stopwatch.h"

#include <time.h>
#include <iostream>

void Stopwatch::start() {
	struct timespec now;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
	startTime = now.tv_sec*1000000000L + now.tv_nsec;
}

int64_t Stopwatch::stop() {
	struct timespec now;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
	return now.tv_sec*1000000000L + now.tv_nsec - startTime;
}
