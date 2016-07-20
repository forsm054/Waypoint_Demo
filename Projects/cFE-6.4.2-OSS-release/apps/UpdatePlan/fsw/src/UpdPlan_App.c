/////////////////////////////////////////////////////////////////////////
//
//  README
//
//  Name: UpdPlan_App.c
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


#include "UpdPlan_App.h"
#include "UpdPlan_Events.h"
#include "UpdPlan_Msgids.h"

UpdPlan_AppData_t	UpdPlan_AppData;

static CFE_EVS_BinFilter_t UpdPlan_EventFilters[] = 
	{
		{DEBUG,										0x0000},
	};		
		

// Main Function
void UpdPlan_AppMain(void)
{
	uint32 Status;

	CFE_ES_RegisterApp(); // Register app w/ CFE
	UpdPlan_init(); // initialize the app. Sets up needed params (Success returns CFE_SUCCESS)

	// UpdPlan Run Loop
	while(CFE_ES_RunLoop(&UpdPlan_AppData.RunStatus) == TRUE)
	{
	
		Status = CFE_SB_RcvMsg(&UpdPlan_AppData.MsgPtr, UpdPlan_AppData.UpdPlanPipe, CFE_SB_PEND_FOREVER);
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"UPDATEFLIGHT MESSAGE RECEIVED!!!"); //debug

		if (Status == CFE_SUCCESS);
		{
			
			if(CFE_SB_GetMsgId(UpdPlan_AppData.MsgPtr) == UPDPLAN_MID)
			{
				
				CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"Message ID matches."); //debug
				UpdPlan_DataPacket_t *DataPacket_temp = (UpdPlan_DataPacket_t *)UpdPlan_AppData.MsgPtr;
				UpdPlan_AppData.DataPacket = *DataPacket_temp;
				
				UpdPlan_AppData.DataPacket = UpdatePlan(UpdPlan_AppData.DataPacket);
				
				CFE_SB_InitMsg(&UpdPlan_AppData.DataPacket, UPDPLAN_RETURN_MID, sizeof(UpdPlan_DataPacket_t), FALSE);
				
				//CFE_ES_ExitApp(UpdPlan_AppData.RunStatus);
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &UpdPlan_AppData.DataPacket);
			}
		}
		
	} // End while

} // End UpdPlan_AppMain


//////////////////////////////////////////////////////////////////////////////////////////////////////

void UpdPlan_init(void)
{
	int status;
	
	CFE_EVS_Register(UpdPlan_EventFilters, sizeof(UpdPlan_EventFilters)/sizeof(CFE_EVS_BinFilter_t), CFE_EVS_BINARY_FILTER); // Register Event filter table so events can be output
	
	UpdPlan_AppData.RunStatus = CFE_ES_APP_RUN;
	
	UpdPlan_AppData.PipeDepth = UPDPLAN_PIPE_DEPTH;
	strcpy(UpdPlan_AppData.PipeName, "UPDPLAN_PIPE");	
	
	// Create Receive (command) pipe
	status = CFE_SB_CreatePipe(&UpdPlan_AppData.UpdPlanPipe, UpdPlan_AppData.PipeDepth, UpdPlan_AppData.PipeName);
	if(status != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_ERROR,"Failed to create UpdPlan pipe");
	}
	
	// Subscribe
	status = CFE_SB_Subscribe(UPDPLAN_MID, UpdPlan_AppData.UpdPlanPipe);
	if(status != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_ERROR,"Failed to subscribe to UpdPlan pipe");
	}
	
	CFE_SB_InitMsg(&UpdPlan_AppData.DataPacket, UPDPLAN_RETURN_MID, sizeof(UpdPlan_DataPacket_t), TRUE);
	
	
	CFE_EVS_SendEvent(DEBUG, CFE_EVS_INFORMATION,"UpdateFlight App Initialized");
	

} // End UpdPlan_init


//////////////////////////////////////////////////////////////////////////////////////////////////////

UpdPlan_DataPacket_t UpdatePlan(UpdPlan_DataPacket_t Data)
{
	CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"Updating Plan");
	
	if(Data.Waypoint == 'A')
	{
		Data.Waypoint = 'B';
	}
	else if(Data.Waypoint == 'B')
	{
		// Do nothing for now
	}
	else
	{
		// Error
	}
	
	return Data;
}





