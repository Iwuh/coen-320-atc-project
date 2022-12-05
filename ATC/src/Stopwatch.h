#ifndef SRC_STOPWATCH_H_
#define SRC_STOPWATCH_H_

#include <stdint.h>
#include <time.h>

class Stopwatch {
public:
	void start();
	int64_t stop();
private:
	int64_t startTime;
};

#endif /* SRC_STOPWATCH_H_ */
