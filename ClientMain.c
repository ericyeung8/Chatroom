/* 

Server code written by Team 26 
Naga Raghavedra Anudeep Tungala
Jiajun Yang

for ECEN 602 HW1
**/




#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include"ClientFunc.c"

#define STDIN 0 //standard input number




int main(int argc, char **argv){

  int csd;
  struct sockaddr_in addr;
  struct hostent *hret;
  short port = 0;
  char buf[256];
  int n_bytes = 0;
  
  //struct timeval tv;
  fd_set sfds;
  fd_set Allfds;

  int s_temp = 0;
  int maxfd = 0;

  //tv.tv_sec = 10;
  //tv.tv_usec = 0;

  //initialize the socket
  if ((csd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("Cannot create client socket");
    exit(EXIT_FAILURE);
  }

  //port = atoi(argv[2]);
  port = atoi(argv[3]);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if ((hret = gethostbyname(argv[1+1])) == NULL){
    perror("Cannot get server's address");
    exit(EXIT_FAILURE);
  }
  memcpy(&addr.sin_addr.s_addr, hret->h_addr, hret->h_length);
  
  if (connect(csd,(struct sockaddr*)&addr, sizeof(struct sockaddr)) == -1){
    perror("Cannot connect to Server");
    exit(EXIT_FAILURE);
  }

  if (Join(csd, argv[3-2]) == -1){ //send join to server
    perror("Cannot join in the server!");
    exit(EXIT_FAILURE);
  }

  //initialize the select()
  FD_ZERO(&sfds);
  FD_SET(STDIN, &sfds);
  FD_SET(csd, &sfds);
  Allfds = sfds;

  if (STDIN < csd) maxfd = csd;
    else maxfd = STDIN; 

        /*memset(buf, 0, sizeof(buf));
        n_bytes = read(csd, buf, sizeof(buf));
        if (n_bytes == 0){
          perror("The other side has been closed.\n");
          exit(EXIT_FAILURE);
        }
        else
          unpackdata(buf);*/

  //loop: creat process to continue if the user doesn't type anything
  while(1){
    memset(buf, 0, sizeof(buf));

    sfds = Allfds;

    s_temp = select(maxfd+1, &sfds, NULL, NULL, NULL);
    if (/*select(maxfd+1, &sfds, NULL, NULL, &tv)*/s_temp == -1){
      perror("select error!");
      exit(EXIT_FAILURE);
    }
    
    int i;
    int j;
    for (i=0; i <= maxfd; ++i){
      if ((j = FD_ISSET(i, &sfds)) == 1){
        if (i == STDIN){
          memset(buf, 0, sizeof(buf));
          n_bytes = read(0, buf, sizeof(buf));
          if (SendMsg(csd, buf) == -1){
            perror("Send error!\n");
            exit(EXIT_FAILURE);
          }
        }
      else if (i == csd){
        memset(buf, 0, sizeof(buf));
        n_bytes = read(csd, buf, sizeof(buf));
        if (n_bytes == 0){
          perror("The other side has been closed.\n");
          exit(EXIT_FAILURE);
        }
        else
          unpackdata(buf);
      }  //end if i == csd
      }  //end read and send
    } //end search fd loop
  } //end main loop

  return 0;
}
