#include <sys/neutrino.h>
#include <iostream>
#include <map>
#include "Plane.h"

using namespace std;
typedef struct {
	struct _pulse header;
	int command;
} ComputerSystemMessage;

typedef struct {
	int timerCode;
	int taskIntervalSeconds;
} periodicTask;

class ComputerSystem {
public:
	ComputerSystem();
	int getChid() const;
	void setOperatorChid(int id);
	void setRadarChid(int id);
	void setDisplayChid(int id);
private:
	void run();
	void listen();
	void createPeriodicTasks();
	void logSystem();
	void violationCheck();
	void printCurrentTime();
	int chid;
	int operatorChid;
	int radarChid;
	int displayChid;
	std::map<int, PlanePositionResponse> airspace;
public:
	static void* start(void *context);
};
