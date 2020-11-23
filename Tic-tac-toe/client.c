#include "hw2.h"

#if defined(_WIN32)
#include <conio.h>
#endif
char board[3][3] = {                    
					   {'1','2','3'},      
					   {'4','5','6'}, 
					   {'7','8','9'}     
					 };
int num,sensor=1,win=0,count=0,check=1,sensor_game=0;
char label;
int main(int argc, char *argv[]) {

#if defined(_WIN32)
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2), &d)) {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif

    if (argc < 3) {
        fprintf(stderr, "usage: tcp_client hostname port\n");
        return 1;
    }

    //printf("Configuring remote address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if (getaddrinfo(argv[1], argv[2], &hints, &peer_address)) {
        fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }


    //printf("Remote address is: ");
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
            address_buffer, sizeof(address_buffer),
            service_buffer, sizeof(service_buffer),
            NI_NUMERICHOST);
    //printf("%s %s\n", address_buffer, service_buffer);


    //printf("Creating socket...\n");
    SOCKET socket_peer;
    socket_peer = socket(peer_address->ai_family,
            peer_address->ai_socktype, peer_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_peer)) {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }


    //printf("Connecting...\n");
    if (connect(socket_peer,
                peer_address->ai_addr, peer_address->ai_addrlen)) {
        fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }
    freeaddrinfo(peer_address);

    printf("Connected.\n");
    if(check)        
    {
        char forcheck[5]={"check"};
        send(socket_peer, forcheck, strlen(forcheck), 0);
        check=0;
        printf("Tic-tac-toe Format\n");
    }
	printf(" %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
	printf("---+---+---\n");
	printf(" %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
	printf("---+---+---\n");
	printf(" %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);	
	
	printf("Input your instruction followed by enter.\n");
	char get_num[10];
	recv(socket_peer, get_num, 10, 0);
	printf("Your number : %c\n",get_num[0]);
	printf("\nUsage: \n\n ls\n invite {player's number}\n send {num} {message: Content}\n leave\n\n");
    while(1) {

        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(socket_peer, &reads);
#if !defined(_WIN32)
        FD_SET(0, &reads);
#endif

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        if (select(socket_peer+1, &reads, 0, 0, &timeout) < 0) {
            fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        if (FD_ISSET(socket_peer, &reads)) 
        {
            char read[4096];
            memset(read,'\0',4096);
            int bytes_received = recv(socket_peer, read, 4096, 0);
            if (bytes_received < 1) {
                printf("Connection closed by peer.\n");
                break;
            }
            if(strstr(read,"message"))
            {
            	printf("%s\n",read);
            	continue;
            }
            if(strstr(read,"Connecting"))
            {
            	read[10]=0;
            	printf("%s\n",read);
            	sensor_game=1;
            	continue;
            }
            if(strstr(read,"Do"))
            {
            	read[41]=0;
            	printf("%s\n",read);
            	continue;
            }
            if(strstr(read,"leave"))
            {
            	printf("Your opponent left\n");
            	printf("Closing socket...\n");
				CLOSESOCKET(socket_peer);

			#if defined(_WIN32)
				WSACleanup();
			#endif

				printf("Finished.\n");
            	return 0;
            }
            if(strstr(read,"l"))
            {
            	int len = strlen(read);
            	printf("Player : ");
            	for(int i=1;i<len;i++)
            	{
            		if(read[i]>47 && read[i]<58)
            		{
            			printf("%c ",read[i]);
            		}
            	}
            	printf("\n");
            	//for(i=0;i<strl)
            	continue;
            }
            if(strstr(read,"draw"))
            {
            	printf("draw\n");
            	return 0;
            }
            sscanf(read, "%d %c", &num,&label);
		    if(num==1) board[0][0] = label;
		    else if(num==2) board[0][1] = label;
		    else if(num==3) board[0][2] = label;
		    else if(num==4) board[1][0] = label;
		    else if(num==5) board[1][1] = label;
		    else if(num==6) board[1][2] = label;
		    else if(num==7) board[2][0] = label;
		    else if(num==8) board[2][1] = label;
		    else if(num==9) board[2][2] = label;
            //printf("bytes_received = %d read = %s\n",bytes_received,read);
            if(read[2]=='0')  //for win
            {
            	printf(" %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
				printf("---+---+---\n");
				printf(" %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
				printf("---+---+---\n");
				printf(" %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);
            	printf("Lose\n");
            	return 0;
            }
            else
            {
            	sensor = 1;
		        printf(" %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
				printf("---+---+---\n");
				printf(" %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
				printf("---+---+---\n");
				printf(" %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);
				printf("R : Input your choice followed by enter.\n");
            }
            
            //printf("Received (%d bytes): %.*s",bytes_received, bytes_received, read);
        }
#if defined(_WIN32)
        if(_kbhit()) {
#else
        if(FD_ISSET(0, &reads)) {
#endif
            char read[4096];
            memset(read,'\0',4096);
            if(sensor_game)
            {
            	count++;
            }      
            //printf("count = %d\n",count);  
            if (!fgets(read, 4096, stdin)) break;
            if(read[strlen(read)-1]=='\n')
				read[strlen(read)-1]='\0';
            //printf("socket_peer = %d\n",socket_peer);
            if(strncmp(read,"send",4)==0)
			{
				//printf("send receive\n");
				send(socket_peer, read, strlen(read), 0);
				continue;
			}
            if(strstr(read,"invite"))
            {
            	send(socket_peer, read, strlen(read), 0);
            	continue;
            }
            if(strncmp(read,"y",1)==0)
            {
            	send(socket_peer, read, strlen(read), 0);
            	continue;
            }
            if(strncmp(read,"n",1)==0)
            {
            	send(socket_peer, read, strlen(read), 0);
            	continue;
            }
            if(strstr(read,"ls"))
            {
            	char forlist[2]={"ls"};
            	send(socket_peer, forlist, strlen(forlist), 0);
            	continue;
            }
            if(strstr(read,"leave"))
            {
            	send(socket_peer, read, strlen(read), 0);
            	break;
            }
            if(sensor==1)
            {
            	sscanf(read, "%d %c", &num,&label);
		        if(num==1) board[0][0] = label;
		        else if(num==2) board[0][1] = label;
		        else if(num==3) board[0][2] = label;
		        else if(num==4) board[1][0] = label;
		        else if(num==5) board[1][1] = label;
		        else if(num==6) board[1][2] = label;
		        else if(num==7) board[2][0] = label;
		        else if(num==8) board[2][1] = label;
		        else if(num==9) board[2][2] = label;
		        //printf("Sending: %s", read);
		        for(int i=0;i<3;i++)
		        {
		        	if(board[i][0]==board[i][1] && board[i][0]==board[i][2])
		        		win=1;
		        	else if(board[0][i]==board[1][i] && board[0][i]==board[2][i])
		        		win=1;
		        }
		        if(board[0][0]==board[1][1] && board[0][0]==board[2][2])
		        		win=1;
		        else if(board[0][2]==board[1][1] && board[0][2]==board[2][0])
		        	win=1;
		        	
		        if(win==1)
		        {
		        	printf(" %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
					printf("---+---+---\n");
					printf(" %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
					printf("---+---+---\n");
					printf(" %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);
		        	printf("win\n");
		        	read[2]='0';
		        	send(socket_peer, read, strlen(read), 0);
		        	sensor = 0;
		        	return 0;
		        }
		        else
		        {
					send(socket_peer, read, strlen(read), 0);
				    printf(" %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
					printf("---+---+---\n");
					printf(" %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
					printf("---+---+---\n");
					printf(" %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);
					if(count==5)
				    {
				    	char draw[4]={"draw"};
				    	printf("draw\n");
				    	send(socket_peer, draw, strlen(draw), 0);
				    	return 0;
				    }
					printf("Other turn\n");
					sensor = 0;
		        }
            }
            else
            {
            	printf("Please Wait\n");
            }
            //int bytes_sent = send(socket_peer, read, strlen(read), 0);
            //printf("Sent %d bytes.\n", bytes_sent);
        }
    } //end while(1)

    printf("Closing socket...\n");
    CLOSESOCKET(socket_peer);

#if defined(_WIN32)
    WSACleanup();
#endif

    printf("Finished.\n");
    return 0;
}

