
/*
*cap.h
*create by qinghuahan 2016/11/17
*/

#ifndef __CAP_H__
#define __CAP_H__

#include <endian.h>

/* ETH header */
typedef struct ETH_HEADER 
{
	unsigned char  ucDMac[6];
	unsigned char  ucSMac[6];
	unsigned short usEthType;
}ETHDR;


typedef struct _IP_HDR  
{
 #if __BYTE_ORDER == __LITTLE_ENDIAN  
	unsigned int ip_hl:4; /* header length */    
	unsigned int ip_v:4; /* version */
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
   unsigned int ip_v:4;  /*version*/
   unsigned int ip_hl:4; /* header length */
#endif    
   unsigned char ip_tos; /* type of service */    
   unsigned short ip_len; /* total length */    
   unsigned short ip_id; /* identification */    
   unsigned short ip_off; /* fragment offset field */
#define IP_RF 0x8000 /* reserved fragment flag */
#define IP_DF 0x4000 /* dont fragment flag */
#define IP_MF 0x2000 /* more fragments flag */
#define IP_OFFMASK 0x1fff /* mask for fragmenting bits */
   unsigned char ip_ttl; /* time to live */    
   unsigned char ip_p; /* protocol */    
   unsigned short ip_sum; /* checksum */    
   unsigned int  ip_src;    
   unsigned int  ip_dst; /* source and dest address */  
 }IP_HDR; 


/* TCP header*/
typedef struct _TCP_HDR {
  unsigned short th_sport; /* source port */
  unsigned short th_dport; /* destination port */
  unsigned int th_seq;   /* sequence number */
  unsigned int th_ack;   /* acknowledgement number */
#if __BYTE_ORDER == __LITTLE_ENDIAN
  unsigned char th_x2: 4; /* (unused) */
  unsigned char th_len: 4;  /* data len */
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
  unsigned char th_len:4; /* data len */
  unsigned char  th_x2 : 4;    /* (unused) */
#endif
  unsigned char th_flags;
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
#define TH_ECE 0x40
#define TH_CWR 0x80
#define	TH_FLAGS	(TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
  unsigned short th_win; /* window */
  unsigned short th_sum; /* checksum */
  unsigned short th_urp; /* urgent pointer */
}TCP_HDR;


/* Ip Packet Type */
#define TCP_PACKET 0x06
#define UDP_PACKET 0x11


#endif


