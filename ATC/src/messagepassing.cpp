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
#define ATTACH_POINT "myname"

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
int NUM_PLANES = 4;

/*** Server Side of the code ***/
int sendUpdateReply() {
	name_attach_t *attach;
	my_data_t msg;
	int rcvid;
	int planes[NUM_PLANES] = { 1, 2, 3 };
	/* Create a local name (/dev/name/local/...) */
	if ((attach = name_attach(NULL, ATTACH_POINT, 0)) == NULL) {
		return EXIT_FAILURE;
	}

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
			MsgReply(rcvid, EOK, planes, sizeof(planes));
			for (int i = 0; i < NUM_PLANES-1; i++) {
				planes[i] += 1;
			}
		}
	}

	/* Remove the name from the space */
	name_detach(attach, 0);

	return EXIT_SUCCESS;
}

/*** Client Side of the code ***/
int sendUpdateRequest() {
	my_data_t msg;
	int server_coid; //server connection ID.

	int planes[NUM_PLANES] = { };
	if ((server_coid = name_open(ATTACH_POINT, 0)) == -1) {
		return EXIT_FAILURE;
	}

	/* We would have pre-defined data to stuff here */
	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x01;

	/* Do whatever work you wanted with server connection */
	printf("Client sending request to update planes... \n", msg.data);
	if (MsgSend(server_coid, &msg, sizeof(msg), planes, sizeof(planes))
			== -1) {
		return EXIT_FAILURE;
	}
	for (int i = 0; i < NUM_PLANES-1; i++) {
		cout << planes[i] << " ";
	}
	cout << endl;
	sleep(1);

	/* Close the connection */
	name_close(server_coid);
	return EXIT_SUCCESS;
}

void* runServer(void *context) {
	cout << "In server\n";
	int ret = sendUpdateReply();
	printf("Server returned %d\n",ret);
	ret = sendUpdateReply();
	printf("Server returned %d\n",ret);
	return NULL;
}

void* runClient(void *context) {
	cout << "In client\n";
	int ret = sendUpdateRequest();
	printf("Client returned %d\n",ret);
	ret = sendUpdateRequest();
	printf("Client returned %d\n",ret);
	return NULL;
}

int main(int argc, char **argv) {
	pthread_t compSystemThread, radarThread;

	pthread_create(&compSystemThread, NULL, runServer, NULL);
	pthread_create(&radarThread, NULL, runClient, NULL);
	pthread_join(compSystemThread, NULL);
	pthread_join(radarThread, NULL);
}
