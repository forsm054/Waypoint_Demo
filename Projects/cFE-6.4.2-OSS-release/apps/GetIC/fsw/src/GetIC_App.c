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


#include "GetIC_App.h"
#include "GetIC_Events.h"
#include "GetIC_Msgids.h"

GetIC_AppData_t	GetIC_AppData;

static CFE_EVS_BinFilter_t GetIC_EventFilters[] = 
	{
		{DEBUG,										0x0000},
	};		
		

// Main Function
void GetIC_AppMain(void)
{

	uint32 Status;

	CFE_ES_RegisterApp(); // Register app w/ CFE
	GetIC_init(); // initialize the app. Sets up needed params (Success returns CFE_SUCCESS)
	
	// GetIC Run Loop
	while(CFE_ES_RunLoop(&GetIC_AppData.RunStatus) == TRUE)
	{
	
		Status = CFE_SB_RcvMsg(&GetIC_AppData.MsgPtr, GetIC_AppData.GetICPipe, CFE_SB_PEND_FOREVER);
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"GETIC MESSAGE RECEIVED!!!"); //debug
		if (Status == CFE_SUCCESS);
		{
			
			if(CFE_SB_GetMsgId(GetIC_AppData.MsgPtr) == GETIC_MID)
			{
				//GetIC_AppData.DataPacket = (GetIC_DataPacket_t *)CFE_SB_GetUserData(GetIC_AppData.MsgPtr);
				//length = CFE_SB_GetUserDataLength(GetIC_AppData.MsgPtr);
				//full_length = CFE_SB_GetTotalMsgLength(GetIC_AppData.MsgPtr);
				//printf("Data Length: %d, Message Length: %d \n", length, full_length); //debug 
				CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"GetIC Message ID matches."); //debug
				
			}

			if(Get_InitCond())
			{
				//CFE_SB_SetCmdCode(GetIC_AppData.MsgPtr, GETIC_RETURN_CID); 
				CFE_SB_SendMsg((CFE_SB_Msg_t *) &GetIC_AppData.InitCond);
			}

			//CFE_ES_ExitApp(GetIC_AppData.RunStatus);
		}
		
	} // End while

} // End GetIC_AppMain


//////////////////////////////////////////////////////////////////////////////////////////////////////

void GetIC_init(void)
{

	int status;
	
	CFE_EVS_Register(GetIC_EventFilters, sizeof(GetIC_EventFilters)/sizeof(CFE_EVS_BinFilter_t), CFE_EVS_BINARY_FILTER); // Register Event filter table so events can be output
	
	GetIC_AppData.RunStatus = CFE_ES_APP_RUN;
	
	// Set up Receive Pipe
	GetIC_AppData.PipeDepth = GETIC_PIPE_DEPTH;
	strcpy(GetIC_AppData.PipeName, "GETIC_PIPE");	
	
	// Create Receive (command) pipe
	status = CFE_SB_CreatePipe(&GetIC_AppData.GetICPipe, GetIC_AppData.PipeDepth, GetIC_AppData.PipeName);
	if(status != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_ERROR,"Failed to create GetIC pipe");
	}
	
	// Subscribe
	status = CFE_SB_Subscribe(GETIC_MID, GetIC_AppData.GetICPipe);
	if(status != CFE_SUCCESS)
	{
		CFE_EVS_SendEvent(DEBUG,CFE_EVS_ERROR,"Failed to subscribe to GetIC pipe");
	}
	
	// Initialize Return Message
	CFE_SB_InitMsg(&GetIC_AppData.InitCond, GETIC_RETURN_MID, sizeof(GetIC_DataPacket_t), TRUE);
	
	
	CFE_EVS_SendEvent(DEBUG, CFE_EVS_INFORMATION,"GetIC App Initialized");
	
	
} // End GetIC_init


//////////////////////////////////////////////////////////////////////////////////////////////////////

int Get_InitCond(void)
{

	//char param[20][20];
	//float temp_data[20], data[20];
	//int i = 0;
	//int status;
	//WP_InitTable_t *InitTblPtr;
	
	CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"Getting Initial Conditions");
	
	//status = CFE_TBL_GetAddress((void **)&InitTblPtr, InitTable_Handle);
	//CFE_EVS_SendEvent(DEBUG,CFE_EVS_INFORMATION,"Getting Initial Conditions");
	//if(status == CFE_TBL_INFO_UPDATED)
	//{
	//	WP_InitTableInit(InitTblPtr);
	//}
	
	strcpy(GetIC_AppData.InitCond.Success,"true");
	GetIC_AppData.InitCond.xWaypoint_A = 10;
	GetIC_AppData.InitCond.yWaypoint_A = 0;
	GetIC_AppData.InitCond.zWaypoint_A = 5;
	GetIC_AppData.InitCond.xWaypoint_B = 10;
	GetIC_AppData.InitCond.yWaypoint_B = 10;
	GetIC_AppData.InitCond.zWaypoint_B = 5;
	GetIC_AppData.InitCond.vel = 1;
	GetIC_AppData.InitCond.xLoc = 0;
	GetIC_AppData.InitCond.yLoc = 0;
	GetIC_AppData.InitCond.zLoc = 5;
	GetIC_AppData.InitCond.velTarget = 1;
	GetIC_AppData.InitCond.xTarget = 0;
	GetIC_AppData.InitCond.yTarget = 0;
	GetIC_AppData.InitCond.zTarget = 0;
	GetIC_AppData.InitCond.xFlag = 0;
	GetIC_AppData.InitCond.yFlag = 0;
	GetIC_AppData.InitCond.zFlag = 0;
	GetIC_AppData.InitCond.Waypoint = 'A';
	
	printf("Initial Conditions: %f %f %f %f %f %f %f %f %f %f %d \n", GetIC_AppData.InitCond.xWaypoint_A, GetIC_AppData.InitCond.yWaypoint_A, GetIC_AppData.InitCond.zWaypoint_A, GetIC_AppData.InitCond.xWaypoint_B, GetIC_AppData.InitCond.yWaypoint_B, GetIC_AppData.InitCond.zWaypoint_B, GetIC_AppData.InitCond.vel, GetIC_AppData.InitCond.xLoc, GetIC_AppData.InitCond.yLoc, GetIC_AppData.InitCond.zLoc, sizeof(int));
	
	//FILE *fp;
	//fp = fopen("data.txt","rt");
	
	//while(fscanf(fp, "%s %f", param[i], &temp_data[i]) != EOF)
	//{
		//data[i] = temp_data[i];
		/*
		if(strcmp(param[i], "xWaypoint_A") == 0)
		{
			GetIC_AppData.InitCond.xWaypoint_A = data[i];
		}
		else if(strcmp(param[i], "yWaypoint_A") == 0)
		{
			GetIC_AppData.InitCond.yWaypoint_A = data[i];
		}
		else if(strcmp(param[i], "zWaypoint_A") == 0)
		{
			GetIC_AppData.InitCond.zWaypoint_A = data[i];
		}
		else if(strcmp(param[i], "xWaypoint_B") == 0)
		{
			GetIC_AppData.InitCond.xWaypoint_B = data[i];
		}
		else if(strcmp(param[i], "yWaypoint_B") == 0)
		{
			GetIC_AppData.InitCond.yWaypoint_B = data[i];
		}
		else if(strcmp(param[i], "zWaypoint_B") == 0)
		{
			GetIC_AppData.InitCond.zWaypoint_B = data[i];
		}
		else if(strcmp(param[i], "vel") == 0)
		{
			GetIC_AppData.InitCond.vel = data[i];
		}
		else if(strcmp(param[i], "xLoc") == 0)
		{
			GetIC_AppData.InitCond.xLoc = data[i];
		}
		else if(strcmp(param[i], "yLoc") == 0)
		{
			GetIC_AppData.InitCond.yLoc = data[i];
		}
		else if(strcmp(param[i], "zLoc") == 0)
		{
			GetIC_AppData.InitCond.zLoc = data[i];
		}
		else if(strcmp(param[i], "xTarget") == 0)
		{
			GetIC_AppData.InitCond.xTarget = data[i];
		}
		else if(strcmp(param[i], "yTarget") == 0)
		{
			GetIC_AppData.InitCond.yTarget = data[i];
		}
		else if(strcmp(param[i], "zTarget") == 0)
		{
			GetIC_AppData.InitCond.zTarget = data[i];
		}
		else
		{
			printf("Invalid Parameter!");
		}
		*/
		//i++;
	//}
	
	
	
	// should compare read in parameter and use that to set values
	//printf("%d \n", InitTblPtr->vel);
	//Waypoint_AppData.DataPacket->vel = InitTblPtr->vel;
	//Waypoint_AppData.DataPacket->xLoc = InitTblPtr->xLoc;
	//Waypoint_AppData.DataPacket->yLoc = InitTblPtr->yLoc;
	//Waypoint_AppData.DataPacket->zLoc = InitTblPtr->zLoc;
	//Waypoint_AppData.DataPacket->xWaypoint_A = InitTblPtr->xWaypoint_A;
	//Waypoint_AppData.DataPacket->yWaypoint_A = InitTblPtr->yWaypoint_A;
	//Waypoint_AppData.DataPacket->zWaypoint_A = InitTblPtr->zWaypoint_A;
	//Waypoint_AppData.DataPacket->xWaypoint_B = InitTblPtr->xWaypoint_B;
	//Waypoint_AppData.DataPacket->yWaypoint_B = InitTblPtr->yWaypoint_B;
	//Waypoint_AppData.DataPacket->zWaypoint_B = InitTblPtr->zWaypoint_B;
	
	// DEBUG
	//for(i=0;i<10;i++)
	//{
	///	printf("%s \n", param[i]);
//		printf("%lf \n", data[i]);
	//}

	
	//status = CFE_TBL_ReleaseAddress(Waypoint_AppData.TblHandles[0]);
	//if(status == CFE_SUCCESS)
	//{
	//	CFE_EVS_SendEvent(DEBUG, CFE_EVS_INFORMATION,"Table address released"); //debug
	//}
	//else
	//{
	//	CFE_EVS_SendEvent(DEBUG, CFE_EVS_INFORMATION,"Error releasing table address"); //debug
	//}
	
	//fclose(fp);
	
	return 1;

}







