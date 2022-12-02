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
	void setCongestionDegreeSeconds(int congestionDegreeSeconds);
private:
	void run();
	void listen();
	void createPeriodicTasks();
	void logSystem();
	void violationCheck();
	void checkForFutureViolation(std::pair<int, PlanePositionResponse> plane1, std::pair<int, PlanePositionResponse> plane2);
	int chid;
	int operatorChid;
	int radarChid;
	int displayChid;
	Vec3 airspaceBounds;
	int congestionDegreeSeconds;
	std::map<int, PlanePositionResponse> airspace;
public:
	static void* start(void *context);
};
