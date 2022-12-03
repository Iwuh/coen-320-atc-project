#include "Rada.h"
#include "ComputerSystem.h"
#include "Plane.h"
#include <sys/siginfo.h>
#include <iostream>


int Rada::getChid() const {
	return chid;
}



PlanePositionResponse Rada::pingPlane(Plane p)
{

			// Connect to plane's message passing channel.
			int coid = ConnectAttach(0, 0, Plane.getChid(), _NTO_SIDE_CHANNEL, 0);

			// Ping the plane
			PlaneCommandMessage msg;
			msg.command = COMMAND_RADAR_PING;
			PlanePositionResponse res;
			MsgSend(coid, &msg, sizeof(msg), &res, sizeof(res));
			return res;



}

std::vector<PlanePositionResponse> Rada::pingMultiplePlanes(std::vector<Plane&> planes)
{




			std::vector<PlanePositionResponse> responses;
			for (int i = 0; i < planes.size(); i++)
			{

				// Connect to plane's message passing channel.
				int coid = ConnectAttach(0, 0, planes[i].getChid(), _NTO_SIDE_CHANNEL, 0);

				// Ping the planes
				PlaneCommandMessage msg;
				msg.command = COMMAND_RADAR_PING;
				PlanePositionResponse res;
				MsgSend(coid, &msg, sizeof(msg), &res, sizeof(res));
				return res;

				responses.push_back(pingPlane(Planes[i]));

				}
			return responses;

}
