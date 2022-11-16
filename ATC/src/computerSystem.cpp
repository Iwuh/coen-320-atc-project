/* class ComputerSystem
 * int ID
 * list of aircraft in the airspace: position - x y z
 *
 * function violationScan - periodic
 * function alert - critical sporadic
 * sendAircraftInfo - id, position - periodic
 * processOperatorRequests - aperiodic
 * logState - periodic
 */
#include <map>
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>

#include "Plane.h"


using namespace std;
struct Position {
	Position() : x_coord(0), y_coord(0), z_coord(0) {}
	Position(int x_coord, int y_coord, int z_coord) : x_coord(x_coord), y_coord(y_coord), z_coord(z_coord) {}
	int x_coord;
	int y_coord;
	int z_coord;
	const int Y_CAP=3000;
	const int X_CAP=3000;
	const int Z_CAP=1000;
};

class ComputerSystem
{
private:
	std::map<int, Position> airspace;
	void violationScan(int n){

	}
	void alert(){

	}
	void sendAircraftInfo(){

	}
	void processOperatorRequest(){

	}
	void printCurrentTime(){
		char s[1000];

		time_t t = time(NULL);
		struct tm * p = localtime(&t);

		strftime(s, 1000, "%A, %B %d %Y", p);

		printf("%s ", s);
	}
public:
	ComputerSystem()
	{
	}
	static void* Start(void* context);
	void logState(){
		printCurrentTime();
		for (auto const& x : airspace){
			printf(" %d:%s,%s,%s |", x.first,std::to_string(x.second.x_coord),std::to_string(x.second.y_coord),std::to_string(x.second.z_coord));
//		    std::cout << x.first  // string (key)
//		              << ':'
//		              << x.second.x_coord // string's value
//		              << x.second.y_coord // string's value
//		              << x.second.z_coord // string's value
//		              << std::endl;
		}
	}
	void updateAirspace(int ID, Vec3 vec){
		Position pos = Position(vec.x, vec.y, vec.z);
		airspace.insert(pair<int,Position>(ID,pos));
	}
};

int main(int argc, char **argv) {
	ComputerSystem c = ComputerSystem();
	for (int i = 1; i< 10; i++){
		PlaneStartParams pparams = {
				i,
				10,
				{1+i,2+i,3+i},
				{1+i,2+i,3+i},
			};
		Plane* p = Plane::CreateWithParams(pparams);
		c.updateAirspace(i,p->getVelocity());
	}
	c.logState();
}
