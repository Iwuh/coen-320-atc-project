#ifndef SRC_COMMAND_CODES_H_
#define SRC_COMMAND_CODES_H_
/*
 * Plane command codes
 */
// Command to have the plane reply with its current position and velocity.
#define COMMAND_RADAR_PING 1
// Command to have the plane change its velocity.
#define COMMAND_SET_VELOCITY 2
// Command to have the plane thread terminate.
#define COMMAND_EXIT_THREAD 3

/*
 * ComputerSystem command codes
 */

// To create timers for periodic tasks we need to know the amount
// Timer code to fire to perform violation checks
#define AIRSPACE_VIOLATION_CONSTRAINT_TIMER 11
// Timer code to fire to update logfile with airspace state
#define LOG_AIRSPACE_TIMER 12
// Timer code to fire a request to operator console for command info
#define OPERATOR_COMMAND_CHECK_TIMER 13

// Inbound command to the system to process operator request
#define COMMAND_OPERATOR_REQUEST 111
// Outbound command to update plane count from the radar
#define COMMAND_UPDATE_PLANE_COUNT 112
// Outbound command to update the plane positions from the radar
#define COMMAND_UPDATE_AIRSPACE 113

/*
 * DataDisplay command codes
 */

// Incoming command to display one plane's information.
#define COMMAND_ONE_PLANE 1111
// Incoming command to display multiple planes' information.
#define COMMAND_MULTIPLE_PLANE 1112
// Incoming command to warn about an airspace constraint violation.
#define COMMAND_WARNING 1113
// Incoming command to display the grid view of the airspace.
#define COMMAND_GRID 1114

/*
 * OperatorConsole command codes
 */

#define OPCON_CONSOLE_COMMAND_GET_USER_COMMAND 11111

#define OPCON_USER_COMMAND_NO_COMMAND_AVAILABLE 11112
#define OPCON_USER_COMMAND_DISPLAY_PLANE_INFO 11113
#define OPCON_USER_COMMAND_SET_PLANE_VELOCITY 11114

#endif /* SRC_COMMAND_CODES_H_ */
