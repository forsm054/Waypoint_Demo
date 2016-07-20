/////////////////////////////////////////////////////////////////////////
//
//  README
//
//  Name: UpdFlight_App.c
//  By: Alec Forsman - Red Canyon Software
//  Created: 5/12/16
//  Updated: 5/12/16
//
//  Description:
//		This is a Core Flight Application intended to model the UpdateFlight commands in the PLEXIL
//	  Waypoint plan. The app simply creates a pipe to receive messages, and when
//		a message is received it will perform the corresponding neccessary functions and send data
//		back so PLEXIL can continue to update the flight. This can be used in the future as the
//		layout for communication between PLEXIL and CFE.
//
//	Updates:
//		5/12/16
//			- Created
//			- Created SB pipe to receive commands from TestUdp_app.c
//		5/13/16
//			- Improved naming conventions
//
//	Issues/ToDo:
//		- Interface with PLEXIL
//		- Setup to perform neccessary calculations
//		- Send data back to TestUdp_app.c
//		- Get app to receive real data
//		- Combine Command Pack structure and Command structure. Shouldn't need both
//		- clean up
//		- Implement error checks
//		- NOTE: If app doesn't run, 90% of the time its an issue with the code(will still happen if it compiles)
//
/////////////////////////////////////////////////////////////////////////


#include "UpdFlight_App.h"
#include "UpdFlight_Events.h"
#include "UpdFlight_Msgids.h"

UpdFlight_AppData_t	UpdFlight_AppData;

static CFE_EVS_BinFilter_t UpdFlight_EventFilters[] = 
	{
		{DEBUG,										0x0000},
	};		
		

// Main Function
void UpdFlight_AppMain(void)
{
	uint32 Status;

	CFE_ES_RegisterApp(); // Register app w/ CFE
	UpdFlight_init(); // initialize the app. Sets up needed params (Success returns CFE_SUCCESS)

	// UpdFlight Run Loop
	while(CFE_ES_RunLoop(&UpdFlight_AppData.RunStatus) == TRUE)
	{
	
		Status = CFE_SB_RcvMsg(&UpdFlight_AppData.MsgPtr, UpdFlight_AppData.UpdFlightPipe, CFE_SB_PEND_FOREVER);
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"UPDATEFLIGHT MESSAGE RECEIVED!!!"); //debug

		if (Status == CFE_SUCCESS);
		{
			
			if(CFE_SB_GetMsgId(UpdFlight_AppData.MsgPtr) == UPDFLIGHT_MID)
			{
				
				CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"Message ID matches."); //debug
				UpdFlight_DataPacket_t *DataPacket_temp = (UpdFlight_DataPacket_t *)UpdFlight_AppData.MsgPtr;
				UpdFlight_AppData.DataPacket = *DataPacket_temp;
				
				UpdFlight_AppData.DataPacket = UpdateFlight(UpdFlight_AppData.DataPacket);
				
				CFE_SB_InitMsg(&UpdFlight_AppData.DataPacket, UPDFLIGHT_RETURN_MID, sizeof(UpdFlight_DataPacket_t), FALSE);
				
				//CFE_ES_ExitApp(UpdFlight_AppData.RunStatus);
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &UpdFlight_AppData.DataPacket);
			}
		}
		
	} // End while

} // End UpdFlight_AppMain


//////////////////////////////////////////////////////////////////////////////////////////////////////

void UpdFlight_init(void)
{
	int status;
	
	CFE_EVS_Register(UpdFlight_EventFilters, sizeof(UpdFlight_EventFilters)/sizeof(CFE_EVS_BinFilter_t), CFE_EVS_BINARY_FILTER); // Register Event filter table so events can be output
	
	UpdFlight_AppData.RunStatus = CFE_ES_APP_RUN;
	
	UpdFlight_AppData.PipeDepth = UPDFLIGHT_PIPE_DEPTH;
	strcpy(UpdFlight_AppData.PipeName, "UPDFLIGHT_PIPE");	
	
	// Create Receive (command) pipe
	status = CFE_SB_CreatePipe(&UpdFlight_AppData.UpdFlightPipe, UpdFlight_AppData.PipeDepth, UpdFlight_AppData.PipeName);
	if(status != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_ERROR,"Failed to create UpdFlight pipe");
	}
	
	// Subscribe
	status = CFE_SB_Subscribe(UPDFLIGHT_MID, UpdFlight_AppData.UpdFlightPipe);
	if(status != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_ERROR,"Failed to subscribe to UpdFlight pipe");
	}
	
	CFE_SB_InitMsg(&UpdFlight_AppData.DataPacket, UPDFLIGHT_RETURN_MID, sizeof(UpdFlight_DataPacket_t), TRUE);
	
	
	CFE_EVS_SendEvent(DEBUG, CFE_EVS_INFORMATION,"UpdateFlight App Initialized");
	

} // End UpdFlight_init


//////////////////////////////////////////////////////////////////////////////////////////////////////

UpdFlight_DataPacket_t UpdateFlight(UpdFlight_DataPacket_t Data)
{

	if(Data.Waypoint == 'A')
	{
		Data.xLoc = Data.xLoc + Data.vel;
	}
	else if(Data.Waypoint == 'B')
	{
		Data.yLoc = Data.yLoc + Data.vel;
	}
	else
	{
		// Err
	}
	
	return Data;
}





