/* 

Server code written by Team 26 
Naga Raghavedra Anudeep Tungala
Jiajun Yang

for ECEN 602 HW1
**/



#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>



#define Protocol_Version 3;

#define SBCP_Size 4;

#define Attr_Size 4;



struct SBCP_Header{
#ifdef B16ENDIAN
  unsigned int Vrsn:9;
  unsigned int Type:7;
#else
  unsigned int Type:7;
  unsigned int Vrsn:9;
#endif
  unsigned int Length:16;
};//setup the SBCP Header

/*struct SBCP_Header{
  unsigned int Vrsn:9;
  unsigned int Type:7;
  unsigned int Length:16;
};*/

struct Attr_Header{
  unsigned int Type:16;
  unsigned int Length:16;
};//Setup the Attribute Header

enum{
  JOIN=2, FWD, SEND
} SBCP_Type;

enum{
  Reason=1, Username, ClientCount, Message
} Attr_Type;

/*int unpack(char *MsgBuf){

  char *tran = &MsgBuf[0];
  int *temp;

  uint16_t host,network;

  memcpy((char *)&host, (char *)(&tran), 2);
  network = ntohs(host);
  memcpy(temp, (const char *)&network,2);
  write(1,&temp,2);

  return 0;
}*/

int Join(int csd, char* UserName){

  struct Attr_Header attr;// initialize the attribute header
  attr.Type = Username;
  attr.Length = strlen(UserName) + Attr_Size;

  struct SBCP_Header header;// initialize the SBCP header
  header.Vrsn = Protocol_Version;
  header.Type = JOIN;
  header.Length = attr.Length + SBCP_Size;

  char MsgBuf[256];// Used to send data
  char *tran = &MsgBuf[0];

  uint16_t host,network;

  memcpy((char *)&host, (char *)(&header), 2);
  network = htons(host);
  memcpy(tran, (const char *)&network,2);
  tran += 2;//copy the first 2 bytes

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

  memcpy(tran, UserName, strlen(UserName));//copy the username
  tran += strlen(UserName);

  return send(csd, MsgBuf, header.Length, 0);//send the SBCP to server
}

int SendMsg(int csd, char *cMessage){// similar to Join
  struct Attr_Header attr;
  attr.Type = Message;
  attr.Length = strlen(cMessage) + Attr_Size;

  struct SBCP_Header header;
  header.Vrsn = Protocol_Version;
  header.Type = SEND;
  header.Length = attr.Length + SBCP_Size;

  char MsgBuf[256];
  char *tran = &MsgBuf[0];
  
  uint16_t host,network;

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

  return send(csd, MsgBuf, header.Length, 0);
}  


void unpackdata(char *MsgBuf){
  char *tran = &MsgBuf[0];

  char Data[256]="";//initialize the unpack array

  uint16_t host, network;

  //uint16_t SBCP_Header_Vrsn, SBCP_Header_Type;
  //uint16_t SBCP_Header_Length, Attr_Type;
  uint16_t Attr_Length;//used to store data from the packet

  memcpy((char *)&network, (char *)tran, 2);
  host = ntohs(network);
  //SBCP_Header_Vrsn = host & 0x000f;//get the SBCP Vrsn

  memcpy((char *)&network, (char *)tran, 2);
  host = ntohs(network);
  host = host & 0xff00;
  //SBCP_Header_Type = host >> 9;//get the SBCP Type
  tran += 2;

  memcpy((char *)&network, (char *)tran, 2);
  host = ntohs(network);
  //SBCP_Header_Length = host;//get the SBCP Length
  tran += 2;

  memcpy((char *)&network, (char *)tran, 2);
  host = ntohs(network);
  //Attr_Type = host;//get the attribute type
  tran += 2;

  memcpy((char *)&network, (char *)tran, 2);
  host = ntohs(network);
  Attr_Length = host;//get the attribute length
  tran += 2;

  int length;
  length = Attr_Length - Attr_Size;//calculate the length of data/msg
  memcpy(Data, tran, length);
  Data[length+1] = '\0';

  write(1, Data, length+1);//print in the screen

}
