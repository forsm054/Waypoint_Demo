/////////////////////////////////////////////////////////////////////////
//
//  README
//
//  Name: GetParams_App.c
//  By: Alec Forsman - Red Canyon Software
//  Created: 5/12/16
//  Updated: 5/12/16
//
//  Description:
//		This is a Core Flight Application intended to model the GetParams commands in the PLEXIL
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
//		5/16/16-5/18/16
//			- Successfully passed udp of different parameter types into plexil.
//		5/19/16
//			- Got app to send udp to plexil by sleeping for 1 second (plexil is too slow).
//		5/27/16
//			- GetParams_SendReturn now passes 'current' flight parameters.
//
//	Issues/ToDo:
//		- Setup to perform neccessary calculations
//		- Send data back to TestUdp_app.c
//		- Use error check instead of sleeping
//		- Can't get return value sent through
//		- Combine Command Pack structure and Command structure. Shouldn't need both.
//		- CFE goes into unknown status when app quits.
//		- clean up
//		- Implement error checks
//		- NOTE: If app doesn't run, 90% of the time its an issue with the code(will still happen if it compiles)
//
/////////////////////////////////////////////////////////////////////////


#include "GetParams_App.h"
#include "GetParams_Events.h"
#include "GetParams_Msgids.h"

GetParams_AppData_t	GetParams_AppData;

static CFE_EVS_BinFilter_t GetParams_EventFilters[] = 
	{
		{DEBUG,										0x0000},
	};		
		

// Main Function
void GetParams_AppMain(void)
{
	uint32 Status;

	CFE_ES_RegisterApp(); // Register app w/ CFE
	GetParams_init(); // initialize the app. Sets up needed params (Success returns CFE_SUCCESS)
	
	// GetParams Run Loop
	while(CFE_ES_RunLoop(&GetParams_AppData.RunStatus) == TRUE)
	{
	
		Status = CFE_SB_RcvMsg(&GetParams_AppData.MsgPtr, GetParams_AppData.GetParamsPipe, CFE_SB_PEND_FOREVER);
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"GETPARAMS MESSAGE RECEIVED!!!"); //debug
		
		if (Status == CFE_SUCCESS);
		{
			
			if(CFE_SB_GetMsgId(GetParams_AppData.MsgPtr) == GETPARAMS_MID)
			{
				CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"GetParams Message ID matches."); //debug
				GetParams_DataPacket_t *DataPacket_temp = (GetParams_DataPacket_t *)GetParams_AppData.MsgPtr;
				GetParams_AppData.DataPacket = *DataPacket_temp;
				
				CFE_SB_InitMsg(&GetParams_AppData.DataPacket, GETPARAMS_RETURN_MID, sizeof(GetParams_DataPacket_t), FALSE);

				printf("Data Received: %f %f %f %f %f %f %f %f %f %f \n", GetParams_AppData.DataPacket.xWaypoint_A, GetParams_AppData.DataPacket.yWaypoint_A, GetParams_AppData.DataPacket.zWaypoint_A, GetParams_AppData.DataPacket.xWaypoint_B, GetParams_AppData.DataPacket.yWaypoint_B, GetParams_AppData.DataPacket.zWaypoint_B,GetParams_AppData.DataPacket.vel, GetParams_AppData.DataPacket.xLoc, GetParams_AppData.DataPacket.yLoc, GetParams_AppData.DataPacket.zLoc);
			
				//GetParams_SendReturn(*GetParams_AppData.DataPacket);
				//CFE_ES_ExitApp(GetParams_AppData.RunStatus);
				CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"GetParams: Sending return data."); //debug
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &GetParams_AppData.DataPacket);
			}
		}
		
	} // End while
	
} // End GetParams_AppMain


//////////////////////////////////////////////////////////////////////////////////////////////////////

void GetParams_init(void)
{
	int status;
	
	CFE_EVS_Register(GetParams_EventFilters, sizeof(GetParams_EventFilters)/sizeof(CFE_EVS_BinFilter_t), CFE_EVS_BINARY_FILTER); // Register Event filter table so events can be output
	
	GetParams_AppData.RunStatus = CFE_ES_APP_RUN;
	
	GetParams_AppData.PipeDepth = GETPARAMS_PIPE_DEPTH;
	strcpy(GetParams_AppData.PipeName, "GETPARAMS_PIPE");	
	
	// Create Receive (command) pipe
	status = CFE_SB_CreatePipe(&GetParams_AppData.GetParamsPipe, GetParams_AppData.PipeDepth, GetParams_AppData.PipeName);
	if(status != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_ERROR,"Failed to create GetParams pipe");
	}
	
	// Subscribe
	status = CFE_SB_Subscribe(GETPARAMS_MID, GetParams_AppData.GetParamsPipe);
	if(status != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_ERROR,"Failed to subscribe to GetParams pipe");
	}
	
	CFE_SB_InitMsg(&GetParams_AppData.DataPacket, GETPARAMS_RETURN_MID, sizeof(GetParams_DataPacket_t), TRUE);
	
	
	CFE_EVS_SendEvent(DEBUG, CFE_EVS_INFORMATION,"GetParams App Initialized");
	
	
} // End GetParams_init


//////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void GetParams_SendReturn(GetParams_DataPacket_t data)
{
	CFE_EVS_SendEvent(DEBUG, CFE_EVS_INFORMATION,"GetParams_SendReturn");
	struct sockaddr_in return_socket;
	int return_SocketID, slen=sizeof(return_socket);
	
	// FIXME
	sleep(1); // should have some sort of error check here instead of sleeping
	
	//char return_buf[6] = "true";
	//printf("Return Buffer is: %s \n", return_buf); //debug
	GetParams_Return_t ReturnData;
	strcpy(ReturnData.success, "true");
	printf("Data being assigned: %f %f %f %f \n", data.zWaypoint_B, data.xLoc, data.yLoc, data.zLoc);
	ReturnData.vel = data.zWaypoint_B; // this should be .vel but something is wrong with the indexing, sending data over pipe
	ReturnData.xLoc = data.xLoc;
	ReturnData.yLoc = data.yLoc;
	ReturnData.zLoc = data.zLoc;
	printf("Data AFTER being assigned: %f %f %f %f \n", ReturnData.vel, ReturnData.xLoc, ReturnData.yLoc, ReturnData.zLoc);
	
	if((return_SocketID = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		CFE_EVS_SendEvent(DEBUG, CFE_EVS_INFORMATION,"SocketError");
		perror("Socket");
		exit(1);
	}
	
	memset((char *) &return_socket, 0, sizeof(return_socket));
	return_socket.sin_family = AF_INET;
	return_socket.sin_port = htons(RETURN_PORT);
	
	if(inet_aton(SERVER, &return_socket.sin_addr) < 0)
	{
		CFE_EVS_SendEvent(DEBUG, CFE_EVS_INFORMATION,"inet_aton Error");
		perror("inet_aton() failed\n");
		exit(1);
	}
	
	if(sendto(return_SocketID, (GetParams_Return_t *)&ReturnData, sizeof(GetParams_Return_t), 0, (struct sockaddr *) &return_socket, slen) < 0)
	{
		CFE_EVS_SendEvent(DEBUG, CFE_EVS_INFORMATION,"sendto Error");
		perror("sendto()");
		exit(1);
	}
	
	printf("Sent Packet to %s:%d\n", inet_ntoa(return_socket.sin_addr), ntohs(return_socket.sin_port)); //debug

}
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////









