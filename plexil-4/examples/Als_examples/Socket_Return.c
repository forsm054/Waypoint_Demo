#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUFLEN 512 // Maximum length of buffer
#define PORT 8032 // listener port
#define PORT_RET 8033 // return port
#define SERVER "127.0.0.1"

int main(void)
{
//////////////////////////////////////////////////////////////////////////////////////////////
	struct sockaddr_in sin_me, sin_other;
	
	int s, i, slen = sizeof(sin_other), recv_len;
	char buf[BUFLEN];
	
	// create UDP socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror("Failed to create socket");
	}
	
	// zero out the structure
	memset((char *) &sin_me, 0, sizeof(sin_me));
	
	sin_me.sin_family = AF_INET;
	sin_me.sin_port = htons(PORT);
	sin_me.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// bind socket to port
	if(bind(s, (struct sockaddr*)&sin_me, sizeof(sin_me)) < 0)
	{
		perror("Failed binding");
	}
//////////////////////////////////////////////////////////////////////////////////////////////
	struct sockaddr_in sin_other2;
	int s2, i2, slen2=sizeof(sin_other2);
	char buf2[BUFLEN];
	char message[BUFLEN] = "done";
	
	if((s2=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror("Socket");
	}
	
	memset((char *) &sin_other2, 0, sizeof(sin_other2));
	sin_other2.sin_family = AF_INET;
	sin_other2.sin_port = htons(PORT);
	
	if(inet_aton(SERVER, &sin_other2.sin_addr) == 0)
	{
		perror("inet_aton() failed\n");
	}
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	// continually listen for data
	while(1)
	{
		printf("Waiting for data...");
		fflush(stdout);
		
		// try to receive some data
		memset(buf, '\0', BUFLEN);
		if((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &sin_other, &slen)) < 0)
		{
			perror("Failed receiving data");
		}
		
		printf("Received packet from %s:%d\n", inet_ntoa(sin_other.sin_addr), ntohs(sin_other.sin_port));
		
		//int data_int[1];
		char data_char = buf[0];
		//sprintf(data_char,"%c",buf[0]);
		int data_int[10];
		//data_char[0] = buf[0];
		//sprintf(data_char,"%d",buf[0]);
		data_int[0] = buf[1];
		data_int[1] = buf[3];
		data_int[2] = buf[5];
		//sscanf(buf[0], "%s", &data_char);
		//printf("Data: %d\n", data_int+1);
		//sprintf(data_char,"%s\n",buf[0]);
		printf("String: %c %d %d %d\n", data_char,data_int[0],data_int[1],data_int[2]);
		//printf("Integer: %d\n", data_int);
		
		//int data = buf[1];
		//data = data + 2;
		//buf[1] = data;
		
		//char buf2[10] = "done";
		
		// send message
		if(sendto(s2,message, strlen(message), 0, (struct sockaddr *) &sin_other2, slen2) < 0)
		{
			perror("sendto()");
		}
		
		// receive a reply and print it
		// clear the buffer
		memset(buf2, '\0', BUFLEN);
		// try to receive some data, this is a blocking call
		//if(recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &sin_other, &slen) < -1)
		//{
		//	perror("recvfrom()");
		//}
		
		puts(buf2);
	}
	
	// close socket
	close(s);
	return 0;
}