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
