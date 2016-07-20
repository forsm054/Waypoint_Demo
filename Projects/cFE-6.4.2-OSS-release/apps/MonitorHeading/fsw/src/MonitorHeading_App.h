#ifndef _MonitorHeading_App_h
#define _MonitorHeading_App_h

#include "network_includes.h" // only need if using sockets

// CoreFlight includes
#include "cfe_error.h"
#include "cfe_sb.h"
#include "cfe_time.h"
#include "cfe_evs.h"
#include "cfe_tbl.h"
#include "cfe_sb.h"
#include "cfe_es.h"

// C includes
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>

// OSAL includes
#include "common_types.h"
#include "osapi.h"

#define MONITORHEADING_PIPE_DEPTH 12
#define ARRAY_SIZE 100

typedef struct
{
	// Initial Conditions (only used for simulation purposes)
	float xWaypoint_A;
	float yWaypoint_A;
	float zWaypoint_A;
	float xWaypoint_B;
	float yWaypoint_B;
	float zWaypoint_B;
	
	// State
  float   AirSpeed;
  float		heading;
  float		xLoc;
  float		yLoc;
  float		zLoc;
  char		Waypoint;
	
} MonitorHeading_State_t;


typedef struct
{
	// Command interface counters
	uint8 CmdCounter;
	uint8 ErrCounter;
	
	MonitorHeading_State_t DataPacket;
	
	CFE_SB_MsgPtr_t	MsgPtr;
	CFE_SB_PipeId_t MonitorHeadingPipe; // Pipe Identifier
	
	// App status
	uint32 RunStatus;
	
	// Pipe Data
	char PipeName[16];
	uint16 PipeDepth;
	
	CFE_TBL_Handle_t TblHandle;
	
} MonitorHeading_AppData_t;


// Function Definitions
void MntrHding_AppMain(void);
void MonitorHeading_init(void);
void AdjustHeading(void);

#endif
