
/*
*pkt_parse.h
*create by qinghuahan 2016/11/17
*/
#ifndef __PKT_PARSE_H_
#define __PKT_PARSE_H_

#define REDIS_CMD_PARA_NUM 1
typedef struct _tagREDIS_CMD
{
	char cmd[REDIS_CMD_PARA_NUM][32];
	u_int cmd_num;
}REDIS_CMD;

typedef struct _tagDATA_DESC
{
	u_char *dataptr;
	u_short sPort;		//source port
	u_short dPort; //destination port
	u_short usflag;
	u_int sIp;			//source ip address
	u_int dIp;	//destination ip address
	u_int uidatalen;
}DATA_DESC;


typedef struct _tagPKTDUMP_INFO
{
	u_int sec;
	u_int u_sec;
	u_int ip;
	int (*recv)(void *intf,u_char *rbuf,int rlen);
}PKTDUMP_INFO;

int pkt_recv(void *PktdumpInfo,u_char *rbuf,int len);

#endif

