
#include "product.h"
#include "pkt_parse.h"

#include "cap.h"
#include "port_vhash.h"

void pkt_dump(unsigned char *p_buf, int p_len)
{
	register const char *pref = ""; 	
	unsigned char *buf = p_buf;	
	unsigned int i = 0, width = 16;	
	unsigned int len = p_len;	
	for (i=1; i < len+1; i++)
	{
		printf("%2.2x ", (unsigned char)buf[i-1]);
		if(0 == (i % width))
			printf("\n");
	}
	printf("\n\n");
}		

int pparse_get_dataptr(u_char *rbuf,int irlen,DATA_DESC *pDataDesc)
{
	u_char *p = rbuf;
	IP_HDR *ipp;
	TCP_HDR *pTcp = NULL;
	
	int iOffset = 14;

	p = rbuf  + iOffset;
	
	ipp=(IP_HDR *)p;

	if(4 != ipp->ip_v)
	{
		return 0;
	}
	
	if (TCP_PACKET != ipp->ip_p)
	{
		return 0;
	}
	

    pDataDesc->sIp = pkt_swapu32(ipp->ip_src);
    pDataDesc->dIp = pkt_swapu32(ipp->ip_dst);


	iOffset += 20;
	p = rbuf + iOffset;

	pTcp = (TCP_HDR *)p;
	iOffset += (pTcp->th_len)<<2;

	
	if(iOffset >= irlen)
	{
		return 0;	
	}
	

    pDataDesc->sPort = pkt_swapu16(pTcp->th_sport);
    pDataDesc->dPort = pkt_swapu16(pTcp->th_dport);

	p = rbuf + iOffset;
	pDataDesc->dataptr = p;
	pDataDesc->uidatalen = irlen - iOffset;
	
	return 1;	
}

int redis_cmd_parse(u_char *pdata,int len,REDIS_CMD *pstRdsCmd)
{
	u_char *psrc = pdata;
	if(psrc[0] != '*')
	{
		return -1;
	}

	u_char *p1 =  psrc + 1;
	len -= 1;
	int i = 0;
	u_int r_para_num = 0;
	
	for (i; i < len; i++)
	{
		if(*(p1+i) == 0x0d && *(p1+i+1) == 0x0a)
		{
			break;
		}
	}
	u_char *p2 = p1 + i + 2;
	*(p1 + i) = '\0';
	r_para_num = atoi((char *)p1);

	if (len < i+2 || r_para_num == 0)
	{
		return -1;
	}
		
	len = len - i - 2; 
	p1 = p2;
	if (p1[0] != '$')
	{
		return -1;
	}
	p1 += 1;
	len -= 1;
	for (i = 0; i < len; i++)
	{
		if(*(p1+i) == 0x0d && *(p1+i+1) == 0x0a)
		{
			break;
		}
	}
	int cmd_len = 0;
	p2 = p1+i+2;
	*(p1+i) = '\0';
	cmd_len = atoi((char *)p1);

	if (len < i+2 || cmd_len == 0)
	{
		return -1;
	}

	len = len - i - 2; 
	p1 = p2;
	for (i = 0; i < len; i++)
	{
		if(*(p1+i) == 0x0d && *(p1+i+1) == 0x0a)
		{
			if(cmd_len != i )
			{
				return -1;
			}
			memcpy(pstRdsCmd->cmd[0],p1,cmd_len);
			pstRdsCmd->cmd_num = 1;
			break;
		}
	}
	return 0;
}

int pkt_recv(void *PktdumpInfo,u_char *rbuf,int len)
{
	PKTDUMP_INFO *dumpInfo = (PKTDUMP_INFO *)PktdumpInfo;
	u_char *pkt_data = rbuf;
	u_short port = 0;


	DATA_DESC stDataDesc;

	memset(&stDataDesc,0,sizeof(DATA_DESC));

	if(1 != pparse_get_dataptr(rbuf,len,&stDataDesc))
	{
		return 0;
	}

	
	if(stDataDesc.sIp != dumpInfo->ip && stDataDesc.dIp != dumpInfo->ip)
	{
		return 0;
	}
	stDataDesc.usflag = ((stDataDesc.sIp == dumpInfo->ip) ? en_OUT:en_IN);
	
	REDIS_CMD stRedisCmd;
	PORT_ITEM stPortItem;
	
	memset(&stRedisCmd,0,sizeof(REDIS_CMD));
	
	stPortItem.sport = stDataDesc.sPort;
	stPortItem.dport = stDataDesc.dPort;
	stPortItem.sip = stDataDesc.sIp;
	stPortItem.dip = stDataDesc.dIp;

	PORT_VHASH_ITEM *pstVhashItem = NULL;
	PORT_VHash_FindStruct(&stPortItem,&pstVhashItem);
	
	if(pstVhashItem != NULL)
	{
		u_long ula = dumpInfo->sec*1000000+dumpInfo->u_sec;
		u_long ulq = pstVhashItem->sec*1000000+pstVhashItem->u_sec;
		printf("redis cmd %s r %lu a %lu es %lu\n",pstVhashItem->cmd,ulq,\
			ula,ula - ulq);
		PORT_VHash_Del(&stPortItem);
	}
	else
	{
		stPortItem.sport = stDataDesc.dPort;
		stPortItem.dport = stDataDesc.sPort;
		stPortItem.sip = stDataDesc.dIp;
		stPortItem.dip = stDataDesc.sIp;
		PORT_VHash_FindStruct(&stPortItem,&pstVhashItem);
		if(pstVhashItem != NULL)
		{
			pstVhashItem->sec = dumpInfo->sec;
			pstVhashItem->u_sec = dumpInfo->u_sec;
		}
		else
		{
			if (0 != redis_cmd_parse(stDataDesc.dataptr,stDataDesc.uidatalen,&stRedisCmd))
			{
				return 0;
			}
			
			PORT_VHash_Add(&stPortItem,&pstVhashItem);
			if(pstVhashItem != NULL)
			{
				pstVhashItem->sec = dumpInfo->sec;
				pstVhashItem->u_sec = dumpInfo->u_sec;
				strncpy(pstVhashItem->cmd,stRedisCmd.cmd[0],32);
			}
		}

	}
	
    return 1;
}





