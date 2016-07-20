/////////////////////////////////////////////////////////////////////////
//
//  README
//
//  Name: ActSci_App.c
//  By: Alec Forsman - Red Canyon Software
//  Created: 5/12/16
//  Updated: 5/12/16
//
//  Description:
//		This is a Core Flight Application intended to model the ActivateScience command in the PLEXIL
//	  Waypoint plan. The app simply creates a pipe to receive messages, and when
//		a message is received it will perform the corresponding neccessary functions and send data
//		back so PLEXIL can continue to update the flight. This can be used in the future as the
//		layout for communication between PLEXIL and CFE.
//
//	Updates:
//		5/12/16
//			- Created
//			- Created SB pipe to receive commands from Waypoint_App.c
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


#include "ActSci_App.h"
#include "ActSci_Events.h"
#include "ActSci_Msgids.h"

ActSci_AppData_t	ActSci_AppData;

static CFE_EVS_BinFilter_t ActSci_EventFilters[] = 
	{
		{DEBUG,										0x0000},
	};		
		

// Main Function
void ActSci_AppMain(void)
{
	uint32 Status;

	CFE_ES_RegisterApp(); // Register app w/ CFE
	ActSci_init(); // initialize the app. Sets up needed params (Success returns CFE_SUCCESS)
	
	// ActSci Run Loop
	while(CFE_ES_RunLoop(&ActSci_AppData.RunStatus) == TRUE)
	{
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"Run loop entered, Run Status: %d", &ActSci_AppData.RunStatus);	
	
		Status = CFE_SB_RcvMsg(&ActSci_AppData.MsgPtr, ActSci_AppData.ActSciPipe, CFE_SB_PEND_FOREVER);
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"ACTIVATESCIENCES MESSAGE RECEIVED!!!");
		
		if (Status == CFE_SUCCESS);
		{
		
			if(CFE_SB_GetMsgId(ActSci_AppData.MsgPtr) == ACTSCI_MID)
			{
				CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"Message ID matches."); //debug
				ActSci_DataPacket_t *DataPacket_temp = (ActSci_DataPacket_t *)ActSci_AppData.MsgPtr;
				ActSci_AppData.DataPacket = *DataPacket_temp;
				
				CFE_SB_InitMsg(&ActSci_AppData.DataPacket, ACTSCI_RETURN_MID, sizeof(ActSci_DataPacket_t), FALSE);
				
				//CFE_ES_ExitApp(UpdFlight_AppData.RunStatus);
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &ActSci_AppData.DataPacket);
			}
		}
		
	} // End while

} // End ActSci_AppMain


//////////////////////////////////////////////////////////////////////////////////////////////////////

void ActSci_init(void)
{
	int status;

	CFE_EVS_Register(ActSci_EventFilters, sizeof(ActSci_EventFilters)/sizeof(CFE_EVS_BinFilter_t), CFE_EVS_BINARY_FILTER); // Register Event filter table so events can be output
	
	ActSci_AppData.RunStatus = CFE_ES_APP_RUN;
	
	ActSci_AppData.PipeDepth = ACTSCI_PIPE_DEPTH;
	strcpy(ActSci_AppData.PipeName, "ACTSCI_PIPE");	
	
	// Create Receive (command) pipe
	status = CFE_SB_CreatePipe(&ActSci_AppData.ActSciPipe, ActSci_AppData.PipeDepth, ActSci_AppData.PipeName);
	if(status != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_ERROR,"Failed to create ActSci pipe");
	}
	
	// Subscribe
	status = CFE_SB_Subscribe(ACTSCI_MID, ActSci_AppData.ActSciPipe);
	if(status != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_ERROR,"Failed to subscribe to ActSci pipe");
	}
	
	CFE_SB_InitMsg(&ActSci_AppData.DataPacket, ACTSCI_RETURN_MID, sizeof(ActSci_DataPacket_t), TRUE);
	
	
	CFE_EVS_SendEvent(DEBUG, CFE_EVS_INFORMATION,"ActivateScience App Initialized");
	
} // End ActSci_init




