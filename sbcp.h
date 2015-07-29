/* 

Server code written by Team 26 
Naga Raghavedra Anudeep Tungala
Jiajun Yang

for ECEN 602 HW1
**/



#ifndef COMMON_H
#define COMMON_H

#define Protocol_Version 3;

#define SBCP_Size 4;

#define Attr_Size 4;

//extern char name[20][50];

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
};

enum{
  JOIN=2, FWD, SEND
} SBCP_Type;

enum{
  Reason=1, Username, ClientCount, Message
} Attr_Type;

#endif

