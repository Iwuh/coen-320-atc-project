#include "Radar.h"

#include <sys/siginfo.h>

Radar::Radar() {

}
Radar::Radar(std::vector<Plane> &planes) :
		planes(planes) {
}

bool Radar::pingPlane(int planeNumber, PlanePositionResponse *out) {
	cout << planes.size() << endl;
	for (size_t i = 0; i < planes.size(); i++) {
		if (planes[i].getPlaneId() == planeNumber) {
			*out = pingPlane(planes[i]);
			return true;
		}
	}
	return false;
}
std::vector<pair<int, PlanePositionResponse>> Radar::pingAirspace() {
	constexpr Vec3 NOT_IN_AIRSPACE { -1, -1, -1 };
	std::vector<pair<int, PlanePositionResponse>> vec;
	for (size_t i = 0; i < planes.size(); i++) {
		PlanePositionResponse res = pingPlane(planes[i]);
		if (res.currentPosition != NOT_IN_AIRSPACE) {
			vec.push_back(std::make_pair(planes[i].getPlaneId(), res));
		}
	}
	return vec;
}

PlanePositionResponse Radar::pingPlane(Plane &p) {
	// Connect to plane's message passing channel.
	int coid = ConnectAttach(0, 0, p.getChid(), _NTO_SIDE_CHANNEL, 0);

	// Ping the plane
	PlaneCommandMessage msg;
	msg.command = COMMAND_RADAR_PING;
	PlanePositionResponse res;
	MsgSend(coid, &msg, sizeof(msg), &res, sizeof(res));
	return res;
}

std::vector<PlanePositionResponse> Radar::pingMultiplePlanes(
		std::vector<Plane> &planes) {
	std::vector<PlanePositionResponse> responses;
	for (size_t i = 0; i < planes.size(); i++) {
		// Connect to plane's message passing channel.
		int coid = ConnectAttach(0, 0, planes[i].getChid(), _NTO_SIDE_CHANNEL,
				0);

		// Ping the planes
		PlaneCommandMessage msg;
		msg.command = COMMAND_RADAR_PING;
		PlanePositionResponse res;
		MsgSend(coid, &msg, sizeof(msg), &res, sizeof(res));

		ConnectDetach(coid);
		responses.push_back(res);

	}
	return responses;
}
