#include <map>
#include "Plane.h"

class Radar {
public:
	Radar();
	int getChid() const;
	void run();
	void listen();
	static void* start(void *context);
	void addPlaneToAirspace(int, PlanePositionResponse);

private:
	std::map<int, PlanePositionResponse> radarResults;
	int chid;
};
