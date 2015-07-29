/* 

Server code written by Team 26 
Naga Raghavedra Anudeep Tungala
Jiajun Yang

for ECEN 602 HW1
**/


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdarg.h>
#include "sbcp.h"
char name[20][50]={}; //global variable to store username 
void FwdPackData(fd_set* xbuffFds,int csd,int fdmax, char *cMessage,int servsk){ //function to pack the data in SBCP protocol and send
	int j=0;
	int s_bytes=0;
  fd_set buffFds;
  FD_ZERO(&buffFds);
	buffFds = *xbuffFds;
  struct Attr_Header attr;
  attr.Type = Message;
  attr.Length = strlen(cMessage) + Attr_Size;

  struct SBCP_Header header;
  header.Vrsn = Protocol_Version;
  header.Type = FWD;
  header.Length = attr.Length + SBCP_Size;

  char MsgBuf[256];
  char *tran = &MsgBuf[0];
  
  uint16_t host,network; //Data Packing into SBCP

  memcpy((char *)&host, (char *)&header, 2);
  network = htons(host);
  memcpy(tran, (const char *)&network, 2);
  tran += 2;

  memcpy((char *)&host, ((char *)(&header))+2, 2);
  network = htons(host);
  memcpy(tran, (const char *)&network, 2);
  tran += 2;

  memcpy((char *)&host, (char *)&attr, 2);
  network = htons(host);
  memcpy(tran, (const char *)&network,2);
  tran += 2;

  memcpy((char *)&host, ((char *)(&attr))+2, 2);
  network = htons(host);
  memcpy(tran, (const char *)&network, 2);
  tran += 2;

  memcpy(tran, cMessage,strlen(cMessage));
  for(j = 0; j <= fdmax+1; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &buffFds)) {
                                // except the source and ourselves
                                if (j != csd && j!=servsk) {
					s_bytes = send(j, MsgBuf, header.Length, 0);
                                    if (s_bytes == -1) {
                                        perror("send");
                                    }
                                }
                            }
              }
  
  //return 0;
}

void FwdErrorData(fd_set* xbuffFds,int csd,int fdmax, char *cMessage,int servsk){ //function to pack the Error data in SBCP protocol and send
	
  fd_set buffFds;
  FD_ZERO(&buffFds);
	buffFds = *xbuffFds;
  struct Attr_Header attr;
  attr.Type = Message;
  attr.Length = strlen(cMessage) + Attr_Size;

  struct SBCP_Header header;
  header.Vrsn = Protocol_Version;
  header.Type = FWD;
  header.Length = attr.Length + SBCP_Size;

  char MsgBuf[256];
  char *tran = &MsgBuf[0];
  
  uint16_t host,network; //Data Packing into SBCP

  memcpy((char *)&host, (char *)&header, 2);
  network = htons(host);
  memcpy(tran, (const char *)&network, 2);
  tran += 2;

  memcpy((char *)&host, ((char *)(&header))+2, 2);
  network = htons(host);
  memcpy(tran, (const char *)&network, 2);
  tran += 2;

  memcpy((char *)&host, (char *)&attr, 2);
  network = htons(host);
  memcpy(tran, (const char *)&network,2);
  tran += 2;

  memcpy((char *)&host, ((char *)(&attr))+2, 2);
  network = htons(host);
  memcpy(tran, (const char *)&network, 2);
  tran += 2;

  memcpy(tran, cMessage,strlen(cMessage));
  
	send(csd, MsgBuf, header.Length, 0);
  
  //return 0;
}

int unpack_Joindata(fd_set* buffFds,int fd,int fdmax,char *MsgBuf,int servsk){// function to unpack the data from SBCP format
  char *tran = &MsgBuf[0];

  int j=0;
  fd_set xbuffFds;
  FD_ZERO(&xbuffFds);
	xbuffFds = *buffFds;
  char Data[256]="";

  uint16_t host, network;

  uint16_t SBCP_Header_Vrsn, SBCP_Header_Type, Attr_Type, Attr_Length;

  //uint16_t SBCP_Header_Length;

  memcpy((char *)&network, (char *)tran, 2); //methods to unpack data
  host = ntohs(network);
  host = host & 1111111110000000;
  SBCP_Header_Vrsn = host >> 7;

  memcpy((char *)&network, (char *)tran, 2);
  host = ntohs(network);
  SBCP_Header_Type = host & 0x003f;
 
  tran += 2;

  memcpy((char *)&network, (char *)tran, 2);
  host = ntohs(network);
  //SBCP_Header_Length = host;
  tran += 2;

  memcpy((char *)&network, (char *)tran, 2);
  host = ntohs(network);
  Attr_Type = host;
  tran += 2;

  memcpy((char *)&network, (char *)tran, 2);
  host = ntohs(network);
  Attr_Length = host;
  tran += 2;

  //int length;
  //length = Attr_Length - Attr_Size;
 
  if(SBCP_Header_Vrsn == 3 && SBCP_Header_Type == 2 && Attr_Type == 2) { //routing for new conections
	char s[]=" is Online\n";
        char Data_temp[]="";
	char Name_temp[]="";
	memcpy(Name_temp, tran, Attr_Length - 4);
	for(j=0;j<=20;j++) {
		if(strcmp(name[j],Name_temp)==0)
		{
		
		return 0;
		
		}
	}
	strcpy(name[fd],Name_temp);
        tran += Attr_Length - 4;
        memcpy(tran, s, strlen(s));
        memcpy(Data_temp, tran-Attr_Length+4, Attr_Length - 4 + strlen(s));

        FwdPackData(&xbuffFds,fd,fdmax,Data_temp,servsk); //calling the function to send about user online information
	
	
        tran = tran - (Attr_Length - 4 + strlen(s));
	return 1;
  }

  if(SBCP_Header_Vrsn == 3 && SBCP_Header_Type == 4 && Attr_Type == 4) {//routing for the message types
        memcpy(Data, tran, Attr_Length - 4);
	char del_temp[256] = "";
	char del_m[]="-";
			
	memcpy(del_temp, name[fd], strlen(name[fd]));
	memcpy(&del_temp[strlen(name[fd])],del_m, strlen(del_m));
	memcpy(&del_temp[strlen(name[fd])+strlen(del_m)],Data,strlen(Data));
	//printf("%s",del_temp);
	del_temp[strlen(Data)+strlen(del_m)+strlen(name[fd])] = '\0';
	FwdPackData(&xbuffFds,fd,fdmax,del_temp,servsk); //calling the function to send recvd data as fwd data
	return 2;
  }
  else return 0;
  
}





int main(int argc, char** argv) {  //Main movie starts here
	
	if(argc != 4)  { //check for proper user inputs on command line arg

		printf("Please enter in the format ./server ip port clientcount\n");
		return(1);
	}
		
	int max_fd = 0;	
	int servSocket=0;
	int newFd = 0;
	int nbytes = 0;
	char buf[256]="";    // buffer for client data
	
        //char remoteIP[INET6_ADDRSTRLEN];
    	int i,j;
	fd_set readFds;
	fd_set connectedFds;
	fd_set writeFds;
	FD_ZERO(&readFds);
	FD_ZERO(&connectedFds);
	FD_ZERO(&writeFds);
	struct addrinfo temp;
	struct addrinfo *res;
	struct sockaddr_storage remoteaddr;
	socklen_t addrlen;

        memset(&temp, 0, sizeof(temp));
        temp.ai_family = AF_UNSPEC;
        temp.ai_socktype = SOCK_STREAM;
       
        int addrVal;

        if((addrVal = getaddrinfo(argv[1],argv[2], &temp, &res)) != 0)
        {
                perror("getaddrinfo() error: ");
                exit(1);
        }

        //create server main(connection request handler) socket
        servSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(servSocket == -1)
        {
                perror("socket() error: ");
                exit(1);
        }

        //to prevent address in use error messages after server crashes
        int yes = 1;
        if(setsockopt(servSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
                perror("setsockopt() error: ");
                exit(1);
        }

        if(bind(servSocket, res->ai_addr, res->ai_addrlen) == -1)
        {
                perror("bind() error: ");
                close(servSocket);
                exit(1);
        }

        freeaddrinfo(res);

        printf("Server started successfully!\n");

        struct sockaddr_in tempaddr;
        unsigned int templen = sizeof(struct sockaddr);
        getsockname(servSocket, (struct sockaddr*)&tempaddr, &templen);
        printf("Server listens to port: %d\n",ntohs(tempaddr.sin_port));
	
	if (listen(servSocket, 10) == -1) {
        perror("listen");
        exit(1);
   	}

	FD_SET(servSocket, &connectedFds);	
	max_fd = servSocket;

	for(;;) {
        strcpy(buf,"");
	FD_ZERO(&readFds);
        readFds = connectedFds; // copy it
        if (select(max_fd+1, &readFds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        for(i = 0; i <= max_fd+1; i++) { //iterations for polling the connected clients
            if (FD_ISSET(i, &readFds)) { // we got one!!
                if (i == servSocket) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newFd = accept(servSocket,(struct sockaddr *)&remoteaddr,&addrlen);
                    if (newFd == -1) {
                        perror("accept");

                    } else if(newFd < (atoi(argv[3])+servSocket+1)){
			nbytes = recv(newFd, buf, sizeof buf, 0);
			if(nbytes!=0) {
			if(unpack_Joindata(&connectedFds,newFd,max_fd,buf,servSocket)==1){
                        FD_SET(newFd, &connectedFds); // add to master connected set
                        if (newFd > max_fd) {    // keep track of the max
                            max_fd = newFd;
			for(j=0;j<=max_fd+1;j++) {
				if (FD_ISSET(j, &connectedFds) && j!=servSocket) {
					char del_temp[] = "";
					char del_m[]=" is available!\n";
					printf("%d",j);
					memcpy(del_temp, name[j], strlen(name[j]));
					memcpy(&del_temp[strlen(name[j])],del_m, strlen(del_m));
					del_temp[strlen(name[j])+strlen(del_m)] = '\0';
					FwdErrorData(&connectedFds,newFd,max_fd,del_temp,servSocket);					
					
					
				}//end for if
			}//close for for
                        }
		      }//close for unpack_Joindata
			else{
				char u_exist[]="User Name already exist dude!!!\n";
				FwdErrorData(&connectedFds,newFd,max_fd,u_exist,servSocket);
				close(newFd); // getlost!
                        	
			}
                     }   
                    }else {
			//char s[] ="Server out of capacity\n";
			//write(newFd, s, strlen(s));
			close(newFd);
		    }
		
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
			  
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
			
			char del_temp[] = "";
			char del_m[]=" is offline!\n";
			
			memcpy(del_temp, name[i], strlen(name[i]));
			memcpy(&del_temp[strlen(name[i])],del_m, strlen(del_m));
			del_temp[strlen(name[i])+strlen(del_m)] = '\0';
			FwdPackData(&connectedFds,i,max_fd,del_temp,servSocket);
			strcpy(name[i],"");
                        FD_CLR(i, &connectedFds); // remove from master set
                    } else {
                        // we got some data from a client
			if(unpack_Joindata(&connectedFds,i,max_fd,buf,servSocket)==2){//calling to fwd the recvd message to other clients
				//for any future additions
			}//end Unpack
                      
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    }
  
	return 0;	
                
}//close of main

