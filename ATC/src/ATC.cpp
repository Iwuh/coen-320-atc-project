#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dispatch.h>
#include <pthread.h>

#include "Plane.h"
#include "computerSystem.cpp"

#define RADAR_ATTACH_POINT "radar"
#define COMPUTERSYSTEM_ATTACH_POINT "computerSystem"

//void* runServer(void* context){
//	int ret = server();
//	return NULL;
//}
//
//void* runClient(void* context){
//	int ret = client();
//	return NULL;
//}
void* radarRun(void* context){
	name_attach_t *attach;
    int rcvid;
	Radar r = Radar();
	while(1){

   }
	return NULL;
}

void* compSystemRun(void* context){

	ComputerSystem cs = ComputerSystem();
	while(1){
			cs.logState();
			sleep(5);
	}
	return NULL;
}


//int main(int argc, char **argv) {
//	pthread_t compSystemThread, radarThread;
//
////	pthread_create(&compSystemThread, NULL, compSystemRun, NULL);
//	pthread_create(&radarThread, NULL, radarRun, NULL);
////	pthread_join(compSystemThread, NULL);
//	pthread_join(radarThread, NULL);
//}
