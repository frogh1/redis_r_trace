
#include "product.h"
#include "cap.h"
#include "pcap.h"
#include "pkt_parse.h"

#include "vhash.h"

typedef struct _tagCONFIG
{
	char device[64];//port name
	char ip[32];
}CONFIG;

CONFIG g_config;

int find_str(char *str,char *sub)
{
	char *p = str;
	int slen = strlen(str);
	int sublen = strlen(sub);
	int offset = slen - sublen;
	int i = 0;
	
	if(offset < 0)
	{
		return -1;
	}
	
	for(i ; i< offset; i++)
	{
		if(0 == strncmp(p,sub,sublen))
		{
			return (i+1);
		}
		p++;
	}
	return -1;
}

int check_self_vip(char *vip)
{
	char szbuf[2048] = {0};
	FILE *fp = popen("/sbin/ip addr","r");
	if(NULL != fp)
	{
		fread(szbuf,1,sizeof(szbuf),fp);
		pclose(fp);
	}

	if(0 != szbuf[0])
	{
		return find_str(szbuf,vip);
	}
	
	return -1;
}

int getInterfaceIp(char *device, unsigned int *ip)
{
 	int s;
	struct ifreq buffer;

	if( (s = socket( AF_INET, SOCK_DGRAM, 0)) < 0 )
	{
        printf("create socket error.");
        return -1;
	}
	memset(&buffer, 0x00, sizeof(buffer));
	buffer.ifr_addr.sa_family = AF_INET;
	strncpy( buffer.ifr_name, device, sizeof(buffer.ifr_name));
	int iret =0;
	iret = ioctl(s, SIOCGIFADDR, &buffer);
	if( iret < 0 )
	{
        printf("unable to query mac address of [%s].\n", device );
        close( s );
        return -1;
	}

	close(s);

	*ip = ntohl(((struct sockaddr_in *)&buffer.ifr_addr)->sin_addr.s_addr);
	return 0;
}


void GetAppDir(char *Apppath,char *AppDir)
{    char spec = '/';    
	unsigned long ulLen = 0;    
	char *p = strrchr(Apppath,spec);    
	if(NULL != p)    
	{        
		ulLen = (u_long)p - (u_long)Apppath;       
		memcpy(AppDir,Apppath,ulLen + 1);        
		AppDir[ulLen + 1] = 0;    
	}    
	else    
	{        
		AppDir[0] = 0;    
	}    
}

/*
./redis_r_trace -d -i eth0 -a 10.10.0.102

*/
int main(int argc,char *argv[])
{
	char c;
	int deamon = 1;
	char apppath[1024]={0};
	memset(&g_config,0,sizeof(CONFIG));
	if(argc > 1 )
	{
		int i = 1;
		for (i; i< argc; i ++)
		{
			if(!strcmp(argv[i], "-v"))
			{
				char szdate[] = "2016-11-30 17:00";
				printf("tcpflow_v0.1 %s %d bit\n",szdate,sizeof(unsigned long)== 4? 32:64);
				return 0;
			}
			else if(!strcmp(argv[i], "-d"))
			{
				deamon = 0;
			}
			else if(!strcmp(argv[i], "-i"))
			{
				strcpy(g_config.device,argv[i+1]);
				i++;
			}
			else if(!strcmp(argv[i], "-a"))
			{
				strcpy(g_config.ip,argv[i+1]);
				i++;
			}
		}
	}
	else
	{
		printf("arg error\n\n");
		return 0;
	}


	GetAppDir(argv[0],apppath);
	printf("apppath %s\n",apppath);

	if (1 == deamon)
    {
        daemon(1,0);
    }

	if (g_config.device[0] == 0 || g_config.ip[0] == 0)
	{
		printf("get configure error!\n\n");
		return 0;
	}

	printf("set config device %s ip %s\n",g_config.device,g_config.ip);

	if(-1 == check_self_vip(g_config.ip))
	{
		printf("set ip error\n");
		return 0;
	}

	PKTDUMP_INFO *pktdump = (PKTDUMP_INFO *)malloc(sizeof(PKTDUMP_INFO));
	memset(pktdump,0,sizeof(PKTDUMP_INFO));
	pktdump->ip = htonl(inet_addr(g_config.ip));
	pktdump->recv = pkt_recv;

	VHash_Init(en_VHASH_MOUDLE_MAX);
	PORT_VHash_INIT();
	
	pcap_t *handle;
	char errbuf[1024]={0};

	struct pcap_pkthdr *header;
	const u_char *pkt_data;
	unsigned char *ptmp = NULL;
	int res = 0;

	ETHDR *ethhdr= NULL;
	IP_HDR *iphdr = NULL;
	TCP_HDR *tcphdr = NULL;
	
	 struct bpf_program filter; 
	 char filter_cmd[256] = {0};
	 sprintf(filter_cmd,"tcp and not http and (ip.addr ==%s)",g_config.ip);
	 bpf_u_int32 mask; 
	 bpf_u_int32 net;

	 handle =  pcap_open_live(g_config.device,65536,1,100,errbuf);
	if(NULL == handle)
	{
		printf("%s\n", errbuf);
		return -1;
	}
	pcap_compile(handle, &filter, filter_cmd, 0, net);
    pcap_setfilter(handle, &filter);
	while((res = pcap_next_ex( handle, &header, &pkt_data )) >= 0)
	{
		if(res = 0)/* Timeout elapsed */	
		{
			continue;
		}
		if(header->caplen < 54)//discart small cap
		{
			continue;
		}

		ethhdr = (ETHDR *)pkt_data;
		if (ntohs(ethhdr->usEthType) != 0x0800)
		{
			continue;	
		}
		
		pktdump->sec = header->ts.tv_sec;
		pktdump->u_sec = header->ts.tv_usec;
		pktdump->recv((void*)pktdump,(u_char *)pkt_data,header->len);
		
	}

	return 0;
}




