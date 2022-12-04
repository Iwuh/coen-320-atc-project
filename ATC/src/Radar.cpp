#include "Radar.h"

#include <sys/siginfo.h>

PlanePositionResponse Radar::pingPlane(Plane &p)
{
	// Connect to plane's message passing channel.
	int coid = ConnectAttach(0, 0, p.getChid(), _NTO_SIDE_CHANNEL, 0);

	// Ping the plane
	PlaneCommandMessage msg;
	msg.command = COMMAND_RADAR_PING;
	PlanePositionResponse res;
	MsgSend(coid, &msg, sizeof(msg), &res, sizeof(res));
	return res;
}

std::vector<PlanePositionResponse> Radar::pingMultiplePlanes(std::vector<Plane> &planes)
{
	std::vector<PlanePositionResponse> responses;
	for (size_t i = 0; i < planes.size(); i++)
	{
		// Connect to plane's message passing channel.
		int coid = ConnectAttach(0, 0, planes[i].getChid(), _NTO_SIDE_CHANNEL, 0);

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
