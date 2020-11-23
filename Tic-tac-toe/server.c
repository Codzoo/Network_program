#include "hw2.h"
#include <ctype.h>
#include<stdlib.h>
int list[20],cnt=0,sensor=0;
int be_invite,invite;
int main() {

#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif


    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);


    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
            bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }


    printf("Binding socket to local address...\n");
    if (bind(socket_listen,
                bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(bind_address);


    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("Waiting for connections...\n");


    while(1) {
        fd_set reads;
        reads = master;
        if (select(max_socket+1, &reads, 0, 0, 0) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        SOCKET i;
        for(i = 1; i <= max_socket; ++i) {
            if (FD_ISSET(i, &reads)) {

                if (i == socket_listen) {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    SOCKET socket_client = accept(socket_listen,
                            (struct sockaddr*) &client_address,
                            &client_len);
                    if (!ISVALIDSOCKET(socket_client)) {
                        fprintf(stderr, "accept() failed. (%d)\n",
                                GETSOCKETERRNO());
                        return 1;
                    }

                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                        max_socket = socket_client;

                    char address_buffer[100];
                    getnameinfo((struct sockaddr*)&client_address,
                            client_len,
                            address_buffer, sizeof(address_buffer), 0, 0,
                            NI_NUMERICHOST);
                    printf("New connection from %s\n", address_buffer);

                } else {
                    char read[4096];
                    memset(read,'\0',4096);
                    int bytes_received = recv(i, read, 4096, 0);
                    if(strncmp(read,"send",4)==0)
                    {
                    	int receiver_num;
                    	char message[100],buf[100]={'\0'},x;
                    	memset(message,'\0',100);
                    	sscanf(read,"%s %d",buf,&receiver_num);
                    	sprintf(message,"%s",read+7);
                    	strcat(message," (from player ");
                    	x=i+48;
                    	strcat(message,&x);
                    	strcat(message,")");
                    	send(receiver_num,message,strlen(message),0);
                    	printf("receiver = %d\nmessage = %s\n",receiver_num,message);
                    	continue;
                    }
                    if(strstr(read,"check"))
                    {
                    	list[cnt]=i;
                    	printf("list[%d] = %d\n",cnt,list[cnt]);
                    	cnt++;
                    	char for_num[10];
                    	for_num[0]=list[cnt-1]+48;
                    	send(i, for_num, strlen(for_num), 0);
                    	continue;
                    }
                    if(strstr(read,"ls"))
                    {
                    	//printf("cnt = %d\n",cnt);
                    	char for_list[cnt+2];
                    	for_list[0]='l';
                    	for(int i=1;i<=cnt;i++)
                    	{
                    		for_list[i]=list[i-1]+48;
                    	}
                    	if(for_list[strlen(for_list)-1]=='\n')
                    		for_list[strlen(for_list)-1]='\0';
                    	send(i, for_list, strlen(for_list), 0);
                    	continue;
                    }
                    if(strstr(read,"leave") || sensor==1)
                    {
                    	//printf("cnt=%d\n",cnt);
                    	for(int j=0;j<cnt;j++)
                    	{
                    		if(list[j]==i)
                    		{
                    			printf("1: %d list[%d] = %d\n",j,j,list[j]);
                    			list[j]=0;
                    		}
                    		if(sensor==1)
                    		{
		                		if(list[j]==invite || list[j]==be_invite)
		                		{
		                			printf("2: %d list[%d] = %d\n",j,j,list[j]);
		                			list[j]=0;
		                		}
                    		}
                    	}
                    }
                    if(read[2]=='0')
                    {
                    	for(int j=0;j<cnt;j++)
                    	{
		                	if(list[j]==invite || list[j]==be_invite)
		                	{
		                		printf("3: %d list[%d] = %d\n",j,j,list[j]);
		                		list[j]=0;
		                	}
                    	}
                    }
                    
                    //printf("num = %d\n",i);
                    if (bytes_received < 1) {
                        FD_CLR(i, &master);
                        CLOSESOCKET(i);
                        continue;
                    }
                    int invite_num;
                    char *tmp;
                    if(strstr(read,"invite"))
                    {
                    	invite_num = atoi(&read[7]);
                    	printf("invite_num = %d\n",invite_num);
                    	char str[80];
                    	sprintf(str,"Do you want to play with %d ? (y&num or n)",i);
                    	send(invite_num, str, strlen(str), 0);
                    	continue;
                    }
                    if(strncmp(read,"y",1)==0)
                    {
                    	printf("%s\n",read);
                    	be_invite = i; invite = atoi(&read[2]);
                    	printf("%d %d\n",invite,be_invite);
                    	char str_invite[11]={"Connecting\0"};
                    	send(invite, str_invite, strlen(str_invite), 0);
                    	send(be_invite, str_invite, strlen(str_invite), 0);
                    	sensor=1;
                    	continue;
                    }
                    if(strncmp(read,"n",1)==0)
                    {
                    	continue;
                    }
					//send(5, read, bytes_received, 0);   //pick one
					if(sensor==1)
					{
						if(i==invite)
						{
							send(be_invite, read, bytes_received, 0);
						}
						else if(i==be_invite)
						{
							send(invite, read, bytes_received, 0);
						}
					}
					/*else if(sensor==0)
					{
						SOCKET j;
		                for (j = 1; j <= max_socket; ++j) {
		                	//printf("j = %d\n",j);
		                    if (FD_ISSET(j, &master)) {
		                        if (j == socket_listen || j == i) //add !=invite
		                            continue;
		                        else
		                            send(j, read, bytes_received, 0);
		                            printf("send(%d) = %skkk\n",j,read);
		                    }
		                }
					}*/
                }
            } //if FD_ISSET
        } //for i to max_socket
    } //while(1)



    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

#if defined(_WIN32)
    WSACleanup();
#endif


    printf("Finished.\n");

    return 0;
}
