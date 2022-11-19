#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dispatch.h>
#include <iostream>
#include <stdlib.h>
using namespace std;
/* This is the name that will be used by the server to create a channel and used by
 * the client to connect to the appropriated channel */
#define RADAR_COMM_CHANNEL "radar-computer_system"
#define DISPLAY_COMM_CHANNEL "display-computer_system"

/* We specify the header as being at least a pulse */
/* All of your messages should start with this header */
/* This struct contains a type/subtype field as the first 4 bytes.
 * The type and subtype field can be used to identify the message that is being received.
 / * This allows you to identify data which isn't destined for your server. */
typedef struct _pulse msg_header_t;

/* Our real data comes after the header */
typedef struct _my_data {
	msg_header_t hdr;
	int data;
} my_data_t;

int PLANE_LIST_UPDATE_REQUEST = 0x01;
int STOP_LISTENING = 0x99;
int NUM_PLANES = 4;

/*** Server Side of the code ***/
int waitForPlaneUpdateRequest(const char *attachId) {
	name_attach_t *attach;
	my_data_t msg;
	int rcvid;
	int planes[NUM_PLANES] = { 10, 20, 30, 40 };
	/* Create a local name (/dev/name/local/...) */
	if ((attach = name_attach(NULL, attachId, 0)) == NULL) {
		return EXIT_FAILURE;
	}

	while (true) {
		/* Do your MsgReceive's here now with the chid */
		/* Server will block in this call, until a client calls MsgSend to send a message to
		 * this server through the channel named "myname", which is the name that we set for the channel,
		 * i.e., the one that we stored at ATTACH_POINT and used in the name_attach call to create the channel. */
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

		/* In the above call, the received message will be stored at msg when the server receives a message.
		 * Moreover, rcvid */

		if (rcvid == -1) {/* Error condition, exit */
			return EXIT_FAILURE;
		}

		if (rcvid == 0) {/* Pulse received */
			switch (msg.hdr.code) {
			case _PULSE_CODE_DISCONNECT:
				/*
				 * A client disconnected all its connections (called
				 * name_close() for each name_open() of our name) or
				 * terminated
				 */
				ConnectDetach(msg.hdr.scoid);
				break;
			case _PULSE_CODE_UNBLOCK:
				/*
				 * REPLY blocked client wants to unblock (was hit by
				 * a signal or timed out).  It's up to you if you
				 * reply now or later.
				 */
				break;
			default:
				/*
				 * A pulse sent by one of your processes or a
				 * _PULSE_CODE_COIDDEATH or _PULSE_CODE_THREADDEATH
				 * from the kernel?
				 */
				break;
			}
		}

		/* name_open() sends a connect message, must EOK this */
		if (msg.hdr.type == _IO_CONNECT) {
			MsgReply(rcvid, EOK, NULL, 0);
		}

		/* Some other QNX IO message was received; reject it */
		if (msg.hdr.type > _IO_BASE && msg.hdr.type <= _IO_MAX) {
			MsgError(rcvid, ENOSYS);
		}

		/* Here are the messages that you will work on, i.e., messages that will have a meaning for
		 * your application. Let's assume that you have one message (e.g., data to be displayed) and several subtypes.
		 * Thus, we first test to check if it is a message we expect. Next, we can have a switch that check
		 * what is the subtype of the message. In your project, for instance, you can have a subtype for each
		 * variable, e.g., (0x01 - speed, 0x02 - temperature, 0x03 - gear, and so on...).
		 * Then, based on the subtype the server is receiving, it would display the information
		 * contained in msg.data in the proper place, e.g., at the proper location in a GUI.
		 * You can use that as well to work on the output your thread should provide.
		 *
		 * In addition, you might have another type of message. For instance, you might have a type of message
		 * that would be used for configuration (e.g., type==0x01 used for configuration and type==0x00 for data).
		 * This can be used to implement the mechanism to change the period of your consumer thread (server).
		 * For instance, let's assume that you implemented this server in a separate thread and, instead of having
		 * a forever loop, you implement the thread as a periodic task as we have seen (this is not necessary in this
		 * case because the server will block when waiting for a message, i.e., it will not be hogging CPU.). Then, the
		 * configuratin message could be used to send the period/frequency in which the client would be sending messages.
		 * Thus, the server could consider that while doing other things. */
		if (msg.hdr.type == 0x00) {
			if (msg.hdr.subtype == PLANE_LIST_UPDATE_REQUEST) {
				/* A message (presumable ours) received, handle */
				printf("Plane list update request received... \n");
				for (int i = 0; i < NUM_PLANES; i++) {
					planes[i] += 1;
				}
				MsgReply(rcvid, EOK, planes, sizeof(planes));
			}
			if (msg.hdr.subtype == STOP_LISTENING) {
				printf("Shutdown message comm channel request received... \n");
				MsgReply(rcvid, EOK, NULL, 0);
				break;
			}
		}
	}

	/* Remove the name from the space */
	name_detach(attach, 0);

	return EXIT_SUCCESS;
}

/*** Client Side of the code ***/
int sendShutdownRequest(const char *attachId) {
	my_data_t msg;
	int server_coid; //server connection ID.

	if ((server_coid = name_open(attachId, 0)) == -1) {
		printf("Failed to open connection to %s.\n", attachId);
		return EXIT_FAILURE;
	}

	/* We would have pre-defined data to stuff here */
	msg.hdr.type = 0x00;
	msg.hdr.subtype = STOP_LISTENING;

	/* Do whatever work you wanted with server connection */
	printf("Client sending request to shutdown comm channel \n", msg.data);
	if (MsgSend(server_coid, &msg, sizeof(msg), NULL, 0) == -1) {
		return EXIT_FAILURE;
	}
	sleep(1);

	/* Close the connection */
	name_close(server_coid);
	return 0;
}

/*** Client Side of the code ***/
int sendPlaneUpdateRequest(const char *attachId, int *planes,
		int sizeOfPlanes) {
	my_data_t msg;
	int server_coid; //server connection ID.

	if ((server_coid = name_open(attachId, 0)) == -1) {
		printf("Failed to open connection to %s.\n", attachId);
		return EXIT_FAILURE;
	}

	/* We would have pre-defined data to stuff here */
	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x01;

	/* Do whatever work you wanted with server connection */
	printf("Client sending request to update planes... \n", msg.data);
	if (MsgSend(server_coid, &msg, sizeof(msg), planes, sizeOfPlanes) == -1) {
		return EXIT_FAILURE;
	}
	sleep(1);

	/* Close the connection */
	name_close(server_coid);
	return 0;
}

struct threadContext {
	const char *attachId;
};

void* runRadar(void *context) {
	threadContext tc = *(threadContext*) context;
	cout << "In server\n";
	int ret = waitForPlaneUpdateRequest(tc.attachId);
	printf("Computer system returned %d on comm channel %s.\n", ret,
			tc.attachId);
	return NULL;
}

void* runComputerSystem(void *context) {
	int ret=0;
	int planes[NUM_PLANES] = { };
	threadContext tc = *(threadContext*) context;
	for (int i = 0; i < NUM_PLANES; i++) {
		cout << planes[i] << " ";
	}
	cout << endl;
	for (int i = 0; i < 10; i++) {
		ret = sendPlaneUpdateRequest(tc.attachId, planes, sizeof(planes));
		if (ret != 0) {
			printf("Unexpected error.\n");
		}
		printf("Radar returned %d on comm channel %s.\n", ret, tc.attachId);
		for (int i = 0; i < NUM_PLANES; i++) {
			cout << planes[i] << " ";
		}
		cout << endl;
	}
	ret = sendShutdownRequest(tc.attachId);
	if (ret != 0) {
		printf("Unexpected error.\n");
	}
	ret = sendShutdownRequest(tc.attachId);
	if (ret != 0) {
		printf("Expected error.\n");
	}
	return NULL;
}

int main(int argc, char **argv) {
	pthread_t radarCompSystemThread, displayCompSystemThread, radarThread;
	threadContext radarCommChannel = { RADAR_COMM_CHANNEL };
	threadContext displayCommChannel = { DISPLAY_COMM_CHANNEL };
	pthread_create(&radarCompSystemThread, NULL, runRadar, &radarCommChannel);
	pthread_create(&radarThread, NULL, runComputerSystem, &radarCommChannel);
//	pthread_create(&displayCompSystemThread, NULL, runRadar, &displayCommChannel);
//	pthread_create(&radarThread, NULL, runComputerSystem, &displayCommChannel);
	pthread_join(radarCompSystemThread, NULL);
	pthread_join(radarThread, NULL);
//	pthread_join(displayCompSystemThread, NULL);
}
